add_executable(ex9_detection_thresholds ex9_detection_thresholds.c)

target_link_libraries(ex9_detection_thresholds pico_stdlib hardware_i2c vl53l5cx)

pico_enable_stdio_usb(ex9_detection_thresholds 1)
pico_enable_stdio_uart(ex9_detection_thresholds 0)

pico_add_extra_outputs(ex9_detection_thresholds)
