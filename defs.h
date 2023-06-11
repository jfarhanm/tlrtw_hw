// MOTOR FUNCTIONS
void motorlist_init(int max_num_motors);
int motorlist_add_motor(int pin, int channel, int max);
void motor_set_motor_velocity(int motor,int velocity);
void init_motors();

// SERVO FUNCTIONS
void servolist_init(int max_num_servos);
int servolist_add_servo(int pin, int channel, int max);
void servolist_set_servo_position(int servo,int position);
void init_cam_servos();

