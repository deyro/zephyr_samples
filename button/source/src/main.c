/*
 * Copyright (c) 2020 Rohan Dey
 *
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>

#include "main.h"

#define SLEEP_TIME_MS	1

/* LED helpers, which use the led0 devicetree alias if it's available. */
static const struct device* initialize_led(void);
static void match_led_to_button(const struct device *button, int button_pin, const struct device *led);

void joystick_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
	if (pins == BIT(PIN_JOYSEL)) {
		printk("Joystick Select pressed at %" PRIu32 "\n", k_cycle_get_32());
	} else if (pins == BIT(PIN_JOYLEFT)) {
		printk("Joystick Left pressed at %" PRIu32 "\n", k_cycle_get_32());
	} else if (pins == BIT(PIN_JOYDOWN)) {
		printk("Joystick Down pressed at %" PRIu32 "\n", k_cycle_get_32());
	} else if (pins == BIT(PIN_JOYRIGHT)) {
		printk("Joystick Right pressed at %" PRIu32 "\n", k_cycle_get_32());
	} else if (pins == BIT(PIN_JOYUP)) {
		printk("Joystick Up pressed at %" PRIu32 "\n", k_cycle_get_32());
	}
}

static const struct device* initialize_button(char *label, int pin, int flags, struct gpio_callback *cb_data,
		gpio_callback_handler_t cb) {
	const struct device *button;
	int ret;

	button = device_get_binding(label);
	if (button == NULL) {
		printk("Error: didn't find %s device\n", label);
		return NULL;
	}

	ret = gpio_pin_configure(button, pin, flags);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n", ret, label, pin);
		return NULL;
	}

	ret = gpio_pin_interrupt_configure(button, pin, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n", ret, label, pin);
		return NULL;
	}

	gpio_init_callback(cb_data, cb, BIT(pin));
	gpio_add_callback(button, cb_data);
	printk("Set up button at %s pin %d\n", label, pin);

	return button;
}

void main(void) {
	struct gpio_callback joyselect_cb_data;
	const struct device *joyselect = initialize_button(LABEL_JOYSEL, PIN_JOYSEL, FLAGS_JOYSEL, &joyselect_cb_data,
			joystick_pressed);

	struct gpio_callback joyleft_cb_data;
	const struct device *joyleft = initialize_button(LABEL_JOYLEFT, PIN_JOYLEFT, FLAGS_JOYLEFT, &joyleft_cb_data,
			joystick_pressed);

	struct gpio_callback joydown_cb_data;
	const struct device *joydown = initialize_button(LABEL_JOYDOWN, PIN_JOYDOWN, FLAGS_JOYDOWN, &joydown_cb_data,
			joystick_pressed);

	struct gpio_callback joyright_cb_data;
	const struct device *joyright = initialize_button(LABEL_JOYRIGHT, PIN_JOYRIGHT, FLAGS_JOYRIGHT, &joyright_cb_data,
			joystick_pressed);

	struct gpio_callback joyup_cb_data;
	const struct device *joyup = initialize_button(LABEL_JOYUP, PIN_JOYUP, FLAGS_JOYUP, &joyup_cb_data,
			joystick_pressed);

	const struct device *led;

	printk("Press the joystick\n");

	led = initialize_led();

	while (1) {
		if (gpio_pin_get(joyselect, PIN_JOYSEL) || gpio_pin_get(joyleft, PIN_JOYLEFT) || gpio_pin_get(joydown, PIN_JOYDOWN)
				|| gpio_pin_get(joyright, PIN_JOYRIGHT) || gpio_pin_get(joyup, PIN_JOYUP)) {
			gpio_pin_set(led, PIN_BLED, true);
		} else {
			gpio_pin_set(led, PIN_BLED, false);
		}
		k_msleep(SLEEP_TIME_MS);
	}
}

static const struct device* initialize_led(void) {
	const struct device *led;
	int ret;

	led = device_get_binding(LABEL_BLED);
	if (led == NULL) {
		printk("Didn't find LED device %s\n", LABEL_BLED);
		return NULL;
	}

	ret = gpio_pin_configure(led, PIN_BLED, FLAGS_BLED);
	if (ret != 0) {
		printk("Error %d: failed to configure LED device %s pin %d\n", ret, LABEL_BLED, PIN_BLED);
		return NULL;
	}

	printk("Set up LED at %s pin %d\n", LABEL_BLED, PIN_BLED);

	return led;
}

