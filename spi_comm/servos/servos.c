#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"


// Hardware Commands
typedef struct Servo{
	int PIN;
	int MAX;
	int MAX_ROT_TIME_Us;
	unsigned int PWM_SLICE;
	unsigned int PWM_CHANNEL;
	int position;

}Servo;

Servo* servo_new(int pin, int channel, int max){
	Servo* servo_tmp = malloc(sizeof(Servo));
	servo_tmp->PIN = pin;
	servo_tmp->MAX = max;
	servo_tmp->MAX_ROT_TIME_Us = 100000; //defaulting to 100ms
	servo_tmp->PWM_CHANNEL = channel;

    	gpio_set_function(servo_tmp->PIN, GPIO_FUNC_PWM);
    	servo_tmp->PWM_SLICE = pwm_gpio_to_slice_num(servo_tmp->PIN);

	pwm_set_clkdiv_int_frac (servo_tmp->PWM_SLICE,  38,3); // 50Hz
	pwm_set_wrap( servo_tmp->PWM_SLICE,65534);

	pwm_set_chan_level(servo_tmp->PWM_SLICE,servo_tmp->PWM_CHANNEL,0);
	pwm_set_enabled(servo_tmp->PWM_SLICE,true);
	return servo_tmp;
}

void servo_set_position(Servo* servo,uint16_t position){
	printf("POSITION IS %d",position);
	if(position>servo->MAX){
		return;
	}
	pwm_set_chan_level(servo->PWM_SLICE,servo->PWM_CHANNEL,position);
	servo->position=position;
}
// End of Hardware commands 



typedef struct ServoList{
	Servo** servos;
	int num_servos;
	int max_servos;
}ServoList;

ServoList* servo_list=NULL;
void servolist_init(int max_num_servos){
	if(servo_list!=NULL){
		return;
	}
	servo_list=malloc(sizeof(ServoList));
	servo_list->servos = malloc(sizeof(Servo*)*max_num_servos);
	servo_list->max_servos=max_num_servos;
	servo_list->num_servos=0;
}

int servolist_add_servo(int pin, int channel, int max){
	if(servo_list==NULL||servo_list->num_servos>servo_list->max_servos-1){
		return -1;
	}
	servo_list->servos[servo_list->num_servos] = servo_new(pin,channel,max);
	servo_list->num_servos++;
	return servo_list->num_servos;
}

void servolist_set_servo_position(int servo,int position){
	servo_set_position(servo_list->servos[servo],position);	
}




// This is a #[test]
// our application specific
// Copy this section to the defs.h
#define CAM_SERVO_CHAN_R 0
#define CAM_SERVO_CHAN_Y 1
#define CAM_SERVO_PIN_R 20
#define CAM_SERVO_PIN_Y 21
#define CAM_SERVO_MAX 255

// This is TRTLW specific 
void init_cam_servos(){
	servolist_init(2);
	int multiplier = 65465;
	servolist_add_servo(CAM_SERVO_PIN_R,CAM_SERVO_CHAN_R,255*multiplier);
	servolist_add_servo(CAM_SERVO_PIN_Y,CAM_SERVO_CHAN_Y,255*multiplier);
	int start = 2000;
	servolist_set_servo_position(CAM_SERVO_CHAN_R,start);
	servolist_set_servo_position(CAM_SERVO_CHAN_Y,start);
	sleep_ms(500);
}

/*
int main() {
	stdio_init_all();
	init_cam_servos();
	int roll_servo = CAM_SERVO_CHAN_R;
	int yaw_servo = CAM_SERVO_CHAN_Y;
	sleep_ms(1000);
	printf("\n>INITIALIZING");
	sleep_ms(1000);
	printf("\n>INITIALIZED");
	printf("SOURCE FREQ : %d",clock_get_hz(clk_sys));
	printf("\nSERVO A MAX : %d",(servo_list->servos[CAM_SERVO_CHAN_R])->MAX);
	printf("\nSERVO B MAX : %d",(servo_list->servos[CAM_SERVO_CHAN_Y])->MAX);

	uint16_t new_pos=0;
	int multiplier = 65535;
	printf("\n=========STAGE 1=========");
	printf("MAX_POS = %d",(254*multiplier)/255);
	printf("MAX_POS = %d",(25*multiplier)/255);
	
	int mid = 5500;
	int end = 9000;
	int start = 2000;

	servolist_set_servo_position(roll_servo,mid);
	printf(">\nSERVO R POS : %d",(servo_list->servos[CAM_SERVO_CHAN_R])->position);
	sleep_ms(1000);

	for(int i=mid;i<end;i+=100){
		new_pos = i;
		servolist_set_servo_position(roll_servo,new_pos);
		printf(">\nSERVO R POS : %d",(servo_list->servos[CAM_SERVO_CHAN_R])->position);
		sleep_ms(100);
	}

	servolist_set_servo_position(roll_servo,start);
	printf(">\nSERVO R POS : %d",(servo_list->servos[CAM_SERVO_CHAN_R])->position);
	sleep_ms(1000);


	for(int i=start;i<mid;i+=100){
		new_pos = i;
		servolist_set_servo_position(roll_servo,new_pos);
		printf(">\nSERVO R POS : %d",(servo_list->servos[CAM_SERVO_CHAN_R])->position);
		sleep_ms(100);
	}

	return 0 ;
	for(int i=0;i<100;i++){
		new_pos = (i*multiplier)/255;
		servolist_set_servo_position(roll_servo,new_pos);
		printf(">\nSERVO R POS : %d",(servo_list->servos[CAM_SERVO_CHAN_R])->position);
		sleep_ms(1000);
	}

	for(int i=0;i<25;i++){
		new_pos = (i*multiplier)/255;
		servolist_set_servo_position(yaw_servo,new_pos);
		printf(">\nSERVO Y POS : %d",(servo_list->servos[CAM_SERVO_CHAN_Y])->position);
		sleep_ms(1000);
	}


	printf("\n==========STAGE 2=========");
	servolist_set_servo_position(roll_servo,0);
	servolist_set_servo_position(yaw_servo,0);
	sleep_ms(1000);
	for(int i=0;i<25;i++){
		new_pos = (i*multiplier)/255;
		servolist_set_servo_position(yaw_servo,new_pos);
		servolist_set_servo_position(roll_servo,new_pos);
		printf("\n>SERVO R POS : %d",(servo_list->servos[CAM_SERVO_CHAN_R])->position);
		printf("\n>SERVO Y POS : %d",(servo_list->servos[CAM_SERVO_CHAN_Y])->position);
		sleep_ms(1000);
	}
}
*/
