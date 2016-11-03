/*
 * glcd.h
 *
 *  Created on: 20-Sep-2016
 *      Author: abhishek
 */

#ifndef GLCD_H_
#define GLCD_H_

uint32_t enable_delay = 1340;


void glcd_init();
void glcd_cleardisplay();
void glcd_cmd (unsigned char cmd);
void glcd_data (unsigned char data);
void glcd_setpage (unsigned char page);
void glcd_setcolumn (unsigned char column);

void glcd_init() {
    /**
     * 	Enable Enable pin connected to Port F Pin 0
     */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //
    // Unlock PF0 so we can change it to a GPIO input
    // Once we have enabled (unlocked) the commit register then re-lock it
    // to prevent further changes.  PF0 is muxed with NMI thus a special case.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);


    SysCtlDelay (134000);

    // Clear RST
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x00);
    SysCtlDelay (134000);

    // Set RST
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x20);

    // Initialize left side of GLCD
    // Set CS1=1 and CS2=0
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);

    // Select start line
    glcd_cmd(0xC0);
    // Send page
    glcd_cmd(0xB8);
    // Send colummn
    glcd_cmd(0x40);
    // Send GLCD on command
    glcd_cmd(0x3F);


    // Initialize right side of GLCD
    // Set CS1=0 and CS2=1
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);

    // Select start line
    glcd_cmd(0xC0);
    // Send page
    glcd_cmd(0xB8);
    // Send colummn
    glcd_cmd(0x40);
    // Send GLCD on command
    glcd_cmd(0x3F);
}

void glcd_cleardisplay() {
    unsigned char i,j;
    for (i=0 ; i<8 ; ++i) {
        glcd_setpage(i);
        for (j=0 ; j<128 ; ++j) {
            glcd_setcolumn(j);

            glcd_data(0x00);
        }
    }
}

void glcd_cmd (unsigned char cmd) {
    // Clear data line
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 , 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 , 0x00);

    // RS = 0
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x00);

    // Put command on data lines
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 , cmd);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 , cmd);

    // Generate high to low pulse on enable
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x01);
    SysCtlDelay (100);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
}


void glcd_data (unsigned char data) {
    // Clear data line
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 , 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 , 0x00);

    // RS = 1
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x40);

    // Put command on data lines
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 , data);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 , data);

    // Generate high to low pulse on enable
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x01);
    SysCtlDelay (100);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
}

void glcd_setpage (unsigned char page) {
    // Set CS1 to seletc right side screen (column > 64)
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);

    // Select the page
    glcd_cmd(0xB8 | page);
    //SysCtlDelay(100);

    // Set CS2
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);

    // Select the page
    glcd_cmd(0xB8 | page);
    //SysCtlDelay(100);
}

void glcd_setcolumn (unsigned char column) {
    if (column < 64) {
        // Set CS1
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);

        // Select column on left
        glcd_cmd(0x40 | column);
        //SysCtlDelay(6700);
    } else {
        // Set CS2
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);

        // Select column on left
        glcd_cmd(0x40 | (column-64));
        //SysCtlDelay(6700);
    }
}
unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void glcd_draw_block(int x, int y) {
	int page = y/8, i;
	unsigned char rem = y%8;
	rem = 0xFF >> rem;
	rem = reverse(rem);

	for (i=0 ; i<8 ; i++) {
		glcd_setpage(page);
		glcd_setcolumn(x+i);
		glcd_data(rem);
		glcd_setpage(page+1);
		glcd_setcolumn(x+i);
		glcd_data(~rem);
	}
}

void glcd_erase_block(int x, int y) {
	int page = y/8, i;

	for (i=0 ; i<8 ; i++) {
		glcd_setpage(page);
		glcd_setcolumn(x+i);
		glcd_data(0x00);
		glcd_setpage(page+1);
		glcd_setcolumn(x+i);
		glcd_data(0x00);
	}
}

void glcd_draw_image(const unsigned char img[]) {
	int i,p;
	for(i=0 ; i<128 ; ++i) {
		for (p=0 ; p<8 ; ++p) {
			glcd_setpage(p);
			glcd_setcolumn(i);
			glcd_data(img[i + p*128]);
		}
	}
}


#endif /* GLCD_H_ */
