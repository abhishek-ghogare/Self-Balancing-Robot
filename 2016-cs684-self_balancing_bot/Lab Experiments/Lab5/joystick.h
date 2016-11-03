
#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "driverlib/adc.h"

int block_loc_x_old=120;
int block_loc_x_new=120;
int block_loc_y_old=56;
int block_loc_y_new=56;

uint32_t ui32ADC0Value[4];


void joystick_init() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 );

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
}

void joystick_get_coords(uint32_t coords[]) {
	ADCIntClear(ADC0_BASE, 1);
	ADCIntClear(ADC1_BASE, 1);
	ADCProcessorTrigger(ADC0_BASE, 1);
	ADCProcessorTrigger(ADC1_BASE, 1);
	while (!ADCIntStatus(ADC0_BASE, 1, false) && !ADCIntStatus(ADC1_BASE, 1, false)) {
	}

	if (ADCIntStatus(ADC0_BASE, 1, false)) {
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		// y axis
		coords[1] = 4095 - ((ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2]
				+ ui32ADC0Value[3] + 2) / 4);
	}

	if (ADCIntStatus(ADC1_BASE, 1, false)) {
		ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC0Value);
		// x axis
		coords[0] = 4095 - ((ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2]
				+ ui32ADC0Value[3] + 2) / 4);
	}
}



#endif /* JOYSTICK_H_ */
