#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include <time.h>

void onSW1ButtonDown(void);
void onSW1ButtonUp(void);
void onSW2ButtonDown(void);
void onSW2ButtonUp(void);

uint8_t ui8LED = 2;
uint8_t sw2Status = 0;
uint32_t delay_500ms = 6700000;	// Delay worth 500ms
uint8_t delay = 1;				// Delay multiplier

// Interrupt handler for all button down
void onButtonDown(void) {
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4) {
        onSW1ButtonDown();
    } else if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0) {
    	onSW2ButtonDown();
    }
}

// Interrupt handler for all button up
void onButtonUp(void) {
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4) {
        onSW1ButtonUp();
    } else if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0) {
    	onSW2ButtonUp();
    }
}

void onSW2ButtonDown(void) {
	// PF4 was interrupt cause
	GPIOIntRegister(GPIO_PORTF_BASE, onButtonUp);   				// Register our handler function for port F
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);  // Configure PF4 for rising edge trigger
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);  					// Clear interrupt flag

	// Set led light
	if (ui8LED == 8)
	{
		ui8LED = 2;
	}
	else
	{
		ui8LED = ui8LED*2;
	}
}

void onSW2ButtonUp(void) {
	// PF4 was interrupt cause
	GPIOIntRegister(GPIO_PORTF_BASE, onButtonDown); 				// Register our handler function for port F
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);	// Configure SW2 for falling edge trigger
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);  					// Clear interrupt flag
}



void onSW1ButtonDown(void) {
	// PF4 was interrupt cause
	GPIOIntRegister(GPIO_PORTF_BASE, onButtonUp);   				// Register our handler function for port F
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);	// Configure SW1 for rising edge trigger
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);  					// Clear interrupt flag

	// Circulate over delays
	if (delay == 4)
	{
		delay = 1;
	}
	else
	{
		delay = delay*2;
	}
}

void onSW1ButtonUp(void) {
	// PF4 was interrupt cause
	sw2Status++;
	GPIOIntRegister(GPIO_PORTF_BASE, onButtonDown); 				// Register our handler function for port F
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);	// Configure PF4 for falling edge trigger
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);  					// Clear interrupt flag
}

/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals

* Example Call: setup();

*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	// Enable port F

    //
    // Unlock PF0 so we can change it to a GPIO input
    // Once we have enabled (unlocked) the commit register then re-lock it
    // to prevent further changes.  PF0 is muxed with NMI thus a special case.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    // Init PF4 and PF0 as input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    // Enable weak pullup resistor for PF4
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    // Interrupt setup sw1 PF4 & sw2 PF0
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);	// Disable interrupt for PF4 & PF0 (in case it was enabled)
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);		// Clear pending interrupts for PF4 & PF0

    GPIOIntRegister(GPIO_PORTF_BASE, onButtonDown);     		// Register handler function for port F

    // Configure PF4 & PF0 for falling edge trigger
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);	// Enable interrupt for PF4 & PF0
}

/*

* Function Name: led_pin_config()

* Input: none

* Output: none

* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output. On this pin Red, Blue and Green LEDs are connected.

* Example Call: led_pin_config();

*/

void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}


int main(void)
{
	setup();
	led_pin_config();

	while(1)
	{
		// Turn on the LED
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		// Delay for a bit
		SysCtlDelay(delay*delay_500ms);
		// Turn on the LED
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		// Delay for a bit
		SysCtlDelay(delay*delay_500ms);
	}
}
