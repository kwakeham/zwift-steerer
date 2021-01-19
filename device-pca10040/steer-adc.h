/**
 * Copyright (c) 2018 Keith Wakeham
 *
 * All rights reserved.
 *
 *
 */

#ifndef STEERING_H
#define STEERING_H

#include <inttypes.h>
#include <math.h>
#include <stdint.h>

#include "app_error.h"
#include "nrfx_saadc.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief Init the steer module
     *
     */
    void steering_init(void);

    /**
     * @brief Get the angle of the joystick
     *
     * @return float angle of joystick
     */
    float get_angle(void);

#ifdef __cplusplus
}
#endif

#endif  // MPOS_H
