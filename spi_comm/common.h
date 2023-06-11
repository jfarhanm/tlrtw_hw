#ifndef SPI_COMMON_H
#define SPI_COMMON_H

#define _BUF_SIZE 64

void spi_comm_init();
uint8_t* spi_comm_read();
void spi_comm_write(uint8_t* data);
uint8_t* get_cmd_log_buf();
int decode(uint8_t* in_buffer, uint8_t* out_buffer, int B_SIZE);

#endif
