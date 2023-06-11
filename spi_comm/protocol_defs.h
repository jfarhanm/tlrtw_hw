#ifndef PROTOCOL_DEFS_H
#define PROTOCOL_DEFS_H
#include <stdint.h>
/*
 * ============================================
 *	A NOTE ON HOW THE PROTOCOL WORKS
 *=============================================
 *	The pico acts as the SPI Slave
 * 	SPI Master (raspberry pi zero in tlrtw's case) sends data with the
 * 	following format:
 * 	
 * 	MASTER:
 *
 * 	Master sends 64 bytes of command data:
 * 	MASTER : [BEGIN] | [1 Byte Magic Number] | [DATA(Max 32 bytes)] | [;] | 0(Till 64 bytes)
 *	
 *	Master Sends get reply command 
 *	MASTER: [BEGIN] | [0XFF]|[64 BYTES]
 *	
 *
 *
 *	SLAVE:
 *	Slave waits for master to send data and initiates the following response
 *	SLAVE : [BEGIN] | [1 Byte Magic Number Sent By Master]| [ACK|NACK] | Data(Max 32 bytes) |[;] | 0 (Till 64 Bytes)
 *	
 *	Copyright© My Insomnia  - J. Farhan M.
 * */

// PROTOCOL DEFINITIONS 
#define BEGIN 	0x24	//$
#define END	0x3B	//;
#define ACK	0x6	//ASCII ACK
#define NACK	0x7	// BELL

// ERROR DEFINITIONS
#define ERR_INV_HEAD	0x48
#define ERR_INV_FOOT	0x46
#define OK		0x4F
#define ERR_INV_CMD 	0x49
#define RESP		0x50

// NOTE: These are for Debug Delete later 

int process_commands(
		uint8_t* in_buf,
		int start_index, 
		int end_index, 
		uint8_t* out_buf,
		int factor
		);



#endif


