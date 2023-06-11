add_executable(servos
        servos.c
        )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(servos pico_stdlib hardware_pwm)

pico_enable_stdio_usb(servos 1)
pico_enable_stdio_uart(servos 0)


# create map/bin/hex file etc.
pico_add_extra_outputs(servos)

# add url via pico_set_program_url
example_auto_set_url(servos)
