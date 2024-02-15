/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/dac.h>

#if defined(CONFIG_BOARD_NUCLEO_F091RC) || \
	defined(CONFIG_BOARD_NUCLEO_G071RB) || \
	defined(CONFIG_BOARD_NUCLEO_G431RB) || \
	defined(CONFIG_BOARD_NUCLEO_L073RZ) || \
	defined(CONFIG_BOARD_NUCLEO_L152RE) || \
	defined(CONFIG_BOARD_G474RE_DISCO)
#define DAC_DEVICE_NAME		DT_LABEL(DT_NODELABEL(dac1))
#define DAC_CHANNEL_ID		1
#define DAC_RESOLUTION		12
#elif defined(CONFIG_BOARD_TWR_KE18F)
#define DAC_DEVICE_NAME		DT_LABEL(DT_NODELABEL(dac0))
#define DAC_CHANNEL_ID		0
#define DAC_RESOLUTION		12
#elif defined(CONFIG_BOARD_FRDM_K64F)
#define DAC_DEVICE_NAME		DT_LABEL(DT_NODELABEL(dac0))
#define DAC_CHANNEL_ID		0
#define DAC_RESOLUTION		12
#elif defined(CONFIG_BOARD_ARDUINO_ZERO)
#define DAC_DEVICE_NAME		DT_LABEL(DT_NODELABEL(dac0))
#define DAC_CHANNEL_ID		0
#define DAC_RESOLUTION		10
#else
#error "Unsupported board."
#endif

static const struct dac_channel_cfg dac_ch_cfg = { .channel_id = DAC_CHANNEL_ID, .resolution = DAC_RESOLUTION };

void main(void) {
	const struct device *dac_dev = device_get_binding(DAC_DEVICE_NAME);

	if (!dac_dev) {
		printk("Cannot get DAC device\n");
		return;
	}

	int ret = dac_channel_setup(dac_dev, &dac_ch_cfg);

	if (ret != 0) {
		printk("Setting up of DAC channel failed with code %d\n", ret);
		return;
	}

	printk("Generating sawtooth signal at DAC channel %d.\n", DAC_CHANNEL_ID);

#define MAX_NUM_STEPS	7
#define SLEEP_TIME		4000
	const int step_val = (4096 / MAX_NUM_STEPS);
	int dac_values = step_val;
	bool toggle = 1;
	int count = 1;

	printk("step value = %d\n", step_val);

	while (1) {
		printk("count = %d, dac_value = %d\n", count, dac_values);
		dac_write_value(dac_dev, DAC_CHANNEL_ID, dac_values);
		k_sleep(K_MSEC(SLEEP_TIME));

		if (count == MAX_NUM_STEPS) {
			toggle = !toggle;
			count = 1;
			printk("\n");
			continue;
		}

		if (toggle) {
			dac_values += step_val;
		} else {
			dac_values -= step_val;
		}

		count++;

	}
}
