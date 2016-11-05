/*
*
* Team Id			: self_balancing_bot
* Author List		: Abhishek Ghogare
* Filename			: motor.h
* Functions			: void 	MOT_init_motor(),
* 				  	  void 	MOT_stop(),
* 				  	  void 	MOT_forward(),
* 				  	  void 	MOT_reverse(),
* 				  	  void 	MOT_setSpeed(int32_t load)
* Global Variables	: uint32_t MOT_pwm_load, MOT_ui32PWMClock
*
*/

#ifndef MOTOR_H_
#define MOTOR_H_

#include "../common.h"

#define MOT_PWM_FREQUENCY 55					// Frequency of the PWM module
volatile uint32_t MOT_pwm_load, MOT_ui32PWMClock;

void MOT_init_motor(void);

// Stop motors by supplying 0 to INA INB of both motors
void MOT_stop();

// Sets INA INB according to desired direction of movement
void MOT_forward();
void MOT_reverse();

// Set PWM value of both motors to load
void MOT_setSpeed(int32_t load);


#endif /* MOTOR_H_ */
