/*
 * Copyright (c) 2021 Rohan Dey
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/i2c.h>
#include <stdio.h>
#include <drivers/sensor.h>
#include <drivers/led.h>

#define BMA280_LABEL "TILT_SENSOR"
#define PCA9632_LABEL "UI_LEDS"

void main(void) {
	printk("*** Starting PCA9545_TEST ...\n\n");

	const struct device *led_dev;

	led_dev = device_get_binding(PCA9632_LABEL);
	if (!led_dev) {
		printk("Device driver not found.\n");
		return;
	} else {
		printk("device %s found\n", PCA9632_LABEL);
	}

	int ret = led_blink(led_dev, 1, 100, 250);
	if (ret < 0) {
		printk("led_blink failed\n");
	} else {
		printk("led_blink succeeded\n");
	}

	while (1) {
		k_sleep(K_MSEC(1000));
	}
}
