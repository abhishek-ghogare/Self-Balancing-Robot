/*
 * common.h
 *
 *  Created on: Nov 2, 2016
 *      Author: Thor
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/qei.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"


#include "inc/hw_gpio.h"
#include "inc/hw_i2c.h"
//#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"


#include "mpu_9250.h"
#include "i2c/i2c.h"
#include "uart/uart.h"
#include "medfilter/medfilter.h"
#include "buttons/buttons.h"
#include "qencoder/qencoder.h"
#include "motor/motor.h"
#include "pid/pid.h"


#define GYRO_SENSITIVITY 		131			// All angles are scaled by this factor for more accuracy
#define ANGLE_SCALING_FACTOR 	100			// All angles are scaled by this factor for more accuracy
#define BOT_START_ANGLE 		20			// Threshold angle to start bot, i.e. START bot if angle of inclination is between (-BOT_START_ANGLE, +BOT_START_ANGLE)
#define BOT_STOP_ANGLE 			40			// Threshold angle to stop bot, i.e. STOP bot if angle of inclination is beyond (-BOT_STOP_ANGLE, +BOT_STOP_ANGLE)
#define BOT_START_WAIT_COUNT	20			// Count till this number before STARTING the bot, count only when angle is between (-BOT_START_ANGLE, +BOT_START_ANGLE)
#define BOT_STOP_WAIT_COUNT		50			// Count till this number before STOPPING the bot, count only when angle is between (-BOT_STOP_ANGLE, +BOT_STOP_ANGLE)
#define PWM_OFFSET_LEFT_MOTOR	817			// Left motor starts moving at this pwm value, add this to final pwm value
#define PWM_OFFSET_RIGHT_MOTOR	600			// Right motor starts moving at this pwm value, add this to final pwm value
#define GYRO_OFFSET_Y			241			// Offset value of y component of gyroscope
#define ANGLE_OFFSET			60
#define MOTOR_MAX_SPEED			86284		// Speed of the encoder max, speed is in divisions per sec

#define angPid_kp				20
#define angPid_ki				3
#define angPid_kd				0.3

#define posPid_kp				0.03		// base value is 0.04, must be neaby this value
#define posPid_ki				0.000
#define posPid_kd				0.001

#define velPid_kp				0.01
#define velPid_ki				0.0000000
#define velPid_kd				0.0000000



#endif /* COMMON_H_ */
