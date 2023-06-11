#include <stdlib.h>
#include "pico/stdlib.h"
#include "common.h"
#include <stdio.h>
#include <stdint.h>

// Solving the errors in this was a bitch and a half.
//  TODO:
//  	I have not tested if the debug buffer will fill up. Possible exception
//  	Issue with first row not row printing, I think has been solved, but have to check.
//  Things have to, according to the new set of protocols, be perfect, but this is good enough 
//  for now.

// NOTE: THIS IS A #[test] IN THE PROGRAMMING LANGUAGE THAT CIVILIZED PEOPLE USE
// AND A USAGE EXAMPLE
/*
void main(){
	stdio_init_all();
	sleep_ms(1000);
	debug_logger_init();
	sleep_ms(1000);
	debug_logger_log("The Little Robot That Wouldn't");
	debug_logger_log(":-)");
	debug_logger_screen_clear();
	char* to_print = malloc(sizeof(char)*100);
	for (int i=0;i<20;i++)
	{
		sprintf(to_print,"NUMBER,%d",i);
		debug_logger_log(to_print);
		//debug_logger_screen_clear();
		sleep_ms(10000);
	}
}
*/
