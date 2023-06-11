add_executable(
	display
	debug_log.c
	oled_display.c
	main.c
	)

# pull in common dependencies and additional i2c hardware support
target_link_libraries(display pico_stdlib hardware_i2c hardware_spi)

pico_enable_stdio_usb(display 1)
pico_enable_stdio_uart(display 0)


# create map/bin/hex file etc.
pico_add_extra_outputs(display 0)

# add url via pico_set_program_url
example_auto_set_url(display 0)
