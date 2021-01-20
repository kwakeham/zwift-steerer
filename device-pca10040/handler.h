/**
 * Copyright (c) 2019 - 2020, TITAN LAB INC
 *
 * All rights reserved.
 * 
 *
 */

#include "nrf_log.h"
#include "nrf_log_ctrl.h"



/**
 * @brief Averages the data down
 * 
 * @param raw_data 
 */

void handler_accel_data(int16_t *raw_accel_data, int16_t *raw_gyro_data);

/**
 * @brief 
 * 
 */
void accel_data_average(void);

/**
 * @brief Calculates the data average and resets the values
 * 
 */
void accel_data_avg_reset(void);

/**
 * @brief Calculates the angle in the xy by calculating the corrected accel data for x and y
 * 
 * @return float angle in the xy
 */
float calc_angle_xy(void);

/**
 * @brief 
 * 
 * @param raw_acc_data 
 */
void accel_stable_process(int16_t *raw_acc_data);

/**
 * @brief 
 * 
 */
void accel_stable_check(void);