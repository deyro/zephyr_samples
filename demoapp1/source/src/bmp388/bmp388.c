/*
 * bmp388.c
 *
 *  Created on: 28-Dec-2020
 *      Author: Rohan Dey
 */

#include <errno.h>
#include <zephyr.h>
#include <device.h>
#include <drivers/i2c.h>
#include <logging/log.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmp388.h"
#include "bmp3_selftest.h"
#include "demoapp_devices.h"

LOG_MODULE_REGISTER(bmp388);

#define BMP388_I2C_ADDRESS	BMP3_ADDR_I2C_PRIM

static BMP3_INTF_RET_TYPE bmp388_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t num_bytes, void *intf_ptr) {

	struct device *i2c_dev = (struct device*) intf_ptr;

	int ret = -1;

	if (num_bytes > 1) {
		/* Burst write */
		ret = i2c_burst_write(i2c_dev, BMP388_I2C_ADDRESS, reg_addr, data, num_bytes);
		if (ret < 0) {
			LOG_ERR("i2c_burst_write failed, %d, 0x%x\n", ret, reg_addr);
		}
	} else {
		uint8_t wr_val = *data;
		ret = i2c_reg_write_byte(i2c_dev, BMP388_I2C_ADDRESS, reg_addr, wr_val);
		if (ret < 0) {
			LOG_ERR("i2c_reg_write_byte failed, %d, 0x%x\n", ret, reg_addr);
		}
	}

	return ret;
}

static BMP3_INTF_RET_TYPE bmp388_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t num_bytes, void *intf_ptr) {
	struct device *i2c_dev = (struct device*) intf_ptr;

	int ret = -1;

	if (num_bytes > 1) {
		ret = i2c_burst_read(i2c_dev, BMP388_I2C_ADDRESS, reg_addr, data, num_bytes);
		if (ret < 0) {
			LOG_ERR("i2c_burst_read failed, %d, 0x%x\n", ret, reg_addr);
		}
	} else {
		/* read device ID */
		ret = i2c_reg_read_byte(i2c_dev, BMP388_I2C_ADDRESS, reg_addr, data);
		if (ret < 0) {
			LOG_ERR("i2c_reg_read_byte failed, %d, 0x%x\n", ret, reg_addr);
		}
	}

	return ret;
}

static void bmp388_usdelay(uint32_t period, void *intf_ptr) {
	k_usleep(period);
}

static struct bmp3_dev m_bmp388;
struct k_fifo m_bmp388_fifo;

#define BMP388_THREAD_STACK_SIZE	1024
#define BMP388_THREAD_PRIORITY		1
K_THREAD_STACK_DEFINE(bmp388_stack, BMP388_THREAD_STACK_SIZE);
struct k_thread bmp388_thread_data;
k_tid_t bmp388_tid;

#define BMP388_DATA_AVG_FREQ	20

static void bmp388_thread(void *p1, void *p2, void *p3) {
	LOG_INF("bmp388_thread started");

	int8_t rslt, count = 0;

	/* Variable used to select the sensor component */
	uint8_t sensor_comp;
	/* Sensor component selection */
	sensor_comp = BMP3_PRESS | BMP3_TEMP;
	/* Variable used to store the compensated data */
	struct bmp3_data data = { 0 };

	/* Pointer where memory will be allocated for fifo */
	bmp388_data_t *p_bmp388_data = NULL;

	/* temporary variable for averaging */
    int64_t temp=0, pres=0;

	while (1) {
		/* Temperature and Pressure data are read and stored in the bmp3_data instance */
		rslt = bmp3_get_sensor_data(sensor_comp, &data, &m_bmp388);
		if (rslt != 0) {
			LOG_ERR("bmp3 read error!");
			k_sleep(K_MSEC(50));
			continue;
		}

		/* Add the values for averaging later on */
		temp += data.temperature;
		pres += data.pressure;

		if (count >= BMP388_DATA_AVG_FREQ) {
			/* Average the data */
			temp /= BMP388_DATA_AVG_FREQ;
			pres /= BMP388_DATA_AVG_FREQ;

			/* Print the temperature and pressure */
			/*printf("T = %0.2fC, P = %0.2fPa\n", (float) temp / 100, (float) pres / 100);*/

			/* Allocate memory and set values */
			p_bmp388_data = (bmp388_data_t*) calloc(1, sizeof(bmp388_data_t));
			if (p_bmp388_data == NULL) {
				LOG_ERR("calloc failed!");
				k_sleep(K_MSEC(50));
				continue;
			}
			p_bmp388_data->temperature = temp;
			p_bmp388_data->pressure = pres;

			/* Put data into the fifo */
			k_fifo_put(&m_bmp388_fifo, p_bmp388_data);

			/* Reset the values to 0*/
			temp = 0;
			pres = 0;

			/* Reset the counter */
			count = 0;
		}
		++count;

		k_sleep(K_MSEC(50));
	}
}

int bmp388_start_thread() {
	bmp388_tid = k_thread_create(&bmp388_thread_data, bmp388_stack, K_THREAD_STACK_SIZEOF(bmp388_stack), bmp388_thread,
	NULL, NULL, NULL, BMP388_THREAD_PRIORITY, 0, K_NO_WAIT);

	return 0;
}

int bmp388_init() {
	const struct device *i2c0_dev;
	i2c0_dev = device_get_binding(I2C0_DEV);
	if (!i2c0_dev) {
		LOG_ERR("I2C: Device driver not found");
		return -1;
	} else {
		LOG_INF("I2C device %s found", I2C0_DEV);
	}

	m_bmp388.intf_ptr = i2c0_dev;
	m_bmp388.intf = BMP3_I2C_INTF;
	m_bmp388.read = bmp388_i2c_read;
	m_bmp388.write = bmp388_i2c_write;
	m_bmp388.delay_us = bmp388_usdelay;

	int8_t ret = bmp3_selftest_check(&m_bmp388);
	if (ret == 0) {
		LOG_INF("bmp388 selftest succeeded");

		/* Initialize the fifo */
		k_fifo_init(&m_bmp388_fifo);

	} else {
		LOG_ERR("bmp388 selftest failed");
	}

	return ret;
}
