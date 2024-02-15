/*
 * main.c
 *
 *  Created on: 17-Dec-2020
 *      Author: Rohan Dey
 */

#include <device.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>

#include "main.h"
#include "demoapp_devices.h"
#include "app_sd_card.h"
#include "app_keypad.h"
#include "app_tft_display.h"
#include "app_pwm_motor.h"
#include "bmp388.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(main);

void main(void)
{
	int res = 0;
	/* Initialize all devices
	 * 1. TFT Display
	 * 2. SD Card
	 * 3. Keypad
	 * 4. PWM Motor
	 * 5. BMP388
	 * */
	res = app_tft_display_init();
	if (res != 0) {
		LOG_ERR("Failed to initialize display!");
	}

	res = app_sd_init();
	if (res != 0) {
		LOG_ERR("Failed to initialize sd card!");
	}

	res = app_keypad_init();
	if (res != 0) {
		LOG_ERR("Failed to initialize keypad!");
	}

	res = app_pwm_motor_init();
	if (res != 0) {
		LOG_ERR("Failed to initialize pwm motor!");
	}

	res = bmp388_init();
	if (res != 0) {
		LOG_ERR("Failed to initialize bmp388!");
	}
#if 0
	/* Display message on TFT */
	if (res != 0) {
		app_tft_display_msg(TFT_MSG_INIT_FAILED);
	} else {
		app_tft_display_msg(TFT_MSG_INIT_SUCCESS);
	}
#endif

	if (res == 0) {
		/* Start worker threads
		 * 1. Display thread
		 * 2. BMP388 sensor thread
		 * 3. Motor control thread
		 * */
		res = app_tft_start_thread();
		res = bmp388_start_thread();
	}

	while (1) {
//		app_tft_task_handler();
		k_sleep(K_MSEC(1000));
	}
}
