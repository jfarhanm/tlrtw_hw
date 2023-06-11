#include <stdlib.h>
#include "pico/stdlib.h"
#include "display/common.h"
#include "spi_comm/common.h"
#include "spi_comm/servos/common.h"
#include "spi_comm/motors/common.h"
#include <stdio.h>
#include <stdint.h>


void initialize(){
}

void main(){
	stdio_init_all();
	debug_logger_init();
	spi_comm_init();
	init_cam_servos();
	init_motors();
	uint8_t* working_in_buf = NULL;
	uint8_t* working_out_buf = malloc(sizeof(uint8_t)*(_BUF_SIZE));

	const uint LED_PIN = PICO_DEFAULT_LED_PIN;
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	gpio_put(LED_PIN, 0);
	debug_logger_log("INITIALIZING");
	sleep_ms(12000);
	debug_logger_log("INITIALIZED");
	char* error_data = malloc(sizeof(char)*64);


	// Main Event Loop
	while(1){
		gpio_put(LED_PIN, 0);
		working_in_buf = spi_comm_read();
		gpio_put(LED_PIN, 1);
		int resp = decode(working_in_buf, working_out_buf,_BUF_SIZE);
		spi_comm_write(working_out_buf);
		//sprintf(error_data,"ERROR IS %d",resp);
		error_data = get_cmd_log_buf();
		debug_logger_log(error_data);
		sleep_ms(10);
	}
}

