#define DT_DRV_COMPAT myunittest

#include <errno.h>
#include <kernel.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>
#include "myunittest.h"
#define LOG_LEVEL CONFIG_STSPIN220_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(stspin220);

/** Configuration data */
struct myunittest_config {
	/* Step Mode 1 pin definition */
	const char *ut_gpio_port;
	gpio_pin_t ut_gpio_pin;
	gpio_flags_t ut_gpio_flags;
};

struct myunittest_data {
	struct k_mutex lock;
};

/* Init function */
static int myunittest_init(const struct device *dev) {
	const struct myunittest_config *config = dev->config;
	struct myunittest_data *data = dev->data;
	int ret = -1;

	k_mutex_init(&data->lock);
	return 0;
}

/* API Definitions */
static int myunittest_check(const struct device *dev) {
	const struct myunittest_config *config = dev->config;
	int ret = -1;

	const struct device *dir_gpio_dev = device_get_binding(config->ut_gpio_port);
	if (dir_gpio_dev == NULL) {
		LOG_ERR("Could not get direction gpio device");
		return -ENODEV;
	}
	ret = gpio_pin_configure(dir_gpio_dev, config->ut_gpio_pin, (config->ut_gpio_flags | GPIO_OUTPUT));
	if (ret != 0) {
		LOG_ERR("Failed to configure direction gpio pin %d (%d)", config->ut_gpio_pin, ret);
		return ret;
	}
	ret = gpio_pin_set(dir_gpio_dev, config->ut_gpio_pin, 1);
	if (ret != 0) {
		LOG_ERR("Error setting direction GPIO (%s)", config->ut_gpio_port);
		return ret;
	}

	return 0;
}

static const struct ut_driver_api ut_driver_api_funcs = {
	.ut_check = myunittest_check,
};

#define DEVICE_INSTANCE(inst)													\
																				\
const static struct myunittest_config myunittest_##inst##_cfg = {					\
	.ut_gpio_port = DT_INST_GPIO_LABEL(inst, ut_gpios),			\
	.ut_gpio_pin = DT_INST_GPIO_PIN(inst, ut_gpios),			\
	.ut_gpio_flags = DT_INST_GPIO_FLAGS(inst, ut_gpios),		\
};																				\
																				\
static struct myunittest_data myunittest_##inst##_drvdata;						\
																				\
DEVICE_DT_INST_DEFINE(inst,														\
		myunittest_init,															\
		device_pm_control_nop,													\
		&myunittest_##inst##_drvdata,											\
		&myunittest_##inst##_cfg,												\
		APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY,							\
		&ut_driver_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(DEVICE_INSTANCE);

