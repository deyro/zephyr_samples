/*
 * app_sd_card.h
 *
 *  Created on: 21-Dec-2020
 *      Author: Rohan Dey
 */

#ifndef SRC_APP_SD_CARD_H_
#define SRC_APP_SD_CARD_H_

#ifdef __cplusplus
extern "C" {
#endif

int app_sd_init();
void app_sd_deinit();
int list_files();

#ifdef __cplusplus
}
#endif

#endif /* SRC_APP_SD_CARD_H_ */
