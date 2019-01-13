/*
*
* Team Id           : self_balancing_bot
* Author List       : Abhishek Ghogare
* Filename          : uart.h
* Functions         : void      UART_int_handler(),
*                     void      UART_init(),
*                     void      UART_echo_string(char []),
*                     void      UART_reset_outbuf(),
*                     void      UART_print_int(int),
*                     void      UART_flush()
* Global Variables  : None
*
*/

#ifndef UART_UART_H_
#define UART_UART_H_

#include "../common.h"

// UART interrupt handler
void UART_int_handler(void);

// Initialize UART module
void UART_init();

// Echo string to UART port
void UART_echo_string(char str[]);


void UART_reset_outbuf();   // Reset output buffer
void UART_print_int(int n); // Print a number to buffer
void UART_flush();          // Flush buffer to UART


#endif /* UART_UART_H_ */
