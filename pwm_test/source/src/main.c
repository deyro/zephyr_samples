/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/pwm.h>

#define PWM_BUZZER_NODE	DT_ALIAS(pwm0)

#if DT_NODE_HAS_STATUS(PWM_BUZZER_NODE, okay)
#define PWM_LABEL	DT_PWMS_LABEL(PWM_BUZZER_NODE)
#define PWM_CHANNEL	DT_PWMS_CHANNEL(PWM_BUZZER_NODE)
#define PWM_FLAGS	DT_PWMS_FLAGS(PWM_BUZZER_NODE)
#else
#error "Unsupported board: pwm0 devicetree alias is not defined"
#define PWM_LABEL	""
#define PWM_CHANNEL	0
#define PWM_FLAGS	0
#endif

#define SINGLE_TONE	1
//#define TUNE_1	1

void main(void) {
	const struct device *pwm;
	uint32_t period = 0;
	uint32_t offset = 1000;
	int ret;

	printk("PWM-based buzzer\n");

	pwm = device_get_binding(PWM_LABEL);
	if (!pwm) {
		printk("Error: didn't find %s device\n", PWM_LABEL);
		return;
	}

#if SINGLE_TONE
	uint32_t period_4k = (1*1000*1000)/4000;	// 4000 Hz, period in usecs
	bool toggle = 1;
#endif

	while (1) {
#if SINGLE_TONE
		if (toggle)
			period = period_4k;
		else
			period = 0;

		printk("Setting pwm period to %d usec\n", period);

		ret = pwm_pin_set_usec(pwm, PWM_CHANNEL, period, period / 2U, PWM_FLAGS);
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return;
		}

		toggle = !toggle;
		k_sleep(K_MSEC(500));

#elif TUNE_1
		for (int i=0; i<8; i++) {
			printk("Setting pwm period to %d usec\n", period);
			ret = pwm_pin_set_usec(pwm, PWM_CHANNEL, period, period / 2U, PWM_FLAGS);
			if (ret) {
				printk("Error %d: failed to set pulse width\n", ret);
				return;
			}
			k_sleep(K_MSEC(100));

			period = (1*1000*1000)/offset;
			offset += 1000;
		}

		period = 0;			// reset period
		offset = 1000;		// reset offset
#else

#endif
	}
}
