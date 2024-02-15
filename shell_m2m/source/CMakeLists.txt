# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

set(PROJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/)
set(DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/dts)
set(DTC_OVERLAY_FILE "boards/stm32g474re_disco.overlay")

set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(BOARD stm32g474re_disco)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(shell_m2m)

target_sources(app PRIVATE src/main.c)
