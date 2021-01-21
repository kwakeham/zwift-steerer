/**
 * @file lsm6dsm.c
 * @author Keith Wakeham (keith@titanlab.co)
 * @brief This module uses nrfx_spim to interface and talk to an LSM6DSM
 * 
 * @version 0.1
 * @date 2020-09-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef LSM6DSM_LOG_ENABLED
#define LSM6DSM_LOG_ENABLED 1
#endif

#define NRF_LOG_MODULE_NAME LSM6DSM

#include "boards.h"
#include "nrf_assert.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrfx_spim.h"
#include "nrf_drv_gpiote.h"
#include "handler.h"

NRF_LOG_MODULE_REGISTER();

#include "lsm6dsm.h"

// #define SPI_SLEEP

uint32_t *spi_lsm6_base_address;
bool spim_lsm6_xfer_done = true;

static uint8_t m_lsm_tx_buf[LSM_TX_RX_MSG_LENGTH]; /*!< SPI TX buffer */
static uint8_t m_lsm_rx_buf[LSM_TX_RX_MSG_LENGTH]; /*!< SPI RX buffer */

static uint8_t m_lsm_register_payload; /*!< SPI TX buffer */

//Flags
bool _lsm_readdatacmd_flag = false;
bool _lsm_display_flag = false;

bool _lsm_readdata_flag = false;

#ifdef SPI_SLEEP
        bool _lsm_spi_enable = false;
#endif

//data

int16_t lsm6dsm_gyro[3];
int16_t lsm6dsm_acc[3];

#define SPI_INSTANCE 0 /**< SPI instance index. */
static const nrfx_spim_t lsm6_spi = NRFX_SPIM_INSTANCE(SPI_INSTANCE);
static nrfx_spim_config_t lsm_spi_config = NRFX_SPIM_DEFAULT_CONFIG;


static void spim_lsm6_event_handler(nrfx_spim_evt_t const *p_event,void *p_context)
{
    spim_lsm6_xfer_done = true;
    if(_lsm_readdata_flag)
    {
        _lsm_readdata_flag = false;
        _lsm_display_flag = true; //this will be used to display the data
        lsm6dsm_getdata(lsm6dsm_gyro, lsm6dsm_acc);
    }
}

void lsm6dsm_wait_spi_clear(void)
{
    while (!spim_lsm6_xfer_done)
    {
        __WFE();
    }
    return;
}

void lsm6dsm_write_register_confirm(uint8_t lsm_register, uint8_t lsm_payload)
{
    m_lsm_register_payload = lsm_payload;
    lsm6dsm_comm(LSM6DSM_WRITE_FLAG, lsm_register, 1, &m_lsm_register_payload);
    lsm6dsm_wait_spi_clear();
    lsm6dsm_comm(LSM6DSM_READ_FLAG, lsm_register, 1, NULL);
    lsm6dsm_wait_spi_clear();
    if(m_lsm_rx_buf[1] == m_lsm_register_payload)
    {
        NRF_LOG_DEBUG("Register confirmed")
    }
    else
    {
        NRF_LOG_ERROR("Write or read error")
    }
}

void lsm6dsm_read_register(uint8_t lsm_register)
{
    lsm6dsm_comm(LSM6DSM_READ_FLAG, lsm_register, 1, NULL);
}

void lsm6dsm_write_register(uint8_t lsm_register, uint8_t lsm_payload)
{
    m_lsm_register_payload = lsm_payload;
    lsm6dsm_comm(LSM6DSM_WRITE_FLAG, lsm_register, 1, &m_lsm_register_payload);
}

void lsm6dsm_comm(uint8_t read_write, uint8_t lsm_register, uint8_t length, uint8_t *payload)
{
    m_lsm_tx_buf[0] = read_write | lsm_register; //

    if(payload != NULL)
    {
        if (read_write == LSM6DSM_READ_FLAG)
        {
            for (uint8_t i=0;i<length;i++)
            {
                m_lsm_rx_buf[i+1] = payload[i];
            }
        }
        else
        {
            for (uint8_t i=0;i<length;i++)
            {
                m_lsm_tx_buf[i+1] = payload[i];
            }
        }

    } else
    {
        // NRF_LOG_DEBUG("It's a null");
    }
    nrfx_spim_xfer_desc_t const spi_xfer_desc3 =
        {
            .p_tx_buffer = m_lsm_tx_buf,
            .tx_length = length+1,
            .p_rx_buffer = m_lsm_rx_buf,
            .rx_length = length+1,
        };
    while (!spim_lsm6_xfer_done)
    {
        __WFE();
    }
    spim_lsm6_xfer_done = false;
    APP_ERROR_CHECK(nrfx_spim_xfer(&lsm6_spi, &spi_xfer_desc3, 0));

}


void lsm6dsm_reset()
{
    lsm6dsm_write_register(LSM6DSM_CTRL3_C, 0x01); // Set bit 0 to 1 to reset LSM6DSM
    nrf_delay_ms(20); // Wait for all registers to reset 
}

void lsm_int1_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    _lsm_readdatacmd_flag = true;
}


void lsm6dsm_init()
{
    ret_code_t err_code;

    lsm_spi_config.frequency = NRF_SPIM_FREQ_8M; //Gotta go fast
    lsm_spi_config.mode = NRF_SPIM_MODE_3;       // Looks like CPOL = 1, CPHA = 1, page 28 of datasheet
    lsm_spi_config.sck_pin = LSM6DSM_SCK_PIN;
    lsm_spi_config.miso_pin = LSM6DSM_MISO_PIN;
    lsm_spi_config.mosi_pin = LSM6DSM_MOSI_PIN;
    lsm_spi_config.ss_pin = LSM6DSM_SS;
    lsm6dsm_spi_init();
#ifdef SPI_SLEEP
    _lsm_spi_enable = true;
#endif
    lsm6dsm_start();

    // Configure interrupts
    nrf_drv_gpiote_in_config_t in_config3 = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config3.pull = NRF_GPIO_PIN_NOPULL;

    err_code = nrf_drv_gpiote_in_init(LSM6DSM_INT1, &in_config3, lsm_int1_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(LSM6DSM_INT1, true);

    NRF_LOG_INFO("LSM6DSM Init done");
    NRF_LOG_FLUSH();
}

void lsm6dsm_spi_init()
{
    ret_code_t err_code;
#ifdef SPI_SLEEP
    _lsm_spi_enable = true;
#endif
    err_code = nrfx_spim_init(&lsm6_spi, &lsm_spi_config, spim_lsm6_event_handler, NULL);
    APP_ERROR_CHECK(err_code);
}

void lsm6dsm_start()
{
    lsm6dsm_hwtest();
    NRF_LOG_INFO("LSM6DSM HW test");
    // NRF_LOG_FLUSH();
    // lsm6dsm_reset();
    
    //Set acc_sensitivity
    // _inst.acc_sensitivity  = 16;
    lsm6dsm_write_register_confirm(LSM6DSM_CTRL1_XL, (LSM6DSM_AODR_1660Hz << 4) | (LSM6DSM_AFS_2G<<2));
    // lsm6dsm_write_register_confirm(LSM6DSM_CTRL1_XL, (LSM6DSM_AODR_12_5Hz << 4) | (LSM6DSM_AFS_2G<<2));

    //Set gyro_sensitivity
    // _inst.gyro_sensitivity = 2000;
    lsm6dsm_write_register_confirm(LSM6DSM_CTRL2_G, (LSM6DSM_GODR_1660Hz << 4) | (LSM6DSM_GFS_2000DPS<<2));
    //  lsm6dsm_write_register_confirm(LSM6DSM_CTRL2_G, (LSM6DSM_GODR_12_5Hz << 4) | (LSM6DSM_GFS_2000DPS<<2));

    // NRF_LOG_PROCESS();
    // enable block update (bit 6 = 1), auto-increment registers (bit 2 = 1)
    lsm6dsm_write_register_confirm(LSM6DSM_CTRL3_C, 0x40 | 0x04);

    // enable accel LP2 (bit 7 = 1), set LP2 tp ODR/9 (bit 6 = 1), enable input_composite (bit 3) for low noise
    //Keith note: this is probably fine for now for testing
    lsm6dsm_write_register_confirm(LSM6DSM_CTRL8_XL, 0x80 | 0x40 | 0x08 );

    // interrupt handling
    // latch interrupt until data read
    lsm6dsm_write_register_confirm(LSM6DSM_DRDY_PULSE_CFG, 0x80); 

    // enable accel/gyro interrupts on INT1
    lsm6dsm_write_register_confirm(LSM6DSM_INT1_CTRL, 0x03);
    NRF_LOG_FLUSH();

#ifdef SPI_SLEEP
    _lsm_spi_enable = false;
    nrfx_spim_uninit(&lsm6_spi);
    nrf_spim_event_clear(lsm6_spi.p_reg, NRF_SPIM_EVENT_END);
    // nrf_gpio_pin_write(LSM6DSM_SS, 1);
#endif
}


void lsm6dsm_readdata()
{
#ifdef SPI_SLEEP
    if(!_lsm_spi_enable)
    {
        lsm6dsm_spi_init();
    }
#endif
    // NRF_LOG_INFO("read");
    lsm6dsm_comm(LSM6DSM_READ_FLAG ,LSM6DSM_OUT_TEMP_L,14, NULL);
    _lsm_readdata_flag = true;
}

void lsm6dsm_getdata(int16_t *gyro, int16_t *acc)
{    
    gyro[0] = m_lsm_rx_buf[4]<<8 | m_lsm_rx_buf[3];
    gyro[1] = m_lsm_rx_buf[6]<<8 | m_lsm_rx_buf[5];
    gyro[2] = m_lsm_rx_buf[8]<<8 | m_lsm_rx_buf[7];
    acc[0] = m_lsm_rx_buf[10]<<8 | m_lsm_rx_buf[9];
    acc[1] = m_lsm_rx_buf[12]<<8 | m_lsm_rx_buf[11];
    acc[2] = m_lsm_rx_buf[14]<<8 | m_lsm_rx_buf[13];
}


void lsm6dsm_hwtest()
{
    lsm6dsm_read_register(LSM6DSM_WHO_AM_I);
}

void lsm6dsm_task()
{
    if (_lsm_readdatacmd_flag)
    {
        lsm6dsm_readdata();
        _lsm_readdatacmd_flag = false;
        // NRF_LOG_INFO("readflag");
    }
    if(_lsm_display_flag)
    {
        // NRF_LOG_INFO("display flag");
        _lsm_display_flag = false;
#ifdef SPI_SLEEP
        _lsm_spi_enable = false;
        nrfx_spim_uninit(&lsm6_spi);
        nrf_spim_event_clear(lsm6_spi.p_reg, NRF_SPIM_EVENT_END);
#endif
        handler_accel_data(lsm6dsm_acc, lsm6dsm_gyro);
    }
}