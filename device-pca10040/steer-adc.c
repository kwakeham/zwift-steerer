/**
 * Copyright (c) 2018 Keith Wakeham
 *
 * All rights reserved.
 *
 *
 */

#include "steer-adc.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrfx_saadc.h"
#include "nrf_saadc.h"
#include "app_timer.h"

#define SAMPLING_INTERVAL APP_TIMER_TICKS(100)

APP_TIMER_DEF(m_sampling_timer);




static nrf_saadc_value_t     m_buffer_pool[2];
int16_t sample = 0;
// static uint8_t rotation_count = 0;
// static double angle_old;

bool converting = false;
bool flag_float_angle = false;

// static nrf_saadc_value_t     m_buffer_pool_cos;
// static nrf_saadc_Valu
// static uint32_t              m_adc_evt_counter;
// nrf_saadc_value_t sample_sample;

void saadc_callback(nrfx_saadc_evt_t const * p_event)
{
    // NRF_LOG_INFO("Callback");
    // ret_code_t err_code;
    if (p_event->type == NRFX_SAADC_EVT_DONE)                                                        //Capture offset calibration complete event
    {
        converting = false;
        // ret_code_t err_code;
        // err_code = nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, 1);
        // APP_ERROR_CHECK(err_code);
        // sample = p_event->data.done.p_buffer[0];
        // NRF_LOG_INFO("data: %d",m_buffer_pool[0]);
 
    }
    else if (p_event->type == NRFX_SAADC_EVT_CALIBRATEDONE)
    {
        //Nothing, not calibrating
    }
}

void sampling_timer_callback(void * p_context)
{
    flag_float_angle = true;
    ret_code_t err_code;
    err_code = nrfx_saadc_buffer_convert(&m_buffer_pool[0], 1);
    APP_ERROR_CHECK(err_code);
    err_code = nrfx_saadc_sample();
    APP_ERROR_CHECK(err_code);
}


void steering_init(void)
{
    NRF_LOG_INFO("steer init");
    ret_code_t err_code;
    nrfx_saadc_config_t saadc_config;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_14BIT; //need to manually set the resolution or else it'll default to 8 bit
    // saadc_config.oversample = NRF_SAADC_OVERSAMPLE_256X; // default is 4 sample over sampling so need to override that.

    err_code = nrfx_saadc_init(&saadc_config, saadc_callback);
    APP_ERROR_CHECK(err_code);

    nrf_saadc_channel_config_t channel_config_steer = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);
    channel_config_steer.gain = NRF_SAADC_GAIN1_5; // this is measured against either vdd/4 or vcore = 0.6v.

    nrfx_saadc_channel_init(0, &channel_config_steer);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_buffer_convert(&m_buffer_pool[0], 1);
    APP_ERROR_CHECK(err_code);

    // err_code = nrfx_saadc_buffer_convert(&m_buffer_pool[1], 1);
    // APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_sampling_timer,APP_TIMER_MODE_REPEATED,sampling_timer_callback);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_sampling_timer, SAMPLING_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

}

void steering_convert(void)
{
    ret_code_t err_code;
    // nrf_saadc_value_t duckman;
    converting = true;
    err_code = nrfx_saadc_sample();
    // err_code = nrfx_saadc_sample_convert(0, &duckman);
    APP_ERROR_CHECK(err_code);
    // NRF_LOG_INFO("Value: %d", duckman);
    
}

// void mpos_test_convert_event_activate(void)
// {
//     ret_code_t err_code;
//     err_code = nrfx_saadc_sample();
//     APP_ERROR_CHECK(err_code);
//     if (err_code == NRFX_ERROR_INVALID_STATE)
//     {
//         NRF_LOG_ERROR("fuck sake \r\n");
//     }
    
    
// }

void steering_display_value(void)
{
    NRF_LOG_INFO("read: 1%d, ",  sample);
}


float get_angle(void)
{
    float steering_angle = 0;
    if (m_buffer_pool[0] > 8200 || m_buffer_pool[0] < 7800)
    {
        steering_angle = (((m_buffer_pool[0])-8000)*0.012);
    }
    else
    {
        steering_angle = 0;
    }

    return steering_angle;
    
}

void saadc_execute(void)
{
    if (flag_float_angle)
    {
        flag_float_angle = false;
        float angle = get_angle();
        // NRF_LOG_INFO("data: %d",);
        NRF_LOG_INFO( "Float " NRF_LOG_FLOAT_MARKER "", NRF_LOG_FLOAT(angle));
    }
}