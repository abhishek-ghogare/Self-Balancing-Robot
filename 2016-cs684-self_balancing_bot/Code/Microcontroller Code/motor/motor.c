
#include "motor.h"

void MOT_init_motor(void) {
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	//
	//	Enable pwm for PF2 & PF3
	//  Selecting F2 and F3 pins since both are connected to LEDs,
	//  so that live PWM values can be tracked using LEDs
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
	SysCtlDelay(9000);

	MOT_ui32PWMClock = SysCtlClockGet() / 64;
	MOT_pwm_load = (MOT_ui32PWMClock / MOT_PWM_FREQUENCY) - 1;	// Load value should be less by 1 to generate tick on the end of the desired period

	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, MOT_pwm_load);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);

	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);

	// Enable PWM generator 3
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);


	//
	//	Enable motor inputs
	//

	// Right motor output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0);		// IN1
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);		// In2

	// Left motor output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1);		// IN1
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_2);		// In2
}


void MOT_stop() {
	// Stop motors
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0x00);
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x00);
}

void MOT_forward() {
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0xFF);
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0x00);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0xFF);
}

void MOT_reverse() {
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0x00);
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0xFF);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0xFF);
	GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x00);
}

void MOT_setSpeed(int32_t load) {
	uint32_t pwm_out = fabs(load);
	pwm_out = pwm_out == 0 ? 1 : pwm_out;

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, pwm_out + PWM_OFFSET_LEFT_MOTOR);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, pwm_out + PWM_OFFSET_RIGHT_MOTOR);

	if (load>0) {
		MOT_forward();
	} else {
		MOT_reverse();
	}
}
