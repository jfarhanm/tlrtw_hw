add_executable(spi_comm
        spi_comm_b.c
        )

# pull in common dependencies and additional pwm hardware support
target_link_libraries(spi_comm pico_stdlib hardware_pwm hardware_spi)

pico_enable_stdio_usb(spi_comm 1)
pico_enable_stdio_uart(spi_comm 0)


# create map/bin/hex file etc.
pico_add_extra_outputs(spi_comm)

# add url via pico_set_program_url
example_auto_set_url(spi_comm)
