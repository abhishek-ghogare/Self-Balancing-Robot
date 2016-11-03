#include "buttons.h"


// Button states
// 0 : Idle
// 1 : Press	:: first press
// 2 : Release	:: still pressed
char btn_sw1_state=0, btn_sw2_state=0;

void btn_onSW1ButtonDown(void);
void btn_onSW2ButtonDown(void);
void btn_onSW1ButtonUp(void);
void btn_onSW2ButtonUp(void);


/************************************************************************************
 * 	ALL Button UP/DOWN Handlers
 *
 *
 ************************************************************************************/
// Interrupt handler for all button down
void btn_onButtonDown(void) {/*
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4) {
    	btn_onSW1ButtonDown();
    } else if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0) {
    	btn_onSW2ButtonDown();
    }*/

    if ( (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4) && btn_sw1_state == 0)
    {
        btn_sw1_state = 1;
        TimerEnable(TIMER0_BASE, TIMER_A);
    }
    if  ((GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0) && btn_sw2_state == 0)
    {
        btn_sw2_state = 1;
        TimerEnable(TIMER0_BASE, TIMER_A);
    }
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
}

// Interrupt handler for all button up
void btn_onButtonUp(void) {
	// Disabling up interrupt
    /*if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4) {
    	btn_onSW1ButtonUp();
    } else if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0) {
    	btn_onSW2ButtonUp();
    }*/
}

/************************************************************************************
 * 	SW1 Button UP/DOWN Handlers
 *
 *
 ************************************************************************************/
/*void btn_onSW1ButtonDown(void) {
    GPIOIntRegister(GPIO_PORTF_BASE, btn_onButtonUp);               // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);  // Configure SW1 for rising edge trigger
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);                      // Clear interrupt flag

    if (btn_sw1_state == 0)
    {
    	btn_sw1_state = 1;
    	TimerEnable(TIMER0_BASE, TIMER_A);
    }
}

void btn_onSW1ButtonUp(void) {
    GPIOIntRegister(GPIO_PORTF_BASE, btn_onButtonDown);             // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE); // Configure PF4 for falling edge trigger
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);                      // Clear interrupt flag

	TimerDisable(TIMER0_BASE, TIMER_A);
    btn_sw1_state = 0; // Reset to idle state
}*/

/************************************************************************************
 * 	SW2 Button UP/DOWN Handlers
 *
 *
 ************************************************************************************/
/*void btn_onSW2ButtonDown(void) {
    GPIOIntRegister(GPIO_PORTF_BASE, btn_onButtonUp);               // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);  // Configure PF4 for rising edge trigger
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);                      // Clear interrupt flag

    if (btn_sw2_state == 0)
    {
    	btn_sw2_state = 1;
    	TimerEnable(TIMER0_BASE, TIMER_A);
    }
}

void btn_onSW2ButtonUp(void) {
    GPIOIntRegister(GPIO_PORTF_BASE, btn_onButtonDown);             // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE); // Configure SW2 for falling edge trigger
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);                      // Clear interrupt flag

	TimerDisable(TIMER0_BASE, TIMER_A);
    btn_sw2_state = 0; // Reset to idle state
}*/


/************************************************************************************
 * 	Timer Interrupt Handlers
 *
 *
 ************************************************************************************/
void Timer0IntHandler(void)
{
	// Get button pressed status
    int32_t sw1_pin4 = !GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
	int32_t sw2_pin0 = !GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state

	if (btn_sw1_state == 1) {				// If sw1 button is in state 1, i.e. if sw1 was pressed 10ms earlier
		if (sw1_pin4) {				        // If pin4 is low, i.e. if sw1 is pressed
    		btn_sw1_state = 2;				// Go in sw1 button state 2
    	    (*btn_onSW1ButtonDownHdr)();	// Execute the registered sw1 down handler by user
    	} else {
    		//btn_onSW1ButtonUp();			// Reset the button state
            if (btn_sw2_state==0)
    	        TimerDisable(TIMER0_BASE, TIMER_A);
    	    btn_sw1_state = 0; // Reset to idle state
    	}
    }
	else if (btn_sw1_state == 2 && !sw1_pin4) {		// If button was already in state 2 and pin4 is high now i.e. button is not pressed now
	    (*btn_onSW1ButtonUpHdr)();			// Execute button up handler
		//btn_onSW1ButtonUp();
	    if (btn_sw2_state==0)
	        TimerDisable(TIMER0_BASE, TIMER_A);
	    btn_sw1_state = 0; // Reset to idle state				// Reset button state
	}

	if (btn_sw2_state == 1) {				// If sw2 button is in state 1, i.e. if sw2 was pressed 10ms earlier
		if (sw2_pin0) {					    // If pin0 is low, i.e. if sw2 is pressed
    		btn_sw2_state = 2;				// Go in sw2 button state 2
    	    (*btn_onSW2ButtonDownHdr)();	// Execute the registered sw2 down handler by user
    	} else {
    		//btn_onSW2ButtonUp();			// Reset the button state
            if (btn_sw1_state==0)
                TimerDisable(TIMER0_BASE, TIMER_A);
            btn_sw2_state = 0; // Reset to idle state
    	}
    }
	else if (btn_sw2_state == 2 && !sw2_pin0) {		// If button was already in state 2 and pin0 is high now i.e. button is not pressed now
	    (*btn_onSW2ButtonUpHdr)();			// Execute button up handler
		//btn_onSW2ButtonUp();				// Reset button state
        if (btn_sw1_state==0)
            TimerDisable(TIMER0_BASE, TIMER_A);
        btn_sw2_state = 0; // Reset to idle state               // Reset button state
	}
}


/************************************************************************************
 * 	Button Initialization
 *
 *
 ************************************************************************************/
void btn_initialize() {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    // Enable port F

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
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);   // Disable interrupt for PF4 & PF0 (in case it was enabled)
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);     // Clear pending interrupts for PF4 & PF0

    GPIOIntRegister(GPIO_PORTF_BASE, btn_onButtonDown);         // Register handler function for port F

    // Configure PF4 & PF0 for falling edge trigger
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);    // Enable interrupt for PF4 & PF0


    // Timer initialization
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);				// Enable the clock on the timer0 peripheral
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);			// Configure Timer0A to be periodic

    TimerLoadSet(TIMER0_BASE, TIMER_A, (SysCtlClockGet() / 100) -1);	// Set timer to 10ms, subtracting 1 to make sure the interrupt is generated on 10th ms

    IntEnable(INT_TIMER0A);										// Enable Timer0A interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);			// Enable Timer0A interrupt on timeout event
    IntMasterEnable();											// Enable master interrupts
}
