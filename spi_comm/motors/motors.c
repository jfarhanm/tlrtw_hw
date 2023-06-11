#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "hardware/pwm.h"


// Hardware Commands
typedef struct Motor{
	int PIN;
	int MAX;
	unsigned int PWM_SLICE;
	unsigned int PWM_CHANNEL;
	int velocity;

}Motor;

Motor* motor_new(int pin, int channel, int max){
	Motor* _tmp = malloc(sizeof(Motor));
	_tmp->PIN = pin;
	_tmp->MAX = max;
	_tmp->PWM_CHANNEL = channel;

    	gpio_set_function(_tmp->PIN, GPIO_FUNC_PWM);
    	_tmp->PWM_SLICE = pwm_gpio_to_slice_num(_tmp->PIN);

	pwm_set_clkdiv_int_frac (_tmp->PWM_SLICE,  38,3); // 50Hz
	pwm_set_wrap( _tmp->PWM_SLICE,65534);

	pwm_set_chan_level(_tmp->PWM_SLICE,_tmp->PWM_CHANNEL,0);
	pwm_set_enabled(_tmp->PWM_SLICE,true);
	return _tmp;
}

void motor_set_velocity(Motor* motor,uint16_t velocity){
	if(velocity>motor->MAX){
		return;
	}
	pwm_set_chan_level(motor->PWM_SLICE,motor->PWM_CHANNEL,velocity);
	motor->velocity=velocity;
}
// End of Hardware commands 



typedef struct MotorList{
	Motor** motors;
	int num_motors;
	int max_motors;
}MotorList;

MotorList* motor_list=NULL;
void motorlist_init(int max_num_motors){
	if(motor_list!=NULL){
		return;
	}
	motor_list=malloc(sizeof(MotorList));
	motor_list->motors = malloc(sizeof(Motor*)*max_num_motors);
	motor_list->max_motors=max_num_motors;
	motor_list->num_motors=0;
}

int motorlist_add_motor(int pin, int channel, int max){
	if(motor_list==NULL||motor_list->num_motors>motor_list->max_motors-1){
		return -1;
	}
	motor_list->motors[motor_list->num_motors] = motor_new(pin,channel,max);
	motor_list->num_motors++;
	return motor_list->num_motors;
}

void motor_set_motor_velocity(int motor,int velocity){
	motor_set_velocity(motor_list->motors[motor],velocity);	
}




// This is a #[test]
// our application specific
// Copy this section to the defs.h
#define MOTOR_CHAN_L 0
#define MOTOR_CHAN_R 1

#define MOTOR_PIN_L 16
#define MOTOR_PIN_R 17

#define MOTOR_MAX 65454

void init_motors(){
	motorlist_init(2);
	int max = 65465;
	motorlist_add_motor(MOTOR_PIN_L,MOTOR_CHAN_L,max);
	motorlist_add_motor(MOTOR_PIN_R,MOTOR_CHAN_R,max);
	motor_set_motor_velocity(MOTOR_CHAN_L,0);
	motor_set_motor_velocity(MOTOR_CHAN_R,0);
	sleep_ms(5000);
}
/*
int main() {
	stdio_init_all();
	init_motors();
	int left_motor = MOTOR_CHAN_L;
	int right_motor = MOTOR_CHAN_R;
    	

	const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    	gpio_init(LED_PIN);
    	gpio_set_dir(LED_PIN, GPIO_OUT);	



	sleep_ms(1000);
	printf("\n>INITIALIZING MOTOR TEST");
	sleep_ms(1000);
	printf("\n>INITIALIZED MOTOR TEST");
	printf("\nSERVO A MAX : %d",(motor_list->motors[left_motor])->MAX);
	printf("\nSERVO B MAX : %d",(motor_list->motors[right_motor])->MAX);
	
	int end = 50000;
	int start = 0;
	
	int j = 0;
	for(int i=start;i<end;i+=5000){
		motor_set_motor_velocity(left_motor,i);
		printf(">\nSERVO R POS : %d",(motor_list->motors[left_motor])->velocity);
		sleep_ms(2000);
		j++;
		gpio_put(LED_PIN, j%2);
	}
}
*/
