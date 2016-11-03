/*
 * buttons.h
 *
 *  Created on: Aug 10, 2016
 *      Author: Thor
 */

#ifndef LAB2_BUTTONS_H_
#define LAB2_BUTTONS_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include <time.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

void btn_initialize();

void (*btn_onSW1ButtonDownHdr)(void);
void (*btn_onSW1ButtonUpHdr)(void);
void (*btn_onSW2ButtonDownHdr)(void);
void (*btn_onSW2ButtonUpHdr)(void);


#endif /* LAB2_BUTTONS_H_ */
