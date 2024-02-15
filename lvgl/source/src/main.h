/*
 * main.h
 *
 *  Created on: 17-Dec-2020
 *      Author: Rohan Dey
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include <lvgl.h>
#include <stdio.h>

#define MAX_PROP_TO_DISPLAY		4
#define MAX_PROP_NAME_SIZE		30
#define MAX_PROP_VALUE_SIZE		20

typedef struct {
	lv_obj_t *lvgl_prop_hdl;
//	char lvgl_prop_name[MAX_PROP_NAME_SIZE];
	char lvgl_prop_value[MAX_PROP_VALUE_SIZE];
} disp_obj_info;

#endif /* SRC_MAIN_H_ */
