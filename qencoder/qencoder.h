/*
*
* Team Id           : self_balancing_bot
* Author List       : Abhishek Ghogare
* Filename          : qencoder.h
* Functions         : void      QE_init_qencoder(),
*                     int32_t   QE_get_position(),
*                     int       QE_get_speed()
* Global Variables  : None
*
*/

#ifndef QENCODER_H_
#define QENCODER_H_

#include "../common.h"

#define QENCODER_MAX            1000000 // Max value of the encoder
#define QENCODER_START          500000  // Starting point of the encoder, set to mid of the max

MedFilter mf_vel;       // Filter for velocity

// Initialize quadrature encoder modules
void QE_init_qencoder();

// Get current encoder position reference to QENCODER_START
int32_t QE_get_position();

// Calculate motor speed using quadrature encoder
int QE_get_speed();


#endif /* QENCODER_H_ */
