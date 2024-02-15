.. _lvgl-sample:

LittlevGL Basic Sample
######################

Overview
********

This sample application displays "Hello World" in the center of the screen
and a counter at the bottom which increments every second. If an input driver
is supported, such as the touch panel controller on mimxrt10{50,60,64}_evk
boards, "Hello World" is enclosed in a button that changes to the toggled state
when touched.

Requirements
************

- STM32G474 Discovery Board
- Waveshare 2-inch TFT Display with ST7789VW driver chip

Important
*********

- The dtsi file `dts/acme_apu_c201_spi1_bus.dtsi` is configured for the waveshare 2-inch TFT display. The parameter `mdac` can be used to change the orientation of the display.
- Important `lvgl` configuration are located in `prj.conf` file


