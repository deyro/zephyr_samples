
if(CONFIG_BSP_GPIO_DRIVER)

  zephyr_include_directories(./)
  zephyr_library()
  zephyr_library_sources(bsp_fan.c)

endif()
