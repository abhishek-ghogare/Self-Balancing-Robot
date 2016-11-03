/*
 * motor.h
 *
 *  Created on: Nov 2, 2016
 *      Author: Thor
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
