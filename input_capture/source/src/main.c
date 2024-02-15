#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <sys/__assert.h>
#include <stdio.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>

#include "bsp_fan.h"

/* JOYSTICK SELECT BUTTON - Label, Pin and Flags */
#define LABEL_JOYSEL		DT_GPIO_LABEL(DT_NODELABEL(joysel), gpios)
#define PIN_JOYSEL			DT_GPIO_PIN(DT_NODELABEL(joysel), gpios)
#define FLAGS_JOYSEL		(GPIO_INPUT | DT_GPIO_FLAGS(DT_NODELABEL(joysel), gpios))

/* JOYSTICK LEFT BUTTON - Label, Pin and Flags */
#define LABEL_JOYLEFT		DT_GPIO_LABEL(DT_NODELABEL(joyleft), gpios)
#define PIN_JOYLEFT			DT_GPIO_PIN(DT_NODELABEL(joyleft), gpios)
#define FLAGS_JOYLEFT		(GPIO_INPUT | DT_GPIO_FLAGS(DT_NODELABEL(joyleft), gpios))

/* JOYSTICK DOWN BUTTON - Label, Pin and Flags */
#define LABEL_JOYDOWN		DT_GPIO_LABEL(DT_NODELABEL(joydown), gpios)
#define PIN_JOYDOWN			DT_GPIO_PIN(DT_NODELABEL(joydown), gpios)
#define FLAGS_JOYDOWN		(GPIO_INPUT | DT_GPIO_FLAGS(DT_NODELABEL(joydown), gpios))

/* JOYSTICK RIGHT BUTTON - Label, Pin and Flags */
#define LABEL_JOYRIGHT		DT_GPIO_LABEL(DT_NODELABEL(joyright), gpios)
#define PIN_JOYRIGHT		DT_GPIO_PIN(DT_NODELABEL(joyright), gpios)
#define FLAGS_JOYRIGHT		(GPIO_INPUT | DT_GPIO_FLAGS(DT_NODELABEL(joyright), gpios))

/* Buzzer related */
#define LABEL_PWM_BUZZ				"PWM_20"
#define CHANNEL_PWM_BUZZ			1
#define FLAGS_PWM_BUZZ				(PWM_POLARITY_NORMAL)

static volatile int cc_toggle = -1;
static uint32_t pwm_period = ((1*1000*1000)/100);	// Start with 100 usec
static bool change_pwm_val = true;

static const struct device* initialize_device(char *label) {
	const struct device *dev;
	dev = device_get_binding(label);
	if (!dev) {
		printk("Device driver not found.\n");
		return NULL;
	} else {
		printk("device %s found\n", label);
	}
	return dev;
}

static int change_pwm_usec() {
	/* Initialize Buzzer PWM */
	const struct device *dev_buzz = initialize_device(LABEL_PWM_BUZZ);
	pwm_period = (pwm_period / 2);
	int ret = pwm_pin_set_usec(dev_buzz, CHANNEL_PWM_BUZZ, pwm_period, pwm_period / 2U, FLAGS_PWM_BUZZ);
	if (ret != 0) {
		printk("pwm_pin_set_usec failed, %d\n", ret);
		return -1;
	}
	return pwm_period;
}

void joystick_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins) {
	if (pins == BIT(PIN_JOYSEL)) {
		printk("Joystick Select pressed at %" PRIu32 "\n", k_cycle_get_32());
		change_pwm_val = true;
	} else if (pins == BIT(PIN_JOYLEFT)) {
		printk("Joystick Left pressed at %" PRIu32 "\n", k_cycle_get_32());
		cc_toggle = 0;
	} else if (pins == BIT(PIN_JOYDOWN)) {
		printk("Joystick Down pressed at %" PRIu32 "\n", k_cycle_get_32());
		cc_toggle = 1;
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

	if (cb != NULL) {
		gpio_init_callback(cb_data, cb, BIT(pin));
		gpio_add_callback(button, cb_data);
	}

	printk("Set up button at %s pin %d\n", label, pin);

	return button;
}

void main(void) {
	printk("*** Starting app ....\n\n");

	struct gpio_callback joyleft_cb_data;
	initialize_button(LABEL_JOYLEFT, PIN_JOYLEFT, FLAGS_JOYLEFT, &joyleft_cb_data, joystick_pressed);

	struct gpio_callback joydown_cb_data;
	initialize_button(LABEL_JOYDOWN, PIN_JOYDOWN, FLAGS_JOYDOWN, &joydown_cb_data, joystick_pressed);

	struct gpio_callback joyselect_cb_data;
	initialize_button(LABEL_JOYSEL, PIN_JOYSEL, FLAGS_JOYSEL, &joyselect_cb_data, joystick_pressed);

#if 0
	/* Initialize Buzzer PWM */
	const struct device *dev_buzz = initialize_device(LABEL_PWM_BUZZ);
	int ret = pwm_pin_set_usec(dev_buzz, CHANNEL_PWM_BUZZ, pwm_period, pwm_period / 2U, FLAGS_PWM_BUZZ);
	if (ret != 0) {
		printk("pwm_pin_set_usec failed!\n");
	}
#endif

	const struct device *bsp_fan = device_get_binding("SYS FAN");
	struct bsp_fan_api *fan_api = (struct bsp_fan_api*) bsp_fan->api;

	const struct device *tim_ic_2 = device_get_binding("TIM_2_IC");
//#define NUM_VAL	2
//	uint32_t capture_val[NUM_VAL]={0};

//	k_sleep(K_MSEC(1000));

	while (1) {
//		for (int i=0; i<NUM_VAL; i++)
//			fan_api->ic_val_get(&capture_val[i]);
//		for (int i=0; i<NUM_VAL; i++)
//			printf("%d,", capture_val[i]);

		if (cc_toggle == 0) {
			cc_toggle = -1;
			pwm_pin_disable_capture(tim_ic_2, 1);
		} else if (cc_toggle == 1) {
			cc_toggle = -1;
			pwm_pin_enable_capture(tim_ic_2, 1);
		}

		if (change_pwm_val) {
			change_pwm_val = false;
			pwm_pin_disable_capture(tim_ic_2, 1);
			change_pwm_usec();
			pwm_pin_enable_capture(tim_ic_2, 1);
		}

		uint32_t fan_freq = fan_api->fan_freq_get(bsp_fan);
		printf("%d\n", fan_freq);

		k_sleep(K_MSEC(500));
	}
}

