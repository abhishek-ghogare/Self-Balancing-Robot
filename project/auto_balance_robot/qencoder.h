/*
 * qencoder.h
 *
 *  Created on: Oct 19, 2016
 *      Author: Thor
 */

#ifndef QENCODER_H_
#define QENCODER_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/qei.h"

#define QENCODER_MAX			2147483647	// Max value of the encoder
#define QENCODER_START			1073741823	// Starting point of the encoder, set to mid of the max

void init_qencoder();
int32_t get_position();


#endif /* QENCODER_H_ */
