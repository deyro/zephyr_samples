/*
 * main.c
 *
 *  Created on: 17-Dec-2020
 *      Author: Rohan Dey
 */

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>
#include "main.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(app);


#if 1
static const struct device *display_dev;
/*************************** TFT Display thread ***************************/
#define APP_TFT_THREAD_STACK_SIZE	4096
#define APP_TFT_THREAD_PRIORITY		1

K_THREAD_STACK_DEFINE(app_tft_stack, APP_TFT_THREAD_STACK_SIZE);
//K_KERNEL_STACK_DEFINE(app_tft_stack, APP_TFT_THREAD_STACK_SIZE);
struct k_thread app_tft_thread_data;
k_tid_t app_tft_tid;

void app_tft_display_thread(void* p1, void* p2, void* p3) {
//	struct device *display_dev = (struct device *) p1;

	disp_obj_info pres_name, pres_val, speed_name, speed_val;
	uint32_t pval = 0U, sval = 0U;

	LOG_INF("starting app_tft_display_thread ...");

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
	lv_label_set_text(speed_name.lvgl_prop_hdl, "#00ff00 Speed: #");
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

	while (1) {
		if ((pval % 50) == 0U) {
			sprintf(pres_val.lvgl_prop_value, "%0.2f", (float)pval/50U);
//			lv_label_set_text(pres_val.lvgl_prop_hdl, pres_val.lvgl_prop_value);
		}
		if ((sval % 50) == 0U) {
			sprintf(speed_val.lvgl_prop_value, "%d", sval/50U);
			lv_label_set_text(speed_val.lvgl_prop_hdl, speed_val.lvgl_prop_value);
		}

		lv_task_handler();
		k_sleep(K_MSEC(50));

		++pval;
		++sval;
	}
}

void main() {
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}

	app_tft_tid = k_thread_create(&app_tft_thread_data, app_tft_stack, K_THREAD_STACK_SIZEOF(app_tft_stack)/*K_KERNEL_STACK_SIZEOF(app_tft_stack)*/,
			app_tft_display_thread, NULL, NULL, NULL, APP_TFT_THREAD_PRIORITY, 0, K_NO_WAIT);

	while (1) {
		k_sleep(K_MSEC(1000));
	}
}

#else

void main(void)
{
	const struct device *display_dev;
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}

	LOG_INF("starting main ...");

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
	lv_label_set_text(speed_name.lvgl_prop_hdl, "#00ff00 Speed: #");
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

	while (1) {
		if ((pval % 100) == 0U) {
			sprintf(pres_val.lvgl_prop_value, "%0.2f", (float)pval/100U);
			lv_label_set_text(pres_val.lvgl_prop_hdl, pres_val.lvgl_prop_value);
		}
		if ((sval % 50) == 0U) {
			sprintf(speed_val.lvgl_prop_value, "%d", sval/50U);
			lv_label_set_text(speed_val.lvgl_prop_hdl, speed_val.lvgl_prop_value);
		}

		lv_task_handler();
		k_sleep(K_MSEC(50));

		++pval;
		++sval;
	}
}
#endif
