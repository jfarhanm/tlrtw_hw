#include "common.h"
#include<stdint.h>
#include<stdlib.h>
#include <stdio.h>

typedef struct DebugLogger{
	uint8_t* data;
	int NUM_ROWS;
	int ROW_SIZE;
	int queue_front;
	int queue_back;
}DebugLogger;

volatile static DebugLogger* debug_logger;

void debug_logger_init(){
	oled_init(64,128);
	debug_logger = malloc(sizeof(DebugLogger));
	debug_logger->ROW_SIZE=100;
	debug_logger->NUM_ROWS=50;
	debug_logger->data = malloc(sizeof(uint8_t)*(debug_logger->ROW_SIZE)*(debug_logger->NUM_ROWS));
	debug_logger->queue_back=0;
	debug_logger->queue_front=0;
}


void debug_logger_flush_one(){
	if(debug_logger->queue_front!=debug_logger->queue_back){
		printf("FLUSH ERROR REGION A");
		int res = oled_print_text(
				//(debug_logger->data) + (debug_logger->queue_front)*(debug_logger->ROW_SIZE)
				&(debug_logger->data[(debug_logger->queue_front)*(debug_logger->ROW_SIZE)])
			);
		// Do not be lazy and copy whatever is left to the beginning;
		if (res){
			printf("RES WAS ACTIVATED");
			int n=res;
			for(;debug_logger->data[n]!='\0';n++){
				debug_logger->data[debug_logger->queue_front]=debug_logger->data[debug_logger->queue_front+n];
			}
			debug_logger->data[debug_logger->queue_front+n-1]='\0';
		}else{
			debug_logger->queue_front = (debug_logger->queue_front + 1)%(debug_logger->NUM_ROWS);
		}
	}
	printf("FLUSH ERROR REGION B");
}



#define DEBUG_OK 0
#define DEBUG_ERR -1 
int debug_logger_log(char* text){
	printf("\n> STARTING DEBUG LOGGER");
	int logger_row = (debug_logger->queue_back)*(debug_logger->ROW_SIZE);
	int i=0;
	for(;text[i]!='\0';i++){
		if(i>=debug_logger->ROW_SIZE-1){
			printf("\n>DBGERR");
			return DEBUG_ERR;
		}
		debug_logger->data[logger_row + i+2]=(uint8_t)text[i];
	}

	debug_logger->data[logger_row]='\n';
	debug_logger->data[logger_row+1]='>';
	debug_logger->data[logger_row + 1 + ++i]='\0';
	debug_logger->queue_back = (debug_logger->queue_back + 1)%debug_logger->NUM_ROWS;
	debug_logger_flush_one(); //NOTE TO FUTURE JFARHANM : DO NOT FLUSH IN PRODUCTION
	return DEBUG_OK;
}

void debug_logger_screen_clear(){
	oled_clear();
}

void debug_logger_print_all_in_buffer(int delay_ms){
	while(debug_logger->queue_front!=debug_logger->queue_back){
		debug_logger_flush_one();
		sleep_ms(delay_ms);
	}
}



