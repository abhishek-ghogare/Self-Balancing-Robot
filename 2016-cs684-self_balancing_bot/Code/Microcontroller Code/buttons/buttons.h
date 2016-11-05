/*
*
* Team Id			: self_balancing_bot
* Author List		: Abhishek Ghogare
* Filename			: buttons.h
* Functions			: void btn_initialize()
* Global Variables	: void (*btn_onSW1ButtonDownHdr)(void);
*					  void (*btn_onSW1ButtonUpHdr)(void);
*					  void (*btn_onSW2ButtonDownHdr)(void);
*					  void (*btn_onSW2ButtonUpHdr)(void);
*
*/


#ifndef LAB2_BUTTONS_H_
#define LAB2_BUTTONS_H_

//#include "inc/tm4c123gh6pm.h" // Commenting this include, it was causing comflict with hw_int.h
// #define INT_TIMER0A             35          // Defined in the C file which was declared in above header file

#include "../common.h"

void btn_initialize();

void (*btn_onSW1ButtonDownHdr)(void);
void (*btn_onSW1ButtonUpHdr)(void);
void (*btn_onSW2ButtonDownHdr)(void);
void (*btn_onSW2ButtonUpHdr)(void);




#endif /* LAB2_BUTTONS_H_ */
