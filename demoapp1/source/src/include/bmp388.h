/*
 * bmp388.h
 *
 *  Created on: 28-Dec-2020
 *      Author: Rohan Dey
 */

#ifndef SRC_INCLUDE_BMP388_H_
#define SRC_INCLUDE_BMP388_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr.h>
#include <stdint.h>

typedef struct bmp388_data {
	/*! 1st word reserved for use by FIFO */
	void* fifo_reserved;

	/*! Compensated temperature */
    int64_t temperature;

    /*! Compensated pressure */
    uint64_t pressure;
} bmp388_data_t;

extern struct k_fifo m_bmp388_fifo;

int bmp388_init();
int bmp388_start_thread();

#ifdef __cplusplus
}
#endif

#endif /* SRC_INCLUDE_BMP388_H_ */
