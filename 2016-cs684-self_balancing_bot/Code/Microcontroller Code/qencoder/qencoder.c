/*
*
* Team Id			: self_balancing_bot
* Author List		: Abhishek Ghogare
* Filename			: qencoder.h
* Functions			: void 		QE_init_qencoder(),
* 				  	  int32_t 	QE_get_position(),
* 				  	  int 		QE_get_speed()
* Global Variables	: None
*
*/

#include "qencoder.h"

void QE_init_qencoder() {
	// Enable QEI Peripherals
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);

	//Unlock GPIOD7 - Like PF0 its used for NMI - Without this step it doesn't work
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY; //In Tiva include this is the same as "_DD" in older versions (0x4C4F434B)
	HWREG(GPIO_PORTD_BASE + GPIO_O_CR)  |= 0x80;
	HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

	//Set Pins to be PHA0 PHB0 PHA1 PHB1
	GPIOPinConfigure(GPIO_PD6_PHA0);
	GPIOPinConfigure(GPIO_PD7_PHB0);
	GPIOPinConfigure(GPIO_PC5_PHA1);
	GPIOPinConfigure(GPIO_PC6_PHB1);

	//Set GPIO pins for QEI. PhA0 -> PD6, PhB0 ->PD7. I believe this sets the pull up and makes them inputs
	GPIOPinTypeQEI(GPIO_PORTD_BASE, GPIO_PIN_6 |  GPIO_PIN_7);
	GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_5 |  GPIO_PIN_6);

	//DISable peripheral and int before configuration
	QEIDisable(QEI0_BASE);
	QEIDisable(QEI1_BASE);
	QEIIntDisable(QEI0_BASE,QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER | QEI_INTINDEX);
	QEIIntDisable(QEI1_BASE,QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER | QEI_INTINDEX);

	// Configure quadrature encoder, use an arbitrary top limit of 1000
	QEIConfigure(QEI0_BASE, (QEI_CONFIG_CAPTURE_A_B  | QEI_CONFIG_NO_RESET 	| QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP), QENCODER_MAX);
	QEIConfigure(QEI1_BASE, (QEI_CONFIG_CAPTURE_A_B  | QEI_CONFIG_NO_RESET 	| QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP), QENCODER_MAX);

	// Enable the quadrature encoder.
	QEIEnable(QEI0_BASE);
	QEIEnable(QEI1_BASE);

	//Set position to a middle value so we can see if things are working
	QEIPositionSet(QEI0_BASE, QENCODER_START);
	QEIPositionSet(QEI1_BASE, QENCODER_START);

	MF_init_med_filter(&mf_vel);
}

int32_t QE_get_position() {
	return ((int32_t)QEIPositionGet(QEI1_BASE) - (int32_t)QENCODER_START);
}

int QE_get_speed() {
	double bot_velocity = 0;
	static uint32_t lastPosition = QENCODER_START;
	static int lastTime = 0;
	uint32_t nowPosition = QE_get_position();
	int nowTime = millis();

	if (nowTime > lastTime)	// Calculate only for valid interval
		bot_velocity = ((double)nowPosition - (double)lastPosition)*1000.0 / ((double)nowTime - (double)lastTime);

	lastPosition = nowPosition;
	lastTime = nowTime;

	// Very high spikes because of encoder calculation faults
	// So, need to filter calculated velocity,
	// Filtering stopped very strong vibrations that used to occur for around every 500ms
	bot_velocity = MF_med_filter(&mf_vel, bot_velocity);

	return bot_velocity;
}
