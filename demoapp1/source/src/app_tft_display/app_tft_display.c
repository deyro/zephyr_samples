/*
 * app_tft_display.c
 *
 *  Created on: 21-Dec-2020
 *      Author: Rohan Dey
 */

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <zephyr.h>
#include <kernel.h>
#include <logging/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_tft_display.h"
#include "demoapp_devices.h"
#include "bmp388.h"

LOG_MODULE_REGISTER(app_tft_display);

typedef struct {
	lv_obj_t *o_splash_bg;	// Splash area background
	lv_obj_t *l_splash;		// Splash text label

	lv_obj_t *o_info_bg;	// Info area background
	lv_obj_t *l_info;		// Info text label

	lv_obj_t *o_pname_bg;	// Pname area background
	lv_obj_t *l_pname;		// Pname text label

	lv_obj_t *o_pval_bg;	// Pval area background
	lv_obj_t *l_pval;		// Pval text label

	lv_obj_t *o_punit_bg;	// Punit area background
	lv_obj_t *l_punit;		// Punit text label

	lv_obj_t *o_tname_bg;	// Tname area background
	lv_obj_t *l_tname;		// Tname text label

	lv_obj_t *o_tval_bg;	// Tval area background
	lv_obj_t *l_tval;		// Tval text label

	lv_obj_t *o_tunit_bg;	// Tunit area background
	lv_obj_t *l_tunit;		// Tunit text label
} screenobj;

typedef struct {
	lv_style_t style_info;
	lv_style_t style_normtext;
	lv_style_t style_bigtext;
	lv_style_t style_pval;
	lv_style_t style_tval;
} screenstyles;

static screenobj m_screen;
static screenstyles m_style;

static const struct device *display_dev;

static int app_tft_init_styles() {
	/* Create the info area style */
	lv_style_init(&m_style.style_info);
	lv_style_set_bg_opa(&m_style.style_info, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&m_style.style_info, LV_STATE_DEFAULT, LV_COLOR_SILVER);
	lv_style_set_text_color(&m_style.style_info, LV_STATE_DEFAULT, LV_COLOR_BLUE);
	lv_style_set_text_font(&m_style.style_info, LV_STATE_DEFAULT, &lv_font_montserrat_12);

	/* Create Normal text label styles */
	lv_style_init(&m_style.style_normtext);
//	lv_style_set_bg_opa(&m_style.style_normtext, LV_STATE_DEFAULT, LV_OPA_COVER);
//	lv_style_set_bg_color(&m_style.style_normtext, LV_STATE_DEFAULT, LV_COLOR_WHITE);
//	lv_style_set_text_color(&m_style.style_normtext, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_text_font(&m_style.style_normtext, LV_STATE_DEFAULT, &lv_font_montserrat_24);

	/* Create Big text label styles */
	lv_style_init(&m_style.style_bigtext);
	lv_style_set_bg_opa(&m_style.style_bigtext, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&m_style.style_bigtext, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
	lv_style_set_text_color(&m_style.style_bigtext, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_text_font(&m_style.style_bigtext, LV_STATE_DEFAULT, &lv_font_montserrat_36);
	lv_style_set_border_color(&m_style.style_bigtext, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_border_opa(&m_style.style_bigtext, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_border_width(&m_style.style_bigtext, LV_STATE_DEFAULT, 1);

	/* Create Pval text label styles */
	lv_style_init(&m_style.style_pval);
	lv_style_set_bg_opa(&m_style.style_pval, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&m_style.style_pval, LV_STATE_DEFAULT, LV_COLOR_GREEN);
	lv_style_set_text_color(&m_style.style_pval, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_text_font(&m_style.style_pval, LV_STATE_DEFAULT, &lv_font_montserrat_36);

	/* Create Tval text label styles */
	lv_style_init(&m_style.style_tval);
	lv_style_set_bg_opa(&m_style.style_tval, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&m_style.style_tval, LV_STATE_DEFAULT, LV_COLOR_NAVY);
	lv_style_set_text_color(&m_style.style_tval, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_text_font(&m_style.style_tval, LV_STATE_DEFAULT, &lv_font_montserrat_36);

	return 0;
}

static int app_tft_init_splash_screen() {

	/* Init Splash area */
	m_screen.o_splash_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_width(m_screen.o_splash_bg, 240);
	lv_obj_set_height(m_screen.o_splash_bg, 80);
	lv_obj_add_style(m_screen.o_splash_bg, LV_OBJ_PART_MAIN, &m_style.style_bigtext);
	lv_obj_align(m_screen.o_splash_bg, NULL, LV_ALIGN_CENTER, 0, 0);

	m_screen.l_splash = lv_label_create(m_screen.o_splash_bg, NULL);
//	lv_label_set_recolor(m_screen.l_splash, true);
	lv_label_set_align(m_screen.l_splash, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(m_screen.l_splash, "Demo App");
//	lv_obj_add_style(m_screen.l_splash, LV_OBJ_PART_MAIN, &m_style.style_bigtext);
	lv_obj_set_style_local_text_font(m_screen.l_splash, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_36);
	lv_obj_align(m_screen.l_splash, m_screen.o_splash_bg, LV_ALIGN_CENTER, 0, 0);

	/* Initialize Info area */
	m_screen.o_info_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_width(m_screen.o_info_bg, 320);
	lv_obj_set_height(m_screen.o_info_bg, 20);
	lv_obj_add_style(m_screen.o_info_bg, LV_OBJ_PART_MAIN, &m_style.style_info);
	lv_obj_align(m_screen.o_info_bg, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

	m_screen.l_info = lv_label_create(m_screen.o_info_bg, NULL);
	lv_label_set_recolor(m_screen.l_info, true);
	lv_label_set_align(m_screen.l_info, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(m_screen.l_info, "Initializing ...");
	lv_obj_add_style(m_screen.l_info, LV_OBJ_PART_MAIN, &m_style.style_info);
	lv_obj_align(m_screen.l_info, m_screen.o_info_bg, LV_ALIGN_IN_LEFT_MID, 5, 0);


	return 0;
}

static int app_tft_setup_pressure_area() {

	/* Init Pressure Name area */
	m_screen.o_pname_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_width(m_screen.o_pname_bg, 320);
	lv_obj_set_height(m_screen.o_pname_bg, 30);
	lv_obj_add_style(m_screen.o_pname_bg, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_align(m_screen.o_pname_bg, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

	m_screen.l_pname = lv_label_create(m_screen.o_pname_bg, NULL);
	lv_label_set_align(m_screen.l_pname, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(m_screen.l_pname, "Pressure");
//	lv_obj_add_style(m_screen.l_pname, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_set_style_local_text_font(m_screen.l_pname, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_align(m_screen.l_pname, m_screen.o_pname_bg, LV_ALIGN_IN_LEFT_MID, 5, 0);

	/* Init Pressure Value area */
	m_screen.o_pval_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_width(m_screen.o_pval_bg, 240);
	lv_obj_set_height(m_screen.o_pval_bg, 50);
	lv_obj_add_style(m_screen.o_pval_bg, LV_OBJ_PART_MAIN, &m_style.style_pval);
	lv_obj_align(m_screen.o_pval_bg, m_screen.o_pname_bg, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

	m_screen.l_pval = lv_label_create(m_screen.o_pval_bg, NULL);
	lv_obj_set_width(m_screen.l_pval, 240);
	lv_obj_set_height(m_screen.l_pval, 50);
	lv_label_set_align(m_screen.l_pval, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(m_screen.l_pval, "0.00");
//	lv_obj_add_style(m_screen.l_pname, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_set_style_local_text_font(m_screen.l_pval, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_36);
	lv_obj_align(m_screen.l_pval, m_screen.o_pval_bg, LV_ALIGN_IN_LEFT_MID, 5, 0);

	/* Init Pressure Unit area */
	m_screen.o_punit_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_width(m_screen.o_punit_bg, 40);
	lv_obj_set_height(m_screen.o_punit_bg, 30);
	lv_obj_add_style(m_screen.o_punit_bg, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_align(m_screen.o_punit_bg, m_screen.o_pval_bg, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);

	m_screen.l_punit = lv_label_create(m_screen.o_punit_bg, NULL);
	lv_label_set_align(m_screen.l_punit, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(m_screen.l_punit, "Pa");
//	lv_obj_add_style(m_screen.l_pname, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_set_style_local_text_font(m_screen.l_punit, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_align(m_screen.l_punit, m_screen.o_punit_bg, LV_ALIGN_IN_LEFT_MID, 5, 0);

	return 0;
}

static int app_tft_setup_temp_area() {

	/* Init Temperature Name area */
	m_screen.o_tname_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_width(m_screen.o_tname_bg, 320);
	lv_obj_set_height(m_screen.o_tname_bg, 30);
	lv_obj_add_style(m_screen.o_tname_bg, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_align(m_screen.o_tname_bg, m_screen.o_pname_bg, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 80);
//	lv_obj_align(m_screen.o_tname_bg, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 110);

	m_screen.l_tname = lv_label_create(m_screen.o_tname_bg, NULL);
	lv_label_set_align(m_screen.l_tname, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(m_screen.l_tname, "Temperature");
//	lv_obj_add_style(m_screen.l_tname, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_set_style_local_text_font(m_screen.l_tname, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_align(m_screen.l_tname, m_screen.o_tname_bg, LV_ALIGN_IN_LEFT_MID, 5, 0);

	/* Init Temperature Value area */
	m_screen.o_tval_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_width(m_screen.o_tval_bg, 240);
	lv_obj_set_height(m_screen.o_tval_bg, 50);
	lv_obj_add_style(m_screen.o_tval_bg, LV_OBJ_PART_MAIN, &m_style.style_tval);
	lv_obj_align(m_screen.o_tval_bg, m_screen.o_tname_bg, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

	m_screen.l_tval = lv_label_create(m_screen.o_tval_bg, NULL);
	lv_label_set_align(m_screen.l_tval, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(m_screen.l_tval, "0.00");
//	lv_obj_add_style(m_screen.l_tname, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_set_style_local_text_font(m_screen.l_tval, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_36);
	lv_obj_align(m_screen.l_tval, m_screen.o_tval_bg, LV_ALIGN_IN_LEFT_MID, 5, 0);

	/* Init Temperature Unit area */
	m_screen.o_tunit_bg = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_width(m_screen.o_tunit_bg, 40);
	lv_obj_set_height(m_screen.o_tunit_bg, 30);
	lv_obj_add_style(m_screen.o_tunit_bg, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_align(m_screen.o_tunit_bg, m_screen.o_tval_bg, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, 0);

	m_screen.l_tunit = lv_label_create(m_screen.o_tunit_bg, NULL);
	lv_label_set_align(m_screen.l_tunit, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(m_screen.l_tunit, "C");
//	lv_obj_add_style(m_screen.l_tname, LV_OBJ_PART_MAIN, &m_style.style_normtext);
	lv_obj_set_style_local_text_font(m_screen.l_tunit, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
	lv_obj_align(m_screen.l_tunit, m_screen.o_tunit_bg, LV_ALIGN_IN_LEFT_MID, 5, 0);

	return 0;
}

static int app_tft_setup_pt_screen() {
	app_tft_setup_pressure_area();
	app_tft_setup_temp_area();
	return 0;
}

int app_tft_display_init() {
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return -1;
	}
#if 0
	/* Set 'Initializing' label on screen */
	m_init_label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_recolor(m_init_label, true);
	lv_label_set_align(m_init_label, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(m_init_label, "#0000ff Initializing ... #");
	lv_obj_set_width(m_init_label, 320);
	lv_obj_align(m_init_label, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

	lv_task_handler();
#else
	app_tft_init_styles();
	app_tft_init_splash_screen();

	lv_task_handler();
#endif

	display_blanking_off(display_dev);
	LOG_INF("TFT display init done");

	return 0;
}

static int app_tft_setup_display() {
	app_tft_setup_pt_screen();
	return 0;
}

void app_tft_task_handler() {
	lv_task_handler();
}

/*************************** TFT Display thread ***************************/

#define APP_TFT_THREAD_STACK_SIZE	4096
#define APP_TFT_THREAD_PRIORITY		5
K_THREAD_STACK_DEFINE(app_tft_stack, APP_TFT_THREAD_STACK_SIZE);
struct k_thread app_tft_thread_data;
k_tid_t app_tft_tid;

void app_tft_display_thread(void *p1, void *p2, void *p3) {
//	struct device *tft_dev = (struct device *) p1;

	LOG_INF("app_tft_display_thread started");

#if 0
	/* Clear the 'Init' label */
	lv_label_set_text(m_init_label, LV_SYMBOL_OK);
#endif

//	lv_obj_set_state(m_screen.o_splash_bg, LV_STATE_DISABLED);
//	lv_obj_set_state(m_screen.l_splash, LV_STATE_DISABLED);

	lv_obj_del(m_screen.l_splash);
	lv_obj_del(m_screen.o_splash_bg);

	app_tft_setup_display();

//	lv_task_handler();
//	display_blanking_off(display_dev);

	char pbuf[20] = { 0x00 };
	char tbuf[20] = { 0x00 };
//	int pval = 0;

	/* BMP388 Data */
	bmp388_data_t *p_bmp388_data = NULL;

	while (1) {

		p_bmp388_data = k_fifo_get(&m_bmp388_fifo, K_FOREVER);
		if (p_bmp388_data == NULL) {
			continue;
		}

		/* Print the temperature and pressure */
		printf("T = %0.2fC, P = %0.2fPa\n", (double) p_bmp388_data->temperature / 100,
				(double) p_bmp388_data->pressure / 100);
		sprintf(pbuf, "%0.2f", (float) p_bmp388_data->pressure / 100);
		sprintf(tbuf, "%0.2f", (float) p_bmp388_data->temperature / 100);

		lv_label_set_text(m_screen.l_pval, pbuf);
		lv_label_set_text(m_screen.l_tval, tbuf);

		/* Dealloc the memory */
		free(p_bmp388_data);
	}
}

#define APP_TFT_REFRESH_THREAD_STACK_SIZE	2048
#define APP_TFT_REFRESH_THREAD_PRIORITY		10
K_THREAD_STACK_DEFINE(app_tft_refresh_stack, APP_TFT_REFRESH_THREAD_STACK_SIZE);
struct k_thread app_tft_refresh_thread_data;
k_tid_t app_tft_refresh_tid;

void app_tft_refresh_thread(void *p1, void *p2, void *p3) {
	LOG_INF("app_tft_refresh_thread started");
	while (1) {
		lv_task_handler();
		k_sleep(K_MSEC(10));
	}
}

int app_tft_start_thread() {
	app_tft_tid = k_thread_create(&app_tft_thread_data, app_tft_stack, K_THREAD_STACK_SIZEOF(app_tft_stack),
			app_tft_display_thread, NULL, NULL, NULL, APP_TFT_THREAD_PRIORITY, 0, K_NO_WAIT);

	app_tft_refresh_tid = k_thread_create(&app_tft_refresh_thread_data, app_tft_refresh_stack,
			K_THREAD_STACK_SIZEOF(app_tft_refresh_stack), app_tft_refresh_thread, NULL, NULL, NULL,
			APP_TFT_REFRESH_THREAD_PRIORITY, 0, K_NO_WAIT);

	return 0;
}
