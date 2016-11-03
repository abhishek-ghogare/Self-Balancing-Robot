#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include <time.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#include "buttons.h"


#define PWM_FREQUENCY 55

volatile uint32_t ui32Load;
volatile uint32_t ui32PWMClock;
volatile uint32_t adjust_motor=10;
volatile uint8_t ui8Ad[3];

volatile uint8_t keypress_ctr=0;
volatile uint8_t mode=4;

uint8_t ui8LED = 2;
uint32_t delay_500ms = 6700000; // Delay worth 500ms
uint8_t delay = 1;              // Delay multiplier


// Simple sw2 button down handler
void onSW2ButtonDown(void) {
    keypress_ctr=0;
}

void onSW2ButtonUp(void) {
    if(keypress_ctr==1)
    {
        mode=0;
    } else if(keypress_ctr==2)
    {
        mode=1;
    } else if(keypress_ctr==3)
    {
        mode=2;
    } else if(keypress_ctr==4)
    {
        mode=4;
    } else if(keypress_ctr==5)
    {
        mode=5;
    } else
    {
        if (mode < 4)
        {
            // Set led light
            if (ui8Ad[mode]/11 < 1)
            {
                ui8Ad[mode] = 1;
            }
            else
                ui8Ad[mode]-=10;

            switch(mode)
            {
            case 0:
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Ad[mode] * ui32Load / 255);
                break;
            case 1:
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Ad[mode] * ui32Load / 255);
                break;
            case 2:
                PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Ad[mode] * ui32Load / 255);
                break;
            }
        }
        else if (mode==4)
        {
            delay++;
            if (delay>5)
                delay=5;
        }
        else if (mode==5)
        {
            adjust_motor--;
            if (adjust_motor <= 9)
            {
                adjust_motor = 9;
            }
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, adjust_motor * ui32Load / 360);
        }
    }
}



// Simple sw1 button down handler
void onSW1ButtonDown(void) {
}

void onSW1ButtonUp(void) {
    if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) {
        keypress_ctr++;
    }
    else if (mode < 4)
    {
        if (ui8Ad[mode]/(254-11) >= 1)
        {
            ui8Ad[mode] = 254;
        }
        else
            ui8Ad[mode]+=10;

        switch(mode)
        {
        case 0:
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Ad[mode] * ui32Load / 255);
            break;
        case 1:
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Ad[mode] * ui32Load / 255);
            break;
        case 2:
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Ad[mode] * ui32Load / 255);
            break;

        }
    }
    else if(mode == 4)
    {
        delay--;
        if (delay<1)
            delay=1;
    }
    else if (mode==5){
        adjust_motor++;
        if (adjust_motor > 44)
        {
            adjust_motor = 44;
        }
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, adjust_motor * ui32Load / 360);
    }
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
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    // Enable port F

    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PD0_M1PWM0);
    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);

    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;


    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

    //PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 255);

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Ad[0] * ui32Load / 255);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Ad[1] * ui32Load / 255);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Ad[2] * ui32Load / 255);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, adjust_motor * ui32Load / 360);

    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);


    btn_initialize();	// Initialize button inturrupts

    // Register simple button handlers
    btn_onSW1ButtonDownHdr = &onSW1ButtonDown;
    btn_onSW2ButtonDownHdr = &onSW2ButtonDown;
    btn_onSW1ButtonUpHdr = &onSW1ButtonUp;
    btn_onSW2ButtonUpHdr = &onSW2ButtonUp;
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
    //GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}


int main(void)
{
    volatile uint8_t in=1, dec=0, tmp;
    volatile uint32_t ctr=0;

    setup();
    led_pin_config();

    ui8Ad[0] = 255;
    ui8Ad[1] = 1;
    ui8Ad[2] = 1;


    // Turn on the LED
    //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
    while(1)
    {
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,ui8Ad[0] * ui32Load / 255);
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,ui8Ad[1] * ui32Load / 255);
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,ui8Ad[2] * ui32Load / 255);
        ctr++;

        SysCtlDelay(delay*delay_500ms/500);


        if(mode==4)
        {
            if ( dec==0 ) {
                ui8Ad[(in+1)%3]++;
                if (ui8Ad[(in+1)%3]>254)
                {
                    ui8Ad[(in+1)%3]=254;
                    dec=1;
                    in++;
                }
            } else {
                ui8Ad[(in+2)%3]--;
                if (ui8Ad[(in+2)%3]<1)
                {
                    ui8Ad[(in+2)%3]=1;
                    dec=0;
                }
            }
        }

        /*
        if(mode==4)
        {
            in = ctr%360;
            if (in==0) ctr=0;
            dec = in/120;
            in = in - (120*dec);
            ui8Ad[dec] = ((121-in)*254)/120;
            ui8Ad[(dec+2)%3] = (in*254)/120;
        }*/
    }
}
