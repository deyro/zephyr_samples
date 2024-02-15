# SPDX-License-Identifier: Apache-2.0

# From application_development/out_of_tree_driver example:

# For the sake of demonstration, we add the driver directory as a zephyr module
# by hand. If your driver is a project that's managed by west, you can remove this line.

list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules)

cmake_minimum_required(VERSION 3.13.1)

set(PROJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/)
set(DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/dts)

#set(DTC_OVERLAY_FILE "boards/stm32g473_acme_cpu_c201.overlay application/ui_ux/ui_menu.config")

set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(BOARD stm32g474re_disco)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(input_capture)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

