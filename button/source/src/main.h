/*
 * main.h
 *
 *  Created on: 17-Dec-2020
 *      Author: Rohan Dey
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

/* BLUE LED - Label, Pin and Flags */
#define LABEL_BLED			DT_GPIO_LABEL(DT_NODELABEL(blue_led), gpios)
#define PIN_BLED			DT_GPIO_PIN(DT_NODELABEL(blue_led), gpios)
#define FLAGS_BLED			(GPIO_OUTPUT | DT_GPIO_FLAGS(DT_NODELABEL(blue_led), gpios))

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

/* JOYSTICK UP BUTTON - Label, Pin and Flags */
#define LABEL_JOYUP			DT_GPIO_LABEL(DT_NODELABEL(joyup), gpios)
#define PIN_JOYUP			DT_GPIO_PIN(DT_NODELABEL(joyup), gpios)
#define FLAGS_JOYUP			(GPIO_INPUT | DT_GPIO_FLAGS(DT_NODELABEL(joyup), gpios))

#ifdef __cplusplus
}
#endif

#endif /* SRC_MAIN_H_ */
