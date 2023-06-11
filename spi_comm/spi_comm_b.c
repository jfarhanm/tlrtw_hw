#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "common.h"

#define PI_SPI spi1
#define PI_SPI_FREQ 40

#define RPI_SPI_PIN_RX  12
#define RPI_SPI_PIN_TX  15
#define RPI_SPI_PIN_CLK  14
#define RPI_SPI_PIN_CS	13
#define RPI_RDY_PIN 22 // poor 22 looks like it has no other function 

// HARDWARE FUNCTIONS GO HERE
void spi_hw_init(){
	spi_init(PI_SPI, PI_SPI_FREQ * 1000);

	spi_set_format(
			PI_SPI,
			8,
			SPI_CPOL_1,
			SPI_CPHA_1,
			SPI_MSB_FIRST
			);
	gpio_set_function(RPI_SPI_PIN_RX , GPIO_FUNC_SPI);
	gpio_set_function(RPI_SPI_PIN_TX , GPIO_FUNC_SPI);
	gpio_set_function(RPI_SPI_PIN_CLK, GPIO_FUNC_SPI);
	gpio_set_function(RPI_SPI_PIN_CS , GPIO_FUNC_SPI);
	gpio_set_dir(RPI_SPI_PIN_TX, GPIO_OUT);
	spi_set_slave(PI_SPI,1);

	gpio_init(RPI_RDY_PIN);
	gpio_set_dir(RPI_RDY_PIN,GPIO_OUT);
	gpio_pull_up(RPI_RDY_PIN);
	gpio_put(RPI_RDY_PIN, 0);	
}
// HARDWARE FUNCTIONS END HERE 

// Every message starts with $ and ends with ; and separated by |
//#define _BUF_SIZE 64


typedef struct SpiComm{
	int BUF_SIZE;
	int RDY_PIN;
	int rdy_state;
	uint8_t* in_buf;
	uint8_t* out_buf;
}SpiComm;

static SpiComm* spi_comm;

void spi_comm_set_rdy_pin(){
	gpio_put(spi_comm->RDY_PIN, spi_comm->rdy_state);	
}


void spi_comm_init(){
	spi_hw_init();
	spi_comm = malloc(sizeof(SpiComm)) ;
	spi_comm->BUF_SIZE = _BUF_SIZE;
	spi_comm->in_buf = malloc(_BUF_SIZE);
	spi_comm->out_buf = malloc(_BUF_SIZE);
	spi_comm->RDY_PIN = RPI_RDY_PIN;
	spi_comm->rdy_state = 0;
	spi_comm_set_rdy_pin();
}

void spi_comm_clear_buffers(){
	for (int i=0;i<spi_comm->BUF_SIZE;i++){
		spi_comm->out_buf[i] = 0;
		spi_comm->in_buf[i] = 0;
	}
}

void spi_comm_reset_buffers(){
	spi_comm_clear_buffers();
	//for(int i=0;ACK[i]!='\0';i++)
	//	spi_comm->out_buf[i] = ACK[i];
}



// NOTE : User's responsibilty to copy this
// to their own array
// and to validate this data
uint8_t* spi_comm_read( ){
	spi_comm_reset_buffers();
	spi_comm->rdy_state = 1;
	spi_comm_set_rdy_pin();
        int bytes_read = spi_write_read_blocking(
			PI_SPI,
			spi_comm->out_buf,
			spi_comm->in_buf,
			spi_comm->BUF_SIZE
			);

	spi_comm->rdy_state = 0;
	spi_comm_set_rdy_pin();
	printf("\n>Read : %d bytes", bytes_read);
	return spi_comm->in_buf;
}

void spi_comm_write(uint8_t* data){
	spi_comm_clear_buffers();
	int i=0;
	for(;data[i]!=';';i++){
		spi_comm->out_buf[i] = data[i];		
	}
	spi_comm->out_buf[i]=data[i];
	spi_comm->rdy_state = 1;
	spi_comm_set_rdy_pin();
        int bytes_read = spi_write_read_blocking(
			PI_SPI,
			spi_comm->out_buf,
			spi_comm->in_buf,
			spi_comm->BUF_SIZE
			);
	spi_comm->rdy_state = 0;
	spi_comm_set_rdy_pin();
}


/*
// This is a #[test]
int main(){
	stdio_init_all();
	spi_comm_init();

	// Keep Polling in this gap and check if you get any reply
	for (int i=0;i<10;i++){
		printf(">\nDELAYING");
		sleep_ms(1000);
	}
	uint8_t* resultant_data = malloc(64);
	spi_comm->rdy_state = 1;
	spi_comm_set_rdy_pin();
	uint8_t* out = spi_comm_read();
	int a=0;
	while(1)
	{
		printf("\n[%d]>",a++);
		for(int i=0;i<64;i++)
			printf("%d|",out[i]);
		out = spi_comm_read();
		sleep_ms(100);
	}	
}

*/
