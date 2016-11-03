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


int main(void) {
    char outstr[100];
	uint32_t ui32ADC0Value[4];
	volatile uint32_t y_axis, x_axis;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueF;
	SysCtlClockSet(
			SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 );

	 SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	 GPIOPinConfigure(GPIO_PA0_U0RX);
	 GPIOPinConfigure(GPIO_PA1_U0TX);
	 GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
	 GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2
	 UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	 (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
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



	ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	ADCHardwareOversampleConfigure(ADC1_BASE, 64);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH7);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH7 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);

	ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH6 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC1_BASE, 1);
	while (1) {
		ADCIntClear(ADC0_BASE, 1);
		ADCIntClear(ADC1_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC1_BASE, 1);
		while (!ADCIntStatus(ADC0_BASE, 1, false) && !ADCIntStatus(ADC1_BASE, 1, false)) {
		}

		if (ADCIntStatus(ADC0_BASE, 1, false)) {
			ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
			y_axis = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2]
					+ ui32ADC0Value[3] + 2) / 4;
		}

		if (ADCIntStatus(ADC1_BASE, 1, false)) {
			ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC0Value);
			x_axis = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2]
					+ ui32ADC0Value[3] + 2) / 4;
		}

		itoa(2048 - (int)(x_axis),outstr);
        UARTEchoString("\n\rX: ");
        UARTEchoString(outstr);
        itoa((int)(y_axis) - 2048,outstr);
        UARTEchoString("\n\rY: ");
        UARTEchoString(outstr);
		//if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
	}
}
