#ifndef MODULES_MYUNITTEST_MYUNITTEST_H_
#define MODULES_MYUNITTEST_MYUNITTEST_H_

#include <stdint.h>
#include <device.h>

/* API type defines */
typedef int (*ut_check_t)(const struct device*);

struct ut_driver_api {
	ut_check_t ut_check;
};

#endif /* MODULES_MYUNITTEST_MYUNITTEST_H_ */
