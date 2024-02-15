/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "settings/settings.h"

#include <errno.h>
#include <sys/printk.h>

#define SECTION_BEGIN_LINE "\n=================================================\n"

uint8_t angle_val;
uint64_t length_val = 100;
uint16_t length_1_val = 40;
uint32_t length_2_val = 60;

int alpha_handle_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg);
int alpha_handle_commit(void);
int alpha_handle_export(int (*cb)(const char *name, const void *value, size_t val_len));

/* dynamic main tree handler */
struct settings_handler alph_handler = {
		.name = "alpha",
		.h_get = NULL,
		.h_set = alpha_handle_set,
		.h_commit = alpha_handle_commit,
		.h_export = alpha_handle_export
};

int alpha_handle_set(const char *name, size_t len, settings_read_cb read_cb,
		  void *cb_arg)
{
	const char *next;
	size_t next_len;
	int rc;

	if (settings_name_steq(name, "angle/1", &next) && !next) {
		if (len != sizeof(angle_val)) {
			return -EINVAL;
		}
		rc = read_cb(cb_arg, &angle_val, sizeof(angle_val));
		printk("<alpha/angle/1> = %d\n", angle_val);
		return 0;
	}

	next_len = settings_name_next(name, &next);

	if (!next) {
		return -ENOENT;
	}

	if (!strncmp(name, "length", next_len)) {
		next_len = settings_name_next(name, &next);

		if (!next) {
			rc = read_cb(cb_arg, &length_val, sizeof(length_val));
			printk("<alpha/length> = %" PRId64 "\n", length_val);
			return 0;
		}

		if (!strncmp(next, "1", next_len)) {
			rc = read_cb(cb_arg, &length_1_val,
				     sizeof(length_1_val));
			printk("<alpha/length/1> = %d\n", length_1_val);
			return 0;
		}

		if (!strncmp(next, "2", next_len)) {
			rc = read_cb(cb_arg, &length_2_val,
				     sizeof(length_2_val));
			printk("<alpha/length/2> = %d\n", length_2_val);
			return 0;
		}

		return -ENOENT;
	}

	return -ENOENT;
}

int alpha_handle_commit(void)
{
	printk("loading all settings under <alpha> handler is done\n");
	return 0;
}

int alpha_handle_export(int (*cb)(const char *name,
			       const void *value, size_t val_len))
{
	printk("export keys under <alpha> handler\n");
	(void)cb("alpha/pwr_chrg/powered_on", &angle_val, sizeof(angle_val));
	(void)cb("alpha/pwr_chrg/powered_off", &length_val, sizeof(length_val));
	(void)cb("alpha/pwr_chrg/chrg_dev_on", &length_1_val, sizeof(length_1_val));
	(void)cb("alpha/pwr_chrg/chrg_dev_off", &length_2_val, sizeof(length_2_val));

	return 0;
}

static void example_save_and_load_basic(void)
{
	settings_load();
	settings_save();
}

static void example_initialization(void)
{
	int rc;
	rc = settings_subsys_init();
	if (rc) {
		printk("settings subsys initialization: fail (err %d)\n", rc);
		return;
	}
	printk("settings subsys initialization: OK.\n");

	rc = settings_register(&alph_handler);
	if (rc) {
		printk("subtree <%s> handler registered: fail (err %d)\n",
		       alph_handler.name, rc);
	}
	printk("subtree <%s> handler registered: OK\n", alph_handler.name);
	printk("subtree <alpha/beta> has static handler\n");
}

void main(void)
{
	printk("\n*** Settings usage example ***\n\n");

	/* settings initialization */
	example_initialization();
	example_save_and_load_basic();
	printk("\n*** THE END  ***\n");
}
