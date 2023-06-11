add_executable(tlrtw
	spi_comm/motors/motors.c
	spi_comm/servos/servos.c
	
	display/debug_log.c
	display/oled_display.c

	spi_comm/spi_comm_b.c
	spi_comm/protocol.c
	spi_comm/parse_commands.c
	main.c
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(tlrtw pico_stdlib hardware_i2c hardware_pwm hardware_spi)

pico_enable_stdio_usb(tlrtw 1)
pico_enable_stdio_uart(tlrtw 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(tlrtw)

# add url via pico_set_program_url
example_auto_set_url(tlrtw)
