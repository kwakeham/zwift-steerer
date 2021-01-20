#ifndef LSM6DSM_H
#define LSM6DSM_H


#define LSM_TX_RX_MSG_LENGTH   100

#define LSM6DSM_WRITE_FLAG              0x00
#define LSM6DSM_READ_FLAG               0x80

/* LSM6DSM registers
  http://www.st.com/content/ccc/resource/technical/document/datasheet/76/27/cf/88/c5/03/42/6b/DM00218116.pdf/files/DM00218116.pdf/jcr:content/translations/en.DM00218116.pdf
*/
#define LSM6DSM_FUNC_CFG_ACCESS           0x01
#define LSM6DSM_SENSOR_SYNC_TIME_FRAME    0x04
#define LSM6DSM_SENSOR_SYNC_RES_RATIO     0x05
#define LSM6DSM_FIFO_CTRL1                0x06
#define LSM6DSM_FIFO_CTRL2                0x07
#define LSM6DSM_FIFO_CTRL3                0x08
#define LSM6DSM_FIFO_CTRL4                0x09
#define LSM6DSM_FIFO_CTRL5                0x0A
#define LSM6DSM_DRDY_PULSE_CFG            0x0B
#define LSM6DSM_INT1_CTRL                 0x0D
#define LSM6DSM_INT2_CTRL                 0x0E
#define LSM6DSM_WHO_AM_I                  0x0F  // should be 0x6A
#define LSM6DSM_CTRL1_XL                  0x10
#define LSM6DSM_CTRL2_G                   0x11
#define LSM6DSM_CTRL3_C                   0x12
#define LSM6DSM_CTRL4_C                   0x13
#define LSM6DSM_CTRL5_C                   0x14
#define LSM6DSM_CTRL6_C                   0x15
#define LSM6DSM_CTRL7_G                   0x16
#define LSM6DSM_CTRL8_XL                  0x17
#define LSM6DSM_CTRL9_XL                  0x18
#define LSM6DSM_CTRL10_C                  0x19
#define LSM6DSM_MASTER_CONFIG             0x1A
#define LSM6DSM_WAKE_UP_SRC               0x1B
#define LSM6DSM_TAP_SRC                   0x1C
#define LSM6DSM_D6D_SRC                   0x1D
#define LSM6DSM_STATUS_REG                0x1E
#define LSM6DSM_OUT_TEMP_L                0x20
#define LSM6DSM_OUT_TEMP_H                0x21
#define LSM6DSM_OUTX_L_G                  0x22
#define LSM6DSM_OUTX_H_G                  0x23
#define LSM6DSM_OUTY_L_G                  0x24
#define LSM6DSM_OUTY_H_G                  0x25
#define LSM6DSM_OUTZ_L_G                  0x26
#define LSM6DSM_OUTZ_H_G                  0x27
#define LSM6DSM_OUTX_L_XL                 0x28
#define LSM6DSM_OUTX_H_XL                 0x29
#define LSM6DSM_OUTY_L_XL                 0x2A
#define LSM6DSM_OUTY_H_XL                 0x2B
#define LSM6DSM_OUTZ_L_XL                 0x2C
#define LSM6DSM_OUTZ_H_XL                 0x2D
#define LSM6DSM_SENSORHUB1_REG            0x2E
#define LSM6DSM_SENSORHUB2_REG            0x2F
#define LSM6DSM_SENSORHUB3_REG            0x30
#define LSM6DSM_SENSORHUB4_REG            0x31
#define LSM6DSM_SENSORHUB5_REG            0x32
#define LSM6DSM_SENSORHUB6_REG            0x33
#define LSM6DSM_SENSORHUB7_REG            0x34
#define LSM6DSM_SENSORHUB8_REG            0x35
#define LSM6DSM_SENSORHUB9_REG            0x36
#define LSM6DSM_SENSORHUB10_REG           0x37
#define LSM6DSM_SENSORHUB11_REG           0x38
#define LSM6DSM_SENSORHUB12_REG           0x39
#define LSM6DSM_FIFO_STATUS1              0x3A
#define LSM6DSM_FIFO_STATUS2              0x3B
#define LSM6DSM_FIFO_STATUS3              0x3C
#define LSM6DSM_FIFO_STATUS4              0x3D
#define LSM6DSM_FIFO_DATA_OUT_L           0x3E
#define LSM6DSM_FIFO_DATA_OUT_H           0x3F
#define LSM6DSM_TIMESTAMP0_REG            0x40
#define LSM6DSM_TIMESTAMP1_REG            0x41
#define LSM6DSM_TIMESTAMP2_REG            0x42
#define LSM6DSM_STEP_TIMESTAMP_L          0x49
#define LSM6DSM_STEP_TIMESTAMP_H          0x4A
#define LSM6DSM_STEP_COUNTER_L            0x4B
#define LSM6DSM_STEP_COUNTER_H            0x4C
#define LSM6DSM_SENSORHUB13_REG           0x4D
#define LSM6DSM_SENSORHUB14_REG           0x4E
#define LSM6DSM_SENSORHUB15_REG           0x4F
#define LSM6DSM_SENSORHUB16_REG           0x50
#define LSM6DSM_SENSORHUB17_REG           0x51
#define LSM6DSM_SENSORHUB18_REG           0x52
#define LSM6DSM_FUNC_SRC1                 0x53
#define LSM6DSM_FUNC_SRC2                 0x54
#define LSM6DSM_WRIST_TILT_IA             0x55
#define LSM6DSM_TAP_CFG                   0x58
#define LSM6DSM_TAP_THS_6D                0x59
#define LSM6DSM_INT_DUR2                  0x5A
#define LSM6DSM_WAKE_UP_THS               0x5B
#define LSM6DSM_WAKE_UP_DUR               0x5C
#define LSM6DSM_FREE_FALL                 0x5D
#define LSM6DSM_MD1_CFG                   0x5E
#define LSM6DSM_MD2_CFG                   0x5F
#define LSM6DSM_MASTER_MODE_CODE          0x60
#define LSM6DSM_SENS_SYNC_SPI_ERROR_CODE  0x61
#define LSM6DSM_OUT_MAG_RAW_X_L           0x66
#define LSM6DSM_OUT_MAG_RAW_X_H           0x67
#define LSM6DSM_OUT_MAG_RAW_Y_L           0x68
#define LSM6DSM_OUT_MAG_RAW_Y_H           0x69
#define LSM6DSM_OUT_MAG_RAW_Z_L           0x6A
#define LSM6DSM_OUT_MAG_RAW_Z_H           0x6B
#define LSM6DSM_INT_OIS                   0x6F
#define LSM6DSM_CTRL1_OIS                 0x70
#define LSM6DSM_CTRL2_OIS                 0x71
#define LSM6DSM_CTRL3_OIS                 0x72
#define LSM6DSM_X_OFS_USR                 0x73
#define LSM6DSM_Y_OFS_USR                 0x74
#define LSM6DSM_Z_OFS_USR                 0x75

#define LSM6DSM_AFS_2G  0x00
#define LSM6DSM_AFS_4G  0x02
#define LSM6DSM_AFS_8G  0x03
#define LSM6DSM_AFS_16G 0x01

#define LSM6DSM_GFS_245DPS  0x00
#define LSM6DSM_GFS_500DPS  0x01
#define LSM6DSM_GFS_1000DPS 0x02
#define LSM6DSM_GFS_2000DPS 0x03

#define LSM6DSM_AODR_12_5Hz  0x01  // same for accel and gyro in normal mode
#define LSM6DSM_AODR_26Hz    0x02
#define LSM6DSM_AODR_52Hz    0x03
#define LSM6DSM_AODR_104Hz   0x04
#define LSM6DSM_AODR_208Hz   0x05
#define LSM6DSM_AODR_416Hz   0x06
#define LSM6DSM_AODR_833Hz   0x07
#define LSM6DSM_AODR_1660Hz  0x08
#define LSM6DSM_AODR_3330Hz  0x09
#define LSM6DSM_AODR_6660Hz  0x0A

#define LSM6DSM_GODR_12_5Hz  0x01   
#define LSM6DSM_GODR_26Hz    0x02
#define LSM6DSM_GODR_52Hz    0x03
#define LSM6DSM_GODR_104Hz   0x04
#define LSM6DSM_GODR_208Hz   0x05
#define LSM6DSM_GODR_416Hz   0x06
#define LSM6DSM_GODR_833Hz   0x07
#define LSM6DSM_GODR_1660Hz  0x08
#define LSM6DSM_GODR_3330Hz  0x09
#define LSM6DSM_GODR_6660Hz  0x0A


typedef struct {
    int32_t acc_sensitivity;
    int32_t gyro_sensitivity;
} lsm6dsm_inst_t;

#ifdef __cplusplus
extern "C" {
#endif

void lsm6dsm_wait_spi_clear(void);

void lsm6dsm_write_register_confirm(uint8_t lsm_register, uint8_t lsm_payload);

void lsm6dsm_read_register(uint8_t lsm_register);

void lsm6dsm_write_register(uint8_t lsm_register, uint8_t lsm_payload);

void lsm6dsm_comm(uint8_t read_write, uint8_t lsm_register, uint8_t length, uint8_t *payload);


/**
 * @brief initializes the gpio for int1 and starts the spi bus by calling lsm6dsm_spi_init
 * 
 */
void lsm6dsm_init();


/**
 * @brief initialize the spi bus
 * 
 */
void lsm6dsm_spi_init();

/**
 * @brief Setup the LSM6DSM control registers to hardcoded settings
 * 
 */
void lsm6dsm_start();

void lsm6dsm_readdata();

void lsm6dsm_getdata(int16_t *gyro, int16_t *acc);

void lsm6dsm_hwtest();

void lsm6dsm_task();



#ifdef __cplusplus
}
#endif

#endif
