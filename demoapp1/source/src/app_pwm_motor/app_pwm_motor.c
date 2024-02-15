/*
 * app_pwm_motor.c
 *
 *  Created on: 23-Dec-2020
 *      Author: Rohan Dey
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/pwm.h>
#include <sys/printk.h>
#include <inttypes.h>
#include <logging/log.h>

#include "demoapp_devices.h"
#include "app_pwm_motor.h"

LOG_MODULE_REGISTER(app_pwm_motor);

static const struct device *pwm_motor = NULL;

int app_pwm_motor_init() {
	pwm_motor = device_get_binding(LABEL_PWM_MOTOR);
	if (!pwm_motor) {
		LOG_ERR("Error: didn't find %s device", LABEL_PWM_MOTOR);
		return -1;
	} else {
		LOG_INF("Found %s device, with channel = %d, and flags = %d", LABEL_PWM_MOTOR, CHANNEL_PWM_MOTOR, FLAGS_PWM_MOTOR);
		return 0;
	}
}
