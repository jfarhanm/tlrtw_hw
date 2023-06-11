#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "protocol_defs.h"


#define MAX_MESSAGE_LEN 36

// TODO	: TEST THIS ON WINDOWS FOR ERRORS
//     	: DEVELOP ERROR HANDLING CODE THAT SLAVE SENDS MASTER
int decode(uint8_t* in_buffer, uint8_t* out_buffer, int B_SIZE){
	int factor = 1;
	int index=0;
	int header_index=0xFF;
	int footer_index=0x00;


	// NOTE: Creating the acknowledgment buffer
	// STEP1 : CLEARING THE BUFFER
	for (int i=0;i<B_SIZE;i++)
	{
		out_buffer[i]=0;
	}	
	out_buffer[0+factor] = BEGIN;
	out_buffer[1+factor] = 0xFF; // No magic number
	out_buffer[2+factor] = ACK;
	out_buffer[3+factor] = 0xFF;
	out_buffer[4+factor] = ';';

	
	// Find start and end of message
	int i=0;
	for(;i<B_SIZE;i++){
		if(in_buffer[i]==BEGIN && header_index==0xFF)
			header_index = i;

		if(in_buffer[i]==END){
			break;
		}
		footer_index++;
	}
	
	

	// If start is found after 16 bytes data won't be valid
	if (header_index>B_SIZE/4){
		out_buffer[2+factor] = ERR_INV_HEAD;
		return ERR_INV_HEAD;
	}
	if(footer_index-header_index>=MAX_MESSAGE_LEN){
		out_buffer[2+factor] = ERR_INV_FOOT;
		return ERR_INV_HEAD;
	}


	uint8_t magic_number = in_buffer[header_index+1];
	if(magic_number==0xFF){
		return RESP;
	}


	int data_start = header_index+2;
	int data_end = footer_index;
	int result = process_commands(in_buffer,data_start,data_end,out_buffer, factor);

	// Setting out_buffer[1] to magic number to indicate success
	if ((uint8_t)result==OK)
		out_buffer[1+factor] = magic_number;
	return result;
}




