#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include <time.h>
#include "buttons.h"

uint8_t ui8LED = 2;
uint8_t sw2Status = 0;
uint32_t delay_500ms = 6700000; // Delay worth 500ms
uint8_t delay = 1;              // Delay multiplier


// Simple sw2 button down handler
void onSW2ButtonDown(void) {
    // Set led light
    if (ui8LED == 8)
    {
        ui8LED = 2;
    }
    else
    {
        ui8LED = ui8LED*2;
    }
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
}

void onSW2ButtonUp(void) {
}



// Simple sw1 button down handler
void onSW1ButtonDown(void) {
    sw2Status++;
}

void onSW1ButtonUp(void) {
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
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    btn_initialize();	// Initialize button inturrupts

    // Register simple button handlers
    btn_onSW1ButtonDownHdr = &onSW1ButtonUp;
    btn_onSW2ButtonDownHdr = &onSW2ButtonUp;
    btn_onSW1ButtonUpHdr = &onSW1ButtonDown;
    btn_onSW2ButtonUpHdr = &onSW2ButtonDown;

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

    // Turn on the LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

    while(1)
    {
    }
}
