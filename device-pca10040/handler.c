/**
 * Copyright (c) 2019 - 2020, TITAN LAB INC
 *
 * All rights reserved.
 * 
 *
 */

#include "handler.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"
#include "app_error.h"
#include "math.h"
#include "arm_math.h"

#define ACCEL_AVERAGE_SAMPLES 255
#define ACCEL_STABLE 500
#define DECAYRATE 1000
#define DECAYRATE_N -1000

static uint16_t accel_channel_count = 0;
bool accel_cal_state = 0;
int32_t accel_data[3];
int32_t accel_data_avg[3];
int32_t gyro_data[3];
int32_t gyro_data_avg[3];
int32_t gyro_int[3];
int32_t gyro_offset[3];

int16_t accel_data_max[3];
int16_t accel_data_min[3];
int16_t accel_data_offset[3] = {0,0,0};
float accel_scale[3] = {0.0, 0.0, 0.0};
int32_t accel_scale_int[3];
int16_t accel_inst_max[3];
int16_t accel_inst_min[3];
float angle = 0.0f;

bool accel_stable_flag = false;
bool gyro_cal_flag = true;





void handler_accel_data(int16_t *raw_acc_data, int16_t *raw_gyro_data)
{
    accel_channel_count++;
    accel_data[0] += raw_acc_data[0];
    accel_data[1] += raw_acc_data[1];
    accel_data[2] += raw_acc_data[2];

    gyro_data[0] += raw_gyro_data[0];
    gyro_data[1] += raw_gyro_data[1];
    gyro_data[2] += raw_gyro_data[2];

    // if our gyro is zero'd, we'll do this later via accel stability
    if(!gyro_cal_flag)
    {
        // gyro_int[0] += (raw_gyro_data[0]-gyro_offset[0]);
        // gyro_int[1] += (raw_gyro_data[1]-gyro_offset[1]);
        // gyro_int[2] += (raw_gyro_data[2]-gyro_offset[2]);
        gyro_int[0] += (raw_gyro_data[0]);
        gyro_int[1] += (raw_gyro_data[1]);
        gyro_int[2] += (raw_gyro_data[2]);
    }

    if (accel_channel_count % ACCEL_AVERAGE_SAMPLES == 0)
    {
        accel_data_average();
        
        if(gyro_cal_flag)
        {
            // memcpy(gyro_offset, gyro_data_avg, sizeof(gyro_offset)); 
            memcpy(gyro_offset, gyro_data, sizeof(gyro_offset));  //for the 256 count version
            gyro_cal_flag = false;
            NRF_LOG_INFO("ox, %d, oy %d, oz %d",gyro_offset[0], gyro_offset[1], gyro_offset[2]);
        }
        else
        {
            // NRF_LOG_RAW_INFO("rx, %d, ry %d, rz %d,",gyro_int[0], gyro_int[1], gyro_int[2]);
            gyro_int[0] += -gyro_offset[0];
            gyro_int[1] += -gyro_offset[1];
            gyro_int[2] += -gyro_offset[2];
            
            for(uint8_t i = 0; i < 3; i++)
            {
                if(gyro_int[i]>DECAYRATE)
                {
                    gyro_int[i] -= DECAYRATE;
                } else if (gyro_int[i]<(DECAYRATE_N))
                {
                    gyro_int[i] += DECAYRATE;
                }
            }
            NRF_LOG_RAW_INFO("rx\t%d\try\t%d\trz\t%d\r\n",gyro_int[0], gyro_int[1], gyro_int[2]);
        
        }
        accel_data_avg_reset();
    }

}


void accel_data_average(void)
{
    accel_data_avg[0] = accel_data[0]/accel_channel_count;
    accel_data_avg[1] = accel_data[1]/accel_channel_count;
    accel_data_avg[2] = accel_data[2]/accel_channel_count;

    gyro_data_avg[0] = gyro_data[0]/accel_channel_count;
    gyro_data_avg[1] = gyro_data[1]/accel_channel_count;
    gyro_data_avg[2] = gyro_data[2]/accel_channel_count;
    // NRF_LOG_INFO("x, %d, y %d, z %d",accel_data_avg[0], accel_data_avg[1], accel_data_avg[2]);
    // NRF_LOG_INFO("rx, %d, ry %d, rz %d",gyro_data_avg[0], gyro_data_avg[1], gyro_data_avg[2]);
    
}

void accel_data_avg_reset(void)
{
    accel_channel_count = 0;
    memset(accel_data, 0, sizeof(accel_data_avg));
    memset(gyro_data, 0, sizeof(gyro_data_avg));

    //Reset the instant AVERAGE_SAMPLES
    memset(accel_inst_max, 0, sizeof(accel_inst_max));
    memset(accel_inst_min, 0, sizeof(accel_inst_min));
}


float calc_angle_xy(void)
{
    int16_t accel_corrected[3] = {0,0,0};
    for(uint8_t i = 0; i < 3; i++)
    {
        accel_corrected[i] = (int16_t)((float)(accel_data_avg[i]-accel_data_offset[i])*accel_scale[i]);
    }
    int32_t accel_denom_magnitude =  ( ( (int32_t)accel_corrected[1])*(int32_t)accel_corrected[1])+(((int32_t)accel_corrected[2])*(int32_t)accel_corrected[2]);
    float accel_denom_mag_float;
    arm_sqrt_f32(accel_denom_magnitude,&accel_denom_mag_float);
    // return atan2(accel_corrected[0],accel_corrected[1]);
    if (accel_corrected[1] < 0)
    {
        accel_denom_mag_float = accel_denom_mag_float*-1;
    }
    return atan2(accel_corrected[0],accel_denom_mag_float);
}

void accel_stable_process(int16_t *raw_acc_data)
{
    if (accel_channel_count > 0)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            if (raw_acc_data[i] > accel_inst_max[i])
            {
                accel_inst_max[i] = raw_acc_data[i];
            }

            if (raw_acc_data[i] < accel_inst_min[i])
            {
                accel_inst_min[i] = raw_acc_data[i];
            }
        }
    } else
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            accel_inst_max[i] = raw_acc_data[i];
            accel_inst_min[i] = raw_acc_data[i];
        }
    }
}

void accel_stable_check(void)
{
    accel_stable_flag = true;
    for(uint8_t i = 0; i < 3; i++)
    {
        if((accel_inst_max[i]-accel_inst_min[i]) > ACCEL_STABLE)
        {
            accel_stable_flag = false;
        }
    }
}