# Copyright (c) 2021 Acme CPU

# Add our modules directory to Zephyr Modules
#list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules)

cmake_minimum_required(VERSION 3.13.1)

set(PROJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/)
set(DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/dts)

#set(DTC_OVERLAY_FILE "boards/stm32g473_acme_cpu_c201.overlay application/ui_ux/ui_menu.config")

set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(BOARD stm32g473_acme_cpu_c201)

find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(fat_fs)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
