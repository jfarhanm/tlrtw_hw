add_executable(motors
        motors.c
        )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(motors pico_stdlib hardware_pwm)

pico_enable_stdio_usb(motors 1)
pico_enable_stdio_uart(motors 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(motors)

# add url via pico_set_program_url
example_auto_set_url(motors)
