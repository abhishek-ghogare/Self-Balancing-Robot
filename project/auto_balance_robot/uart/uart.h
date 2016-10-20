/*
 * uart.h
 *
 *  Created on: Oct 14, 2016
 *      Author: Thor
 */

#ifndef UART_UART_H_
#define UART_UART_H_
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char output[200];
int i;


void UARTIntHandler(void) {
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
    while (UARTCharsAvail(UART0_BASE)) //loop while there are chars
    {
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE)); //echo character
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //blink LED
        SysCtlDelay(SysCtlClockGet() / (1000 * 3)); //delay ~1 msec
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn off LED
    }
}

void UARTEchoString(char str[]) {
    int i=0;
    for (i=0 ; str[i] != '\0' ; ++i)
        UARTCharPut(UART0_BASE, str[i]);
}

int itoa(int value,char *ptr)
 {
	int count=0,temp;
	if(ptr==NULL)
		return 0;
	if(value==0)
	{
		*ptr='0';
		return 1;
	}

	if(value<0)
	{
		value*=(-1);
		*ptr++='-';
		count++;
	}
	for(temp=value;temp>0;temp/=10,ptr++);
	*ptr='\0';
	for(temp=value;temp>0;temp/=10)
	{
		*--ptr=temp%10+'0';
		count++;
	}
	return count;
 }



void init_uart() {
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	 GPIOPinConfigure(GPIO_PA0_U0RX);
	 GPIOPinConfigure(GPIO_PA1_U0TX);

	 GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	 UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	 IntMasterEnable(); //enable processor interrupts
	 IntEnable(INT_UART0); //enable the UART interrupt
	 UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts
	 UARTCharPut(UART0_BASE, 'E');
	 UARTCharPut(UART0_BASE, 'n');
	 UARTCharPut(UART0_BASE, 't');
	 UARTCharPut(UART0_BASE, 'e');
	 UARTCharPut(UART0_BASE, 'r');
	 UARTCharPut(UART0_BASE, ' ');
	 UARTCharPut(UART0_BASE, 'T');
	 UARTCharPut(UART0_BASE, 'e');
	 UARTCharPut(UART0_BASE, 'x');
	 UARTCharPut(UART0_BASE, 't');
	 UARTCharPut(UART0_BASE, ':');
     UARTCharPut(UART0_BASE, '\r');
     UARTCharPut(UART0_BASE, '\n');
}


void UART_reset_outbuf() {
	i=0;
}

void UART_print_int(int n) {
	i += itoa(n, output+i);
	output[i++] = '\t';
	output[i]   = '\0';
}

void UART_flush() {
	output[i++] = '\n';
	output[i]   = '\0';
	UARTEchoString(output);
	i=0;
}


#endif /* UART_UART_H_ */
