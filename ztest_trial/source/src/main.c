#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void main(void) {
	printk("Starting app\n");


	while (1) {
		k_sleep(K_MSEC(1500));
	}
}
