# SPDX-License-Identifier: Apache-2.0

if(CONFIG_MYUNITTEST)
  zephyr_include_directories(./)
  zephyr_library()
  zephyr_library_sources(myunittest.c)
endif()
