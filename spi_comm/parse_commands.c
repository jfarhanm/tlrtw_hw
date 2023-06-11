#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "protocol_defs.h"
#include "servos/common.h"
#include "motors/common.h"
// This file is very trtlw specific
// ====================================
// 		COMMANDS
// ====================================
//	D \ Data to print to debug log
//	
//	[Motor]:
//	M \ L \ SPEED
//	  \ R \ SPEED
//
//	[Servo]:
//	S \ R \ POSITION
//	  \ Y \ POSITION 
//
//
//	Copyright© My Insomnia - J. Farhan M.


#define DATA_LEN 32
// Display specific commands
char* cmd_debug_log_buf = NULL;
char* cmd_motor_debug_buf = NULL;
char* cmd_servo_debug_buf = NULL;

void init_cmd_debug_log_buf(){
	if(cmd_debug_log_buf==NULL){
		cmd_debug_log_buf = malloc(sizeof(char)*DATA_LEN);
		cmd_motor_debug_buf = malloc(sizeof(char)*DATA_LEN);
		cmd_servo_debug_buf = malloc(sizeof(char)*DATA_LEN);
	}

	for(int i=0 ;i<DATA_LEN;i++)
		cmd_debug_log_buf[i] = 0;
}

void cmd_debug_log(char* dt,int start, int end){
	for(int i=0 ;i<DATA_LEN;i++)
		cmd_debug_log_buf[i] = 0;

	for(int i=start;i<end;i++){
		cmd_debug_log_buf[i-start] = dt[i];	
	}
}

char* get_cmd_log_buf(){
	return cmd_debug_log_buf;
}

// Motor specific commands:  0 is L |1 is R
// 16 bits in the future 
void set_motor(uint8_t* dt,int start,int end){
	uint8_t index = dt[start+1];
	uint8_t value = dt[start+2]; // Might make this 16 bits 
	sprintf(cmd_motor_debug_buf,"MOTOR: %d,%d",index,value);
	cmd_debug_log(cmd_motor_debug_buf,0,DATA_LEN);
	
	int motor_velocity = (((int)value)*65443)/255; //TLRTW Specific
	motor_set_motor_velocity(index,motor_velocity);
}



// Servo specific commands:  0 is L |1 is R
// 16 bits in the future 
void set_servo(uint8_t* dt,int start,int end){
	uint8_t index = dt[start+1];
	uint8_t value = dt[start+2]; // Might make this 16 bits 
	sprintf(cmd_servo_debug_buf,"SERVO: %d,%d",index,value);
	cmd_debug_log(cmd_servo_debug_buf,0,DATA_LEN);

	uint16_t s_end = 9000;
	uint16_t s_start = 2000;
	uint16_t s_diff = s_end-s_start;
	uint16_t value_16 = (uint16_t)value;

	value_16  = s_start + ((s_diff*value_16)/255);
	servolist_set_servo_position(index,value_16);
}


int process_commands(
		uint8_t* in_buf,
		int start_index, 
		int end_index, 
		uint8_t* out_buf,
		int factor
		)
{
	
	init_cmd_debug_log_buf();
	uint8_t cmd = in_buf[start_index];
	// Print to Display
	if(cmd=='D'){
		cmd_debug_log(in_buf,start_index+1,end_index);
	}else if(cmd=='M'){
		set_motor(in_buf,start_index,end_index);
	}else if(cmd=='S'){
		set_servo(in_buf,start_index,end_index);
	}else{
		out_buf[2+factor]=ERR_INV_CMD;
		return ERR_INV_CMD;
	}
	out_buf[2+factor]=OK;
	return OK;
}

