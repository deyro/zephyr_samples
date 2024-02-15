/*
 * app_tft_display.h
 *
 *  Created on: 21-Dec-2020
 *      Author: Rohan Dey
 */

#ifndef SRC_INCLUDE_APP_TFT_DISPLAY_H_
#define SRC_INCLUDE_APP_TFT_DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "app_tft_display_msgs.h"

int app_tft_display_init();
//int app_tft_setup_display();
void app_tft_task_handler();

int app_tft_start_thread();

#ifdef __cplusplus
}
#endif

#endif /* SRC_INCLUDE_APP_TFT_DISPLAY_H_ */
