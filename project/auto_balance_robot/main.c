#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "i2c.h"
#include "uart/uart.h"
#include "Arduino-PID-Library-master/PID_v1.h"
#include "medfilter.h"
#include "mpu_9250.h"
#include "buttons.h"
#include "qencoder.h"

#define GYRO_SENSITIVITY 		131			// All angles are scaled by this factor for more accuracy
#define ANGLE_SCALING_FACTOR 	100			// All angles are scaled by this factor for more accuracy
#define BOT_START_ANGLE 		20			// Threshold angle to start bot, i.e. START bot if angle of inclination is between (-BOT_START_ANGLE, +BOT_START_ANGLE)
#define BOT_STOP_ANGLE 			40			// Threshold angle to stop bot, i.e. STOP bot if angle of inclination is beyond (-BOT_STOP_ANGLE, +BOT_STOP_ANGLE)
#define BOT_START_WAIT_COUNT	20			// Count till this number before STARTING the bot, count only when angle is between (-BOT_START_ANGLE, +BOT_START_ANGLE)
#define BOT_STOP_WAIT_COUNT		50			// Count till this number before STOPPING the bot, count only when angle is between (-BOT_STOP_ANGLE, +BOT_STOP_ANGLE)
#define PWM_OFFSET_LEFT_MOTOR	817			// Left motor starts moving at this pwm value, add this to final pwm value
#define PWM_OFFSET_RIGHT_MOTOR	600			// Right motor starts moving at this pwm value, add this to final pwm value
#define ANGLE_OFFSET			60
#define MOTOR_MAX_SPEED			86284		// Speed of the encoder max, speed is in divisions per sec

/*
// PID constants
#define PID_KP					30
#define PID_KI					0
#define PID_KD					2
*/
double pidKP_anglePid					=20; //30;
double pidKI_anglePid					=3; //150;
double pidKD_anglePid					=0.3; //0.3;

double pidKP_posPid					= 0.03;	// base value is 0.04, must be neaby this value
double pidKI_posPid					= 0.000;//1;
double pidKD_posPid					= 0.001;//3;

double pidKP_velPid					= 0.01;
double pidKI_velPid					= 0.0000000;
double pidKD_velPid					= 0.0000000;//0.1;//20;

bool botStarted = false;

MedFilter mf_vel;
MedFilter mf_angle;

double bot_velocity = 0;
int calculate_velocity() {
	static uint32_t lastPosition = QENCODER_START;
	static int lastTime = 0;
	uint32_t nowPosition = get_position();
	int nowTime = millis();
/*
	if (abs((int)nowPosition - (int)lastPosition)/350 > 822611239)
		return bot_velocity;*/

	if (nowTime > lastTime)	// Calculate only for valid interval
		bot_velocity = ((double)nowPosition - (double)lastPosition)*1000.0 / ((double)nowTime - (double)lastTime);
/*
	if (bot_velocity > 57913934)
		return bot_velocity;*/

	lastPosition = nowPosition;
	lastTime = nowTime;

	bot_velocity = medfilter(&mf_vel, bot_velocity);

	return bot_velocity;
}



// Buttons
int last_sw1_down, last_sw2_down;
void sw1_down() {
	last_sw1_down = millis();
}
void sw2_down() {
	last_sw2_down = millis();
}

void sw1_up() {
	int dt = millis() - last_sw1_down;
	pidKP_anglePid += 1;
}
void sw2_up() {
	int dt = millis() - last_sw2_down;
	pidKP_anglePid -= 1;
}

// TODO:: TEST working
void MPU9250_write_int16(uint8_t reg, int16_t value) {
	I2CSend(MPU9250_ADDRESS, reg, 1, value>>8);
	I2CSend(MPU9250_ADDRESS, reg+1, 1, value & 0x00FF);
}

int16_t MPU9250_read_int16(uint8_t reg) {
	return (I2CReceive(MPU9250_ADDRESS, reg)<<8 | I2CReceive(MPU9250_ADDRESS, reg+1));
}




#define PWM_FREQUENCY 55
volatile uint32_t ui32Load, ui32PWMClock;
void InitMotor(void)
{
	    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	    //
	    //	Enable pwm for PF2 & PF3
	    //
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    // Enable port F
	    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	    GPIOPinConfigure(GPIO_PF2_M1PWM6);
	    GPIOPinConfigure(GPIO_PF3_M1PWM7);
		SysCtlDelay(9000);

	    ui32PWMClock = SysCtlClockGet() / 64;
	    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

	    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);
	    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	    //PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Ad[1] * ui32Load / 255);
	    //PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Ad[2] * ui32Load / 255);

	    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
/*
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 0);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 0);*/

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

/*
 * Get tilt angle calculated using accelerometer
 * Conversion factor :
 * 					360 Deg angle = 3600 return value
 */
double getAngleAcc() {
	int16_t ax = -MPU9250_read_int16(ACCEL_XOUT_H);
	//int16_t ay = -MPU9250_read_int16(ACCEL_YOUT_H);
	int16_t az =  MPU9250_read_int16(ACCEL_ZOUT_H);

	return medfilter( &mf_angle, ((-atan((double)ax/(double)az)) * 7.0 * 180.0 * ANGLE_SCALING_FACTOR)/22.0 ); // subtraction of offset at the end
}

void checkBotStability() {
	uint32_t whoami;
	int angle;
	static int count = 0;

	whoami = I2CReceive(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
	if (whoami != 0x71) {
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 1);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 1);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x02);
		botStarted = false;
		count = 0;
		return;
	} else {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
	}

	angle = abs(getAngleAcc());

	if (botStarted) {
		if (angle > BOT_STOP_ANGLE * ANGLE_SCALING_FACTOR) {
			// If angle is beyond the threshold, get one step closer to stop the bot
			if (count < BOT_STOP_WAIT_COUNT)
				count++;
			else {
				// Stop motors
				GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0x00);
				GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
				GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0x00);
				GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x00);

				botStarted = false;
				count = 0;
			}
		} else if (count > 0) {
			// Else get one step back from stopping the bot
			count--;
		}
	}

	if(!botStarted && whoami == 0x71 ) {
		// Stop motors
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0x00);
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
		GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0x00);
		GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x00);

		if(count < BOT_START_WAIT_COUNT) {
			if (angle < BOT_START_ANGLE * ANGLE_SCALING_FACTOR) {
				count++;
			} else if (count > 0) {
				count--;
			}

			// Show tilt angle on LEDs
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, (90*ANGLE_SCALING_FACTOR - angle) * ui32Load / (90*ANGLE_SCALING_FACTOR));
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, (90*ANGLE_SCALING_FACTOR - angle) * ui32Load / (90*ANGLE_SCALING_FACTOR));
		} else {
			botStarted = true;

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 1);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 1);

			count = 0;
		}
	}

	// TODO :: error display on LEDs
}

double scale(double value, double src_min, double src_max, double dest_min, double dest_max) {
	double t = (value-src_min)/(src_max-src_min);
	double dest_val = dest_min + t*(dest_max-dest_min);
	return dest_val;
}


void main () {
	int16_t gx, gy, gz;
	double pidSetpoint_anglePid=0, pidOutput_anglePid, pidInput_anglePid;
	double pidSetpoint_posPid=QENCODER_START, pidOutput_posPid, pidInput_posPid;
	double pidSetpoint_velPid=0, pidOutput_velPid, pidInput_velPid;
	double angle_acc, angle_gyro=0;
	uint64_t dt, lastTime;	// in milli sec

	PID angle_pid = PID_construct(&pidInput_anglePid, &pidOutput_anglePid , &pidSetpoint_anglePid, pidKP_anglePid, pidKI_anglePid, pidKD_anglePid, DIRECT);
	PID pos_pid = PID_construct(&pidInput_posPid, &pidOutput_posPid, &pidSetpoint_posPid, pidKP_posPid, pidKI_posPid, pidKD_posPid, REVERSE);
	PID vel_pid = PID_construct(&pidInput_velPid, &pidOutput_velPid, &pidSetpoint_velPid, pidKP_velPid, pidKI_velPid, pidKD_velPid, REVERSE);


	SetOutputLimits(&angle_pid, -180 * ANGLE_SCALING_FACTOR, 180 * ANGLE_SCALING_FACTOR);
	SetOutputLimits(&pos_pid, -300, 300);
	SetOutputLimits(&vel_pid, -500, 500);
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	init_uart();
    InitMotor();
	InitI2C0();
	init_qencoder();
	 InitMedFilter(&mf_angle);
	 InitMedFilter(&mf_vel);

/*
	QEIPositionSet(QEI1_BASE, QENCODER_START);
	while(1) {
		static uint32_t val=0, nval=0;

		nval = QEIPositionGet(QEI1_BASE);
		//if (nval!=val) {
			UART_reset_outbuf();
			UART_print_int(nval);
			UART_print_int(((double)nval-(double)val)/10);
			UART_print_int(calculate_velocity());
			UART_flush();
		//}
		val = nval;
		SysCtlDelay(900000);
	}*/
	/*PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 1);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 1);
	while(1){
		uint32_t whoami;
		double angle;
		static int count = 0;

		whoami = I2CReceive(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
		if (whoami != 0x71) {
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x02);
		} else {
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
		}
		angle = getAngleAcc();
		gx = -MPU9250_read_int16(GYRO_XOUT_H)+71;
		gy = -MPU9250_read_int16(GYRO_YOUT_H)+241;
		gz =  MPU9250_read_int16(GYRO_ZOUT_H)-219;
	}*/

	//btn_initialize();
	btn_onSW1ButtonUpHdr = sw1_up;
	btn_onSW2ButtonUpHdr = sw2_up;
	btn_onSW1ButtonDownHdr = sw1_down;
	btn_onSW2ButtonDownHdr = sw2_down;


	// Set gyro x offset to -71, i.e 0xFFB9
	//MPU9250_write_int16(XG_OFFSET_H, (0));
	//I2CSend(MPU9250_ADDRESS, XG_OFFSET_H, 0xFF);
	//I2CSend(MPU9250_ADDRESS, XG_OFFSET_L, 0xB9);

	// Set gyro y offset to -218, i.e 0xFF26
	//MPU9250_write_int16(YG_OFFSET_H, -3200);
	//I2CSend(MPU9250_ADDRESS, XG_OFFSET_H, 0xFF);
	//I2CSend(MPU9250_ADDRESS, XG_OFFSET_L, 0x26);

	// Set gyro z offset to 219, i.e 0x00DB
	//MPU9250_write_int16(ZG_OFFSET_H,0);
	//I2CSend(MPU9250_ADDRESS, XG_OFFSET_H, 0x00);
	//I2CSend(MPU9250_ADDRESS, XG_OFFSET_L, 0xDB);


	checkBotStability();

	pidInput_anglePid = angle_gyro;
	//turn the PID on
	SetMode(&angle_pid, AUTOMATIC);
	SetMode(&pos_pid, AUTOMATIC);
	SetMode(&vel_pid, AUTOMATIC);

	lastTime = millis();
	while (1) {
		// Gyroscope
		//gx = -MPU9250_read_int16(GYRO_XOUT_H)+71;
		gy = -MPU9250_read_int16(GYRO_YOUT_H)+241;
		//gz =  MPU9250_read_int16(GYRO_ZOUT_H)-219;

		calculate_velocity();

		dt = millis() - lastTime;
		angle_acc = getAngleAcc();

		lastTime = millis();

		//float comp_filter(float newAngle, float newRate)
		{
			double filterTerm0;
			double filterTerm1;
			double drift=0;
			double timeConstant;

			timeConstant=0.001; // default 1.0

			filterTerm0 = (angle_acc - angle_gyro) * timeConstant * timeConstant;
			drift += filterTerm0 * dt;
			filterTerm1 = drift + ((angle_acc - angle_gyro) * 2 * timeConstant) + ((double)gy * ANGLE_SCALING_FACTOR) / (GYRO_SENSITIVITY * 1000.0);	// Scale angle by; Calculated sensitivity = 85
			pidInput_anglePid = angle_gyro += (filterTerm1 * dt);

			//angle_gyro += ((double)gy * ANGLE_SCALING_FACTOR * dt) / 85000.0; // TODO :: TESTING

			//return previousAngle; // This is actually the current angle, but is stored for the next iteration
		}

		UART_reset_outbuf();


		//UART_print_int(bot_velocity);
		//UART_print_int(QEIPositionGet(QEI1_BASE));
		//UART_print_int(angle_gyro);
		//UART_print_int(angle_acc);

		checkBotStability();

		//UART_print_int(angle/1000);
		//UART_print_int(angle_acc);
		//UART_print_int(gx);
		//UART_print_int(gy);
		//UART_print_int(gz);
		//UART_print_int(ax);
		//UART_print_int(az);

		// TODO :: stop all pids when stopping the bot
		//pidInput_posPid = get_position();
		//Compute(&pos_pid);
		//pidSetpoint_anglePid = -pidOutput_posPid;
		//UART_print_int(pidOutput_posPid);

		// TODO:: scale input output to appropriate target range
		pidSetpoint_posPid = 0;
		pidInput_posPid = get_position();
		Compute(&pos_pid);
/*
		SetTunings(&angle_pid,
				scale(pidOutput_posPid, -300, 300, pidKP_anglePid, pidKP_anglePid*2),
				pidKI_anglePid,
				scale(pidOutput_posPid, -300, 300, pidKD_anglePid, pidKD_anglePid*1.8));*/

		pidInput_velPid = bot_velocity;
		pidSetpoint_velPid = 0;
		Compute(&vel_pid);

		pidSetpoint_anglePid = ANGLE_OFFSET + pidOutput_posPid + pidOutput_velPid;

		if(botStarted && Compute(&angle_pid)) {
			double load = scale(pidOutput_anglePid , -180*ANGLE_SCALING_FACTOR , 180*ANGLE_SCALING_FACTOR , -(double)ui32Load , (double)ui32Load );
			//double load = scale(pidOutput_velPid ,-(double)MOTOR_MAX_SPEED , (double)MOTOR_MAX_SPEED , -(double)ui32Load , (double)ui32Load );
			uint32_t pwm_out = fabs(load);
			pwm_out = pwm_out == 0 ? 1 : pwm_out;

			//UART_print_int(angle_pid.dispKp);
			//UART_print_int(pidOutput_posPid);
			//UART_print_int(pidOutput_anglePid);
			//UART_print_int(load);
			UART_print_int(pidOutput_posPid);
			UART_print_int(pidOutput_velPid);
			UART_print_int(pidSetpoint_anglePid);
			//UART_print_int(pidOutput_velPid);
			//UART_print_int(bot_velocity);
			//UART_print_int(pidInput_posPid);
			//UART_print_int(angle_gyro);
			//UART_print_int(bot_velocity);
			//UART_print_int(angle_gyro_raw/6800);
			//UART_print_int(Output>0?pwm_out:-(int)(pwm_out));

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, pwm_out + PWM_OFFSET_LEFT_MOTOR);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, pwm_out + PWM_OFFSET_RIGHT_MOTOR);
			if (load>0) {
				GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0xFF);
				GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0x00);
				GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0x00);
				GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0xFF);
			} else {
				GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0x00);
				GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0xFF);
				GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, 0xFF);
				GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2, 0x00);
			}
		}

		UART_flush();


		SysCtlDelay(90000);
	}
}


