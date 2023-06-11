#ifndef MOTOR_COMMON_H
#define MOTOR_COMMON_H
void motorlist_init(int max_num_motors);
int motorlist_add_motor(int pin, int channel, int max);
void motor_set_motor_velocity(int motor,int velocity);
// This is TLRTW specific
void init_motors();
#endif
