/*
 * app_keypad.c
 *
 *  Created on: 21-Dec-2020
 *      Author: Rohan Dey
 */

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>
#include <logging/log.h>

#include "demoapp_devices.h"
#include "app_keypad.h"

LOG_MODULE_REGISTER(app_keypad);

/* Static variables */
static const struct device *m_joyselect = NULL;
static const struct device *m_joyleft = NULL;
static const struct device *m_joydown = NULL;
static const struct device *m_joyright = NULL;
static const struct device *m_joyup = NULL;

static struct gpio_callback m_joyselect_cb;
static struct gpio_callback m_joyleft_cb;
static struct gpio_callback m_joydown_cb;
static struct gpio_callback m_joyright_cb;
static struct gpio_callback m_joyup_cb;

static void joystick_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
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

static const struct device* configure_key(char *label, int pin, int flags, struct gpio_callback *callback,
		gpio_callback_handler_t handler) {
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

	gpio_init_callback(callback, handler, BIT(pin));
	gpio_add_callback(button, callback);
	LOG_INF("Set up button at %s pin %d", label, pin);

	return button;
}

int app_keypad_init() {
	int ret = -1;

	m_joyselect = configure_key(LABEL_JOYSEL, PIN_JOYSEL, FLAGS_JOYSEL, &m_joyselect_cb, joystick_pressed);
	m_joyleft = configure_key(LABEL_JOYLEFT, PIN_JOYLEFT, FLAGS_JOYLEFT, &m_joyleft_cb, joystick_pressed);
	m_joydown = configure_key(LABEL_JOYDOWN, PIN_JOYDOWN, FLAGS_JOYDOWN, &m_joydown_cb, joystick_pressed);
	m_joyright = configure_key(LABEL_JOYRIGHT, PIN_JOYRIGHT, FLAGS_JOYRIGHT, &m_joyright_cb, joystick_pressed);
	m_joyup = configure_key(LABEL_JOYUP, PIN_JOYUP, FLAGS_JOYUP, &m_joyup_cb, joystick_pressed);

	if ((m_joyselect == NULL) || (m_joyleft == NULL) || (m_joydown == NULL) || (m_joyright == NULL)
			|| (m_joyup == NULL)) {
		ret = -1;
		LOG_ERR("Keypad init failed!");
	} else {
		ret = 0;
		LOG_INF("Keypad initialized!");
	}

	return ret;
}
