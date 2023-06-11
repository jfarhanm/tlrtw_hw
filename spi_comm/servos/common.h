#ifndef SERVO_COMMON_H
#define SERVO_COMMON_H
void servolist_init(int max_num_servos);
int servolist_add_servo(int pin, int channel, int max);
void servolist_set_servo_position(int servo,int position);

// This is TLRTW  specific 
void init_cam_servos();
#endif
