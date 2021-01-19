#include "ble_cus.h"
#include <string.h>
#include "ble_srv_common.h"
#include "boards.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "sdk_common.h"

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_cus_t *p_cus, ble_evt_t const *p_ble_evt)
{
    p_cus->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    ble_cus_evt_t evt;

    evt.evt_type = BLE_CUS_EVT_CONNECTED;

    p_cus->evt_handler(p_cus, &evt);
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_cus_t *p_cus, ble_evt_t const *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;

    ble_cus_evt_t evt;

    evt.evt_type = BLE_CUS_EVT_DISCONNECTED;

    p_cus->evt_handler(p_cus, &evt);
}

static uint8_t challengeRequest[] = {0x03, 0x10, 0x12, 0x34};
static uint8_t someOtherThing[] = {0x03, 0x11, 0xff, 0xff};

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_cus       Custom Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_cus_t *p_cus, ble_evt_t const *p_ble_evt)
{
    ble_gatts_evt_write_t const *p_evt_write =
        &p_ble_evt->evt.gatts_evt.params.write;
    // Custom Value Characteristic Written to.
    if (p_evt_write->handle == p_cus->rx_handles.value_handle)
    {
        // if we got 0x0310
        if (p_evt_write->data[0] == 0x03 && p_evt_write->data[1] == 0x10)
        {
            // issue the challenge of 0x0310yyyy on 0x0032
            NRF_LOG_INFO("got request for challenge");
            ble_cus_tx_value_update(p_cus, challengeRequest, 4);
            // APP_ERROR_CHECK(app_sched_event_put(NULL, 0,
            // send_challenge_response));
        }
        if (p_evt_write->data[0] == 0x03 && p_evt_write->data[1] == 0x11)
        {
            // emit 0x0311ffff on 0x0032
            NRF_LOG_INFO("got thing2");
            ble_cus_tx_value_update(p_cus, someOtherThing, 4);
            // tell app to start firing off steering data
            ble_cus_evt_t evt;
            evt.evt_type = BLE_CUS_START_SENDING_STEERING_DATA;

            // Call the application event handler.
            p_cus->evt_handler(p_cus, &evt);
        }
    }
}

void ble_cus_on_ble_evt(ble_evt_t const *p_ble_evt, void *p_context)
{
    ble_cus_t *p_cus = (ble_cus_t *)p_context;

    NRF_LOG_INFO("BLE event received. Event type = %d\r\n",
                 p_ble_evt->header.evt_id);
    if (p_cus == NULL || p_ble_evt == NULL)
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_cus, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_cus, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_cus, p_ble_evt);
            break;
            /* Handling this event is not necessary
                    case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
                        NRF_LOG_INFO("EXCHANGE_MTU_REQUEST event
               received.\r\n"); break;
            */
        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for adding the Custom Value characteristic.
 *
 * @param[in]   p_cus        Battery Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t custom_value_char_add(ble_cus_t *           p_cus,
                                      const ble_cus_init_t *p_cus_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.write_perm = p_cus_init->custom_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.notify = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = &cccd_md;
    char_md.p_sccd_md = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = STEERER_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm = p_cus_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->custom_value_char_attr_md.write_perm;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(float);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = sizeof(float);

    err_code = sd_ble_gatts_characteristic_add(p_cus->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_cus->steerer_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the Custom Value characteristic.
 *
 * @param[in]   p_cus        Battery Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t tx_char_add(ble_cus_t *p_cus, const ble_cus_init_t *p_cus_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.write_perm = p_cus_init->custom_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.indicate = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = &cccd_md;
    char_md.p_sccd_md = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = TX_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm = p_cus_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->custom_value_char_attr_md.write_perm;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = 4;

    err_code = sd_ble_gatts_characteristic_add(
        p_cus->service_handle, &char_md, &attr_char_value, &p_cus->tx_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**@brief Function for adding the Custom Value characteristic.
 *
 * @param[in]   p_cus        Battery Service structure.
 * @param[in]   p_cus_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t rx_char_add(ble_cus_t *p_cus, const ble_cus_init_t *p_cus_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Add Custom Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);

    cccd_md.write_perm = p_cus_init->custom_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.write = 1;
    char_md.p_char_user_desc = NULL;
    char_md.p_char_pf = NULL;
    char_md.p_user_desc_md = NULL;
    char_md.p_cccd_md = &cccd_md;
    char_md.p_sccd_md = NULL;

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = RX_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm = p_cus_init->custom_value_char_attr_md.read_perm;
    attr_md.write_perm = p_cus_init->custom_value_char_attr_md.write_perm;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len = 4;

    err_code = sd_ble_gatts_characteristic_add(
        p_cus->service_handle, &char_md, &attr_char_value, &p_cus->rx_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

uint32_t ble_cus_init(ble_cus_t *p_cus, const ble_cus_init_t *p_cus_init)
{
    if (p_cus == NULL || p_cus_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_cus->evt_handler = p_cus_init->evt_handler;
    p_cus->conn_handle = BLE_CONN_HANDLE_INVALID;

    // Add Custom Service UUID
    ble_uuid128_t base_uuid = {STEERER_SERVICE_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_cus->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = STEERER_SERVICE_UUID;

    // Add the Custom Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid,
                                        &p_cus->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Custom Value characteristic
    custom_value_char_add(p_cus, p_cus_init);
    rx_char_add(p_cus, p_cus_init);
    tx_char_add(p_cus, p_cus_init);
}

uint32_t ble_cus_tx_value_update(ble_cus_t *p_cus, uint8_t *custom_value,
                                 uint8_t data_len)
{
    if (p_cus == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t          err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len = data_len;
    gatts_value.offset = 0;
    gatts_value.p_value = custom_value;

    // Update database.
    err_code = sd_ble_gatts_value_set(
        p_cus->conn_handle, p_cus->tx_handles.value_handle, &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Send value if connected and notifying.
    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID))
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_cus->tx_handles.value_handle;
        hvx_params.type = BLE_GATT_HVX_INDICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n");
    }

    return err_code;
}

uint32_t ble_cus_steering_value_update(ble_cus_t *p_cus, float angle)
{
    if (p_cus == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t          err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len = 4;
    gatts_value.offset = 0;
    gatts_value.p_value = &angle;

    // Update database.
    err_code = sd_ble_gatts_value_set(
        p_cus->conn_handle, p_cus->steerer_handles.value_handle, &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Send value if connected and notifying.
    if ((p_cus->conn_handle != BLE_CONN_HANDLE_INVALID))
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_cus->steerer_handles.value_handle;
        hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_cus->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n");
    }

    return err_code;
}
