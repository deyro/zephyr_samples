/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/i2c.h>
#include <stdio.h>

#include "bmp3_selftest.h"

#define I2C_DEV DT_LABEL(DT_ALIAS(i2c0))

/**
 * @file Sample app using the Fujitsu MB85RC256V FRAM through ARC I2C.
 */

#define BMP388_I2C_ADDRESS	BMP3_ADDR_I2C_PRIM //0x50

static BMP3_INTF_RET_TYPE write_bytes(uint8_t reg_addr, const uint8_t *data, uint32_t num_bytes, void *intf_ptr) {

	struct device *i2c_dev = (struct device *)intf_ptr;

	int ret = -1;

	if (num_bytes > 1) {
		/* Burst write */
		ret = i2c_burst_write(i2c_dev, BMP388_I2C_ADDRESS, reg_addr, data, num_bytes);
		if (ret < 0) {
			printk("i2c_burst_write failed, %d, 0x%x\n", ret, reg_addr);
		}
	} else {
		uint8_t wr_val = *data;
		ret = i2c_reg_write_byte(i2c_dev, BMP388_I2C_ADDRESS, reg_addr, wr_val);
		if (ret < 0) {
			printk("i2c_reg_write_byte failed, %d, 0x%x\n", ret, reg_addr);
		}
	}

	return ret;
}

static BMP3_INTF_RET_TYPE read_bytes(uint8_t reg_addr, uint8_t *data, uint32_t num_bytes, void *intf_ptr) {
	struct device *i2c_dev = (struct device *)intf_ptr;

	int ret = -1;

	if (num_bytes > 1) {
		ret = i2c_burst_read(i2c_dev, BMP388_I2C_ADDRESS, reg_addr, data, num_bytes);
		if (ret < 0) {
			printk("i2c_burst_read failed, %d, 0x%x\n", ret, reg_addr);
		}
	} else {
		/* read device ID */
		ret = i2c_reg_read_byte(i2c_dev, BMP388_I2C_ADDRESS, reg_addr, data);
		if (ret < 0) {
			printk("i2c_reg_read_byte failed, %d, 0x%x\n", ret, reg_addr);
		}
	}

	return ret;
}

void app_delay(uint32_t period, void *intf_ptr) {
	k_usleep(period);
}

void main(void) {
	const struct device *i2c_dev;

	i2c_dev = device_get_binding(I2C_DEV);
	if (!i2c_dev) {
		printk("I2C: Device driver not found.\n");
		return;
	} else {
		printk("I2C device %s found\n", I2C_DEV);
	}

	struct bmp3_dev bmp388;

	bmp388.intf_ptr = i2c_dev;
	bmp388.intf = BMP3_I2C_INTF;
	bmp388.read = read_bytes;
	bmp388.write = write_bytes;
	bmp388.delay_us = app_delay;

	int8_t ret = bmp3_selftest_check(&bmp388);
	if (ret == 0) {
		printk("bmp388 selftest succeeded\n");
	} else {
		printk("bmp388 selftest failed\n");
	}

    int8_t rslt;

    /* Variable used to select the sensor component */
    uint8_t sensor_comp;
    /* Sensor component selection */
    sensor_comp = BMP3_PRESS | BMP3_TEMP;
    /* Variable used to store the compensated data */
    struct bmp3_data data = { 0 };

	while (1) {
        /* Temperature and Pressure data are read and stored in the bmp3_data instance */
        rslt = bmp3_get_sensor_data(sensor_comp, &data, &bmp388);
        printf("Result = %d, Temp = %0.4f Celcius, Pressure = %0.4f Pascal\n", rslt, (double)data.temperature/100, (double)data.pressure/100);

		k_sleep(K_MSEC(1000));
	}
}
