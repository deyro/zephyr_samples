/*
 * Copyright (c) 2020 Volodymyr Sirous
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ti_pca9545a

/**
 * @file Driver for [PT]CA95XX I2C Bus Multiplexer.
 */

#include <errno.h>
#include <kernel.h>
#include <device.h>
#include <init.h>
#include <sys/byteorder.h>
#include <sys/util.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <sys/printk.h>

#include "pca9545.h"
#include "pca9545_reg.h"


#define LOG_LEVEL CONFIG_I2C_MGM_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(i2c_mgm_pca95xx);

/*
static inline int pca9545_reg_read(struct i2c_mgm_pca95xx_master_data *driver,
		uint8_t reg, uint8_t * const val)
{
	return i2c_reg_read_byte(driver->i2c_master,
			DT_INST_REG_ADDR(0),
			reg, val);
}

static inline int pca9545_reg_write(struct i2c_mgm_pca95xx_data *driver,
		uint8_t reg, uint8_t val)
{
	return i2c_reg_write_byte(driver->i2c_master,
			DT_INST_REG_ADDR(0),
			reg, val);
}

static inline int pca9545_reg_update(struct i2c_mgm_pca95xx_data *driver, uint8_t reg,
		uint8_t mask, uint8_t val)
{
	return i2c_reg_update_byte(driver->i2c_master,
			DT_INST_REG_ADDR(0),
			reg, mask, val);
}

static inline int pca9545_set_hardware_config(const struct device *dev)
{
  struct i2c_mgm_pca95xx_data *data = dev->data;
	return i2c_write(data->i2c_master, NULL, sizeof(NULL),	DT_INST_REG_ADDR(0));
}

static int pca9545_init_device(const struct device *dev)
{
  struct i2c_mgm_pca95xx_data *data = dev->data;
  return i2c_write(data->i2c_master, NULL, sizeof(NULL),  DT_INST_REG_ADDR(0));
}
*/
static int i2c_mgm_pca95xx_master_init(const struct device *dev)
{
	printk("i2c_mgm_pca95xx_master_init\n");
	struct i2c_mgm_pca95xx_master_data *data = dev->data;
	const struct i2c_mgm_pca95xx_master_cfg *config = dev->config;

	data->i2c_bus = device_get_binding(config->i2c_bus_name);
	k_sem_init(&data->lock, 1, 1);

	return 0;
}

static int i2c_mgm_pca95xx_channel_init(const struct device *dev)
{
	printk("i2c_mgm_pca95xx_channel_init\n");
	struct i2c_mgm_pca95xx_ch_data *data = dev->data;
	const struct i2c_mgm_pca95xx_ch_cfg *config = dev->config;
	data->master = device_get_binding(config->master_name);
	struct i2c_mgm_pca95xx_master_data *master_data = data->master->data;
	data->p_master_lock = &master_data->lock;
	return 0;
}

static int i2c_mgm_pca95xx_ch_config(const struct device *dev, uint8_t config)
{
	printk("i2c_mgm_pca95xx_ch_config\n");
	return 0;
}

static inline int i2c_mgm_pca95xx_select_ch(const struct device *dev,\
                                            const uint8_t *buf,\
                                            uint32_t num_bytes,\
                                            uint16_t addr)
{
	printk("i2c_mgm_pca95xx_select_ch\n");
	int ret = i2c_write(dev, buf, num_bytes, addr);
	return ret;
}

static int i2c_mgm_pca95xx_ch_transfer(const struct device *dev,\
                                    struct i2c_msg *msgs,\
                                    uint8_t num_msgs,\
                                    uint16_t addr)
{
	struct i2c_mgm_pca95xx_ch_data *data = dev->data;
	const struct i2c_mgm_pca95xx_ch_cfg *config = dev->config;
	const struct i2c_mgm_pca95xx_master_cfg *bus_master_config =\
 data->master->config;
	const struct i2c_driver_api *bus_master_api = data->master->api;
	int ret = 0;
	uint8_t channel = config->channel_number;
	channel = 1U << channel;

	printk("i2c_mgm_pca95xx_ch_transfer, channel = %d\n", config->channel_number);

	/* Select the I2C bus device */
	struct i2c_mgm_pca95xx_ch_data *data1 = data->master->data;
	const struct device *master_i2c_bus = data1->master;

	k_sem_take(data->p_master_lock, K_FOREVER);
	i2c_mgm_pca95xx_select_ch(master_i2c_bus, &channel, 1, bus_master_config->i2c_addr);
//	ret = bus_master_api->transfer(master_i2c_bus, msgs, num_msgs, addr);
	ret = i2c_transfer(master_i2c_bus, msgs, num_msgs, addr);
	k_sem_give(data->p_master_lock);
//	return ret;
	return 0;
}

static struct i2c_mgm_pca95xx_master_api_t i2c_mgm_pca95xx_master_api = {
    .reset = NULL,
    .get_interrupt = NULL,
    .get_state = NULL,
};

static struct i2c_mgm_pca95xx_channel_api_t i2c_mgm_pca95xx_channel_api = {
    .config = i2c_mgm_pca95xx_ch_config,
    .transfer = i2c_mgm_pca95xx_ch_transfer,
};

#define LABEL_AND_COMMA(node_id) DT_LABEL(node_id),
#define CAT(x,y) x##_##y
#define CAT_NEST(x,y) CAT(x,y)

#define I2C_MGM_PCA95XX_CHANNEL_INIT(channel)                                   \
    static const struct i2c_mgm_pca95xx_ch_cfg                                  \
      CAT_NEST(i2c_mgm_cfg, CAT(inst, channel)) = {                             \
        .channel_number = DT_REG_ADDR(channel),                                 \
        .master_name = DT_LABEL(DT_PARENT(channel)),                            \
};                                                                              \
\
struct i2c_mgm_pca95xx_ch_data CAT_NEST(i2c_mgm_data, CAT(inst, channel));       \
\
DEVICE_AND_API_INIT(CAT_NEST(i2c_mgm, CAT(inst, channel)), DT_LABEL(channel),    \
                    i2c_mgm_pca95xx_channel_init,                               \
                    &CAT_NEST(i2c_mgm_data, CAT(inst, channel)),                \
                    &CAT_NEST(i2c_mgm_cfg, CAT(inst, channel)),                 \
                    POST_KERNEL, CONFIG_I2C_MGM_PCA95XX_INIT_PRIORITY,          \
                    &i2c_mgm_pca95xx_channel_api);

#define I2C_MGM_PCA95XX_DEVICE_INSTANCE(inst)                                   \
\
static const struct i2c_mgm_pca95xx_master_cfg i2c_mgm_##inst##_cfg = {         \
    .i2c_bus_name = DT_INST_BUS_LABEL(inst),                                    \
    .i2c_addr = DT_INST_REG_ADDR(inst),                                         \
};                                                                              \
\
static struct i2c_mgm_pca95xx_master_data i2c_mgm_##inst##_data;                \
\
DEVICE_AND_API_INIT(i2c_mgm_##inst, DT_INST_LABEL(inst),                        \
                    i2c_mgm_pca95xx_master_init,                                \
                    &i2c_mgm_##inst##_data,                                     \
                    &i2c_mgm_##inst##_cfg,                                      \
                    POST_KERNEL, CONFIG_I2C_MGM_PCA95XX_INIT_PRIORITY,          \
                    &i2c_mgm_pca95xx_master_api);                               \
\
DT_INST_FOREACH_CHILD(inst, I2C_MGM_PCA95XX_CHANNEL_INIT);

DT_INST_FOREACH_STATUS_OKAY(I2C_MGM_PCA95XX_DEVICE_INSTANCE)
