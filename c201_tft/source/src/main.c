/**
 * @file Sample app to demonstrate TFT.
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/pwm.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <string.h>

#include "main.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);

/* TFT Backlight PWM device tree configurations */
#define PWM_TFT_BL	DT_ALIAS(pwm0)
#define LABEL_PWM_TFT_BL	DT_PWMS_LABEL(PWM_TFT_BL)
#define CHANNEL_PWM_TFT_BL	DT_PWMS_CHANNEL(PWM_TFT_BL)
#define FLAGS_PWM_TFT_BL	DT_PWMS_FLAGS(PWM_TFT_BL)

static void tft_lcd_set_brightness(const struct device *pwm, float bright_percent) {	/* brightness percentage in float, 0.1, 0.25, 0.75 etc.*/

	uint32_t period_5k = (1*1000*1000)/5000;	// 5000 Hz, period in usecs

	printk("Setting pwm period to %d usec\n", period_5k);

	int ret = pwm_pin_set_usec(pwm, CHANNEL_PWM_TFT_BL, period_5k, (period_5k * bright_percent), FLAGS_PWM_TFT_BL);
	if (ret) {
		printk("Error %d: failed to set pulse width\n", ret);
		return;
	}
}
static const struct device * tft_lcd_initialize() {
	const struct device *pwm;
	pwm = device_get_binding(LABEL_PWM_TFT_BL);
	if (!pwm) {
		printk("Error: didn't find %s device\n", LABEL_PWM_TFT_BL);
		return NULL;
	}

	tft_lcd_set_brightness(pwm, 1);

	return pwm;
}

void main(void) {
	printk("C201 TFT test project ...\n");

	/**/
	const struct device *dev_tft_pwm = tft_lcd_initialize();
	if (dev_tft_pwm == NULL) {
		printk("Could not initialize pwm!\n");
		return;
	}

	const struct device *display_dev;
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}

	disp_obj_info pres_name, pres_val, speed_name, speed_val;
	uint32_t pval = 0U, sval = 0U;

	/* Set 'Pressure' label on screen */
	pres_name.lvgl_prop_hdl = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_recolor(pres_name.lvgl_prop_hdl, true);
	lv_label_set_align(pres_name.lvgl_prop_hdl, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(pres_name.lvgl_prop_hdl, "#0000ff Pressure: #");
	lv_obj_set_width(pres_name.lvgl_prop_hdl, 220);
	lv_obj_align(pres_name.lvgl_prop_hdl, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

	/* Set 'Pressure' value on screen */
	pres_val.lvgl_prop_hdl = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_recolor(pres_val.lvgl_prop_hdl, true);
	lv_label_set_align(pres_val.lvgl_prop_hdl, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(pres_val.lvgl_prop_hdl, "10.25");
	lv_obj_set_width(pres_val.lvgl_prop_hdl, 100);
	lv_obj_align(pres_val.lvgl_prop_hdl, pres_name.lvgl_prop_hdl, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

	/* Set 'Speed' name on screen */
	speed_name.lvgl_prop_hdl = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_recolor(speed_name.lvgl_prop_hdl, true);
	lv_label_set_align(speed_name.lvgl_prop_hdl, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(speed_name.lvgl_prop_hdl, "#ff0000 Speed: #");
	lv_obj_set_width(speed_name.lvgl_prop_hdl, 220);
	lv_obj_align(speed_name.lvgl_prop_hdl, pres_name.lvgl_prop_hdl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

	/* Set 'Speed' value on screen */
	speed_val.lvgl_prop_hdl = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_recolor(speed_val.lvgl_prop_hdl, true);
	lv_label_set_align(speed_val.lvgl_prop_hdl, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(speed_val.lvgl_prop_hdl, "5");
	lv_obj_set_width(speed_val.lvgl_prop_hdl, 100);
	lv_obj_align(speed_val.lvgl_prop_hdl, speed_name.lvgl_prop_hdl, LV_ALIGN_OUT_RIGHT_TOP, 0, 0);

	lv_task_handler();
	display_blanking_off(display_dev);

	float brightness_level = 0.1;

	while (1) {
		if ((pval % 50) == 0U) {
			sprintf(pres_val.lvgl_prop_value, "%d", pval/50U);
			lv_label_set_text(pres_val.lvgl_prop_hdl, pres_val.lvgl_prop_value);
		}
		if ((sval % 10) == 0U) {
			sprintf(speed_val.lvgl_prop_value, "%d", sval/10U);
			lv_label_set_text(speed_val.lvgl_prop_hdl, speed_val.lvgl_prop_value);
		}
		if ((pval % 100) == 0U) {
			if (brightness_level > 1.0) brightness_level = 0.1;
			brightness_level = brightness_level + 0.1;

//			tft_lcd_set_brightness(dev_tft_pwm, brightness_level);
		}

		lv_task_handler();
		k_sleep(K_MSEC(25));

		++pval;
		++sval;
	}
}
