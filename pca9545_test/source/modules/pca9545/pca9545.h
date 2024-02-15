/*
 * Copyright (c) 2020 Volodymyr Sirous
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef MODULES_PCA9545A_PCA9545_H_
#define MODULES_PCA9545A_PCA9545_H_

struct i2c_mgm_pca95xx_master_cfg {
  /** The master I2C bus name */
  const char * const i2c_bus_name;
  /** The slave address of the chip */
  uint16_t i2c_addr;
};

struct i2c_mgm_pca95xx_master_data {
  /** Master I2C device */
  const struct device *i2c_bus;
  const struct device *reset;
  uint8_t ch_state;
  uint8_t interrupt;
  struct k_sem lock;
};

struct i2c_mgm_pca95xx_ch_cfg {
  const uint8_t channel_number;
  const char *master_name;
};

struct i2c_mgm_pca95xx_ch_data {
  const struct device *master;
  struct k_sem *p_master_lock;
};

typedef int (*i2c_mgm_pca95xx_reset_t)(const struct device *);
typedef int (*i2c_mgm_pca95xx_get_int_t)(const struct device *);
typedef int (*i2c_mgm_pca95xx_get_state_t)(const struct device *);

struct i2c_mgm_pca95xx_master_api_t {
  i2c_mgm_pca95xx_reset_t reset;
  i2c_mgm_pca95xx_get_int_t get_interrupt;
  i2c_mgm_pca95xx_get_state_t get_state;
};

typedef int (*i2c_mgm_pca95xx_ch_config_t)(const struct device *, uint8_t);
typedef int (*i2c_mgm_pca95xx_ch_transfer_t)(const struct device *, struct i2c_msg *msgs,\
                                             uint8_t num_msgs, uint16_t addr);

struct i2c_mgm_pca95xx_channel_api_t {
  i2c_mgm_pca95xx_ch_config_t config;
  i2c_mgm_pca95xx_ch_transfer_t transfer;
};

#endif /* MODULES_PCA9545A_PCA9545_H_ */
