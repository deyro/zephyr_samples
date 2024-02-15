# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

set(PROJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/)
set(DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/dts)
#set(DTC_OVERLAY_FILE "boards/stm32g474re_disco.overlay")

set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(BOARD stm32g474re_disco)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(demoapp1)

#FILE(GLOB app_sources src/*.c)
list(APPEND app_sources 
				src/main.c
				src/app_tft_display/app_tft_display.c
				src/app_sd_card/app_sd_card.c
				src/app_keypad/app_keypad.c
				src/app_pwm_motor/app_pwm_motor.c
				src/bmp388/bmp388.c
				src/bmp388/bmp3/bmp3.c
				src/bmp388/bmp3/bmp3_selftest.c
				)
target_sources(app PRIVATE ${app_sources})

target_include_directories(app PRIVATE src/include)