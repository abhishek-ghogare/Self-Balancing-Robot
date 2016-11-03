#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
//#include "inc/hw_ints.h"
#include "glcd.h"
#include "joystick.h"
#include "image.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Lab5-GLCD-images-bitmaps/part1/mickey.h"
#include "Lab5-GLCD-images-bitmaps/part1/logo.h"

#include "Lab5-GLCD-images-bitmaps/part3/one.h.txt"
#include "Lab5-GLCD-images-bitmaps/part3/two.h.txt"
#include "Lab5-GLCD-images-bitmaps/part3/three.h.txt"
#include "Lab5-GLCD-images-bitmaps/part3/four.h.txt"
#include "Lab5-GLCD-images-bitmaps/part3/five.h.txt"
#include "Lab5-GLCD-images-bitmaps/part3/six.h.txt"
#include "Lab5-GLCD-images-bitmaps/part3/seven.h.txt"
#include "Lab5-GLCD-images-bitmaps/part3/eight.h.txt"

int mode=0, prevmode;
void change_mode() {
	prevmode=mode;
	mode++;
	if (mode==4)
		mode=0;

	glcd_cleardisplay();
}


int main(void) {
    int32_t sw1_pin4=0;
	volatile uint32_t coords[2];
	uint32_t animation_delay_div = (5000000-1000)/4095; // between 10000000 to 1000
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);


    // Init PF4 as input for mode change
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    // Enable weak pullup resistor for PF4
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	joystick_init();

	glcd_init();
	glcd_cleardisplay();

	while (1) {

		// mode joystick mode=0
		if (mode==0) {
			joystick_get_coords(coords);
			block_loc_x_old = block_loc_x_new;
			block_loc_y_old = block_loc_y_new;

			block_loc_x_new = coords[0]/34;
			block_loc_y_new = coords[1]/73;

			if (block_loc_x_old != block_loc_x_new || block_loc_y_old != block_loc_y_new) {
				glcd_erase_block(block_loc_x_old,block_loc_y_old);
				glcd_draw_block(block_loc_x_new,block_loc_y_new);
			}
		}
		// mode still image mode = 1
		else if ( prevmode!=1 && mode==1) {
			glcd_draw_image(mickey);
		}
		else if ( prevmode!=2 && mode==2) {
			glcd_draw_image(logo);
		}
		// mode animation image mode = 3
		else if (mode==3) {
			glcd_draw_image(one);
			joystick_get_coords(coords);
			SysCtlDelay(1000+animation_delay_div*coords[1]);
			glcd_draw_image(two);
			joystick_get_coords(coords);
			SysCtlDelay(1000+animation_delay_div*coords[1]);
			glcd_draw_image(three);
			joystick_get_coords(coords);
			SysCtlDelay(1000+animation_delay_div*coords[1]);
			glcd_draw_image(four);
			joystick_get_coords(coords);
			SysCtlDelay(1000+animation_delay_div*coords[1]);
			glcd_draw_image(five);
			joystick_get_coords(coords);
			SysCtlDelay(1000+animation_delay_div*coords[1]);
			glcd_draw_image(six);
			joystick_get_coords(coords);
			SysCtlDelay(1000+animation_delay_div*coords[1]);
			glcd_draw_image(seven);
			joystick_get_coords(coords);
			SysCtlDelay(1000+animation_delay_div*coords[1]);
			glcd_draw_image(eight);
			joystick_get_coords(coords);
			SysCtlDelay(1000+animation_delay_div*coords[1]);
		}
		// Check button press
		if (sw1_pin4==GPIO_PIN_4 && !GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) {
			change_mode();
		}
		sw1_pin4 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
	}
}
