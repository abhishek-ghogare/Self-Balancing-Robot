#include "common.h"

bool botStarted = false;

MedFilter mf_angleAcc;	// Filter for angle calculated using accelerometer


// Write int16 data to given MPU register addr
void MPU9250_write_int16(uint8_t reg, int16_t value) {
	I2CSend(MPU9250_ADDRESS, reg, 1, value>>8);
	I2CSend(MPU9250_ADDRESS, reg+1, 1, value & 0x00FF);
}

// Read int16 data from given MPU register addr
int16_t MPU9250_read_int16(uint8_t reg) {
	return (I2CReceive(MPU9250_ADDRESS, reg)<<8 | I2CReceive(MPU9250_ADDRESS, reg+1));
}


/*
 * Get tilt angle calculated using accelerometer
 * Conversion factor :
 * 					360 Deg angle = 360*ANGLE_SCALING_FACTOR return value
 */
double getAngleAcc() {
	// MPU is attached to the robot such that +x axis points to forward direction
	// and +z axis points to downward direction
	int16_t ax = -MPU9250_read_int16(ACCEL_XOUT_H);		// Read x-axis component of the accelerometer
	int16_t az =  MPU9250_read_int16(ACCEL_ZOUT_H);		// Read z-axis component of the accelerometer

	// Using tan angle to eliminate calculation of max value of the sensor along an axis, also reduces error
	double angle = ((-atan((double)ax/(double)az)) * 7.0 * 180.0 * ANGLE_SCALING_FACTOR)/22.0;

	return MF_med_filter( &mf_angleAcc, angle);
}

// Returns final angle calculated using complementary filter
// Returned angle is scaled by ANGLE_SCALING_FACTOR
double getAngle() {
	static uint64_t dt=0, lastTime=0;	// in milli sec
	static double angle = 0;		// Filtered angle
	int16_t gy;
	double angle_acc = getAngleAcc();
	double gyro_angle_change;

	dt = millis() - lastTime;	// Time changed
	lastTime = millis();
	gy = -MPU9250_read_int16(GYRO_YOUT_H) + GYRO_OFFSET_Y;	// Adding gyro offset to read 0 on steady state
	{
		// Complementary filter implementation
		double filterTerm0;
		double filterTerm1;
		double drift = 0;				// Drift from actual angle
		double timeConstant = 0.001;	// Decide how quickly gyro angle should sync with acc angle

		filterTerm0 = (angle_acc - angle) * timeConstant * timeConstant;
		drift += filterTerm0 * dt;
		gyro_angle_change = ((double)gy * ANGLE_SCALING_FACTOR) / (GYRO_SENSITIVITY * 1000.0);	// Calculate change in angle using gyro
		filterTerm1 = drift + ((angle_acc - angle) * 2 * timeConstant) + gyro_angle_change;		// Scale angle by; Calculated sensitivity was 85
		angle += (filterTerm1 * dt);
	}

	return angle;
}



// Checks whether the bot is in upright position.
// If not, decreases the STOP counter
// else increment START counter.
// Stops or starts robot activity based on the START/STOP counter values
void checkBotStability() {
	uint32_t whoami;
	int angle;
	static int count = 0;

	// Check if MPU is working correctly
	whoami = I2CReceive(MPU9250_ADDRESS, WHO_AM_I_MPU9250);	// Read whoami register of MPU
	if (whoami != 0x71) {									// whoami must be equal to 0x71
		// MPU stopped working, stop everything and show RED LED
		MOT_stop();			// Stop motor
		MOT_setSpeed(1);	// Set lowest speed
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x02);	// RED LED on, to show unrecoverable fault
		botStarted = false;	// Stop bot
		count = 0;			// Reset counter
		return;
	} else {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);	// RED LED off
	}

	angle = abs(getAngleAcc());

	if (botStarted) {
		if (angle > BOT_STOP_ANGLE * ANGLE_SCALING_FACTOR) {
			// If angle is beyond the threshold, get one step closer to stop the bot
			if (count < BOT_STOP_WAIT_COUNT)
				count++;
			else {
				MOT_stop();			// Stop motors
				botStarted = false;	// Stop bot
				count = 0;			// Initialize START counter
			}
		} else if (count > 0) {
			// Else get one step back from stopping the bot
			count--;				// Decrement START counter
		}
	}

	if(!botStarted && whoami == 0x71 ) {
		// Stop motors
		MOT_stop();

		// If START counter has not reached the threshold
		if(count < BOT_START_WAIT_COUNT) {
			if (angle < BOT_START_ANGLE * ANGLE_SCALING_FACTOR) {
				// Take one step towards starting bot
				count++;
			} else if (count > 0) {
				// Take one step backwards from starting bot
				count--;
			}

			// Show tilt angle on LEDs, as the bot stands vertically, LED gets brighter
			MOT_setSpeed((90*ANGLE_SCALING_FACTOR - angle) * MOT_pwm_load / (90*ANGLE_SCALING_FACTOR));
		} else {
			botStarted = true;	// Start bot
			MOT_setSpeed(1);	// Set minimum bot speed
			count = 0;			// Reset START counter
		}
	}
}


// Scale the value from source range to destination range
// value				: value in source range
// [scr_min, src_max] 	: source range
// [dest_min, dest_max] : destination range
// return 				: value in destination range
double scale(double value, double src_min, double src_max, double dest_min, double dest_max) {
	double t = (value-src_min)/(src_max-src_min);
	double dest_val = dest_min + t*(dest_max-dest_min);
	return dest_val;
}


void main () {
	double 	bot_motor_speed = 0;				// Velocity of wheel calculated using quadrature encoder
	double 	angPid_setpoint	= 0,				// angle PID parameters
			angPid_output,
			angPid_input;
	double 	posPid_setpoint	= QENCODER_START,	// Position PID controller parameters
			posPid_output,
			posPid_input;
	double 	velPid_setpoint	= 0,				// Velocity PID controller parameters
			velPid_output,
			velPid_input;
	double 	bot_angle=0;						// Angle made by bot with verticle line

	// Construct PIDs
	PID angle_pid 	= PID_construct(
			&angPid_input, &angPid_output , &angPid_setpoint,	// angle PID input, output, setpoint
			angPid_kp, angPid_ki, angPid_kd,					// angle PID constants
			PID_DIRECT);										// PID direct direction
	PID pos_pid 	= PID_construct(
			&posPid_input, &posPid_output, &posPid_setpoint,	// position PID input, output, setpoint
			posPid_kp, posPid_ki, posPid_kd,					// position PID constants
			PID_REVERSE);										// PID reverse direction
	PID vel_pid 	= PID_construct(
			&velPid_input, &velPid_output, &velPid_setpoint,	// velocity PID input, output, setpoint
			velPid_kp, velPid_ki, velPid_kd,					// velocity PID constants
			PID_REVERSE);										// PID reverse direction

	// Initialize clocks
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	// Initialize modules
	UART_init();
    MOT_init_motor();
	I2C_init_I2C0();
	QE_init_qencoder();
	MF_init_med_filter(&mf_angleAcc);

	// Set PID limits
	PID_setOutputLimits(&angle_pid, -180 * ANGLE_SCALING_FACTOR, 180 * ANGLE_SCALING_FACTOR);
	PID_setOutputLimits(&pos_pid, -150, 150);
	PID_setOutputLimits(&vel_pid, -500, 500);

	// Enable all PIDs
	PID_setMode(&angle_pid, PID_AUTOMATIC);
	PID_setMode(&pos_pid, PID_AUTOMATIC);
	PID_setMode(&vel_pid, PID_AUTOMATIC);

	// Infinite loop
	while (1) {
		bot_angle = getAngle();		// Calculate current filtered angle
		angPid_input = bot_angle;	// Set angle PID input with current angle

		checkBotStability();		// Check if sensor data is valid

		posPid_input = QE_get_position();	// Set PID input to current value
		posPid_setpoint = 0;				// Set PID setpoint to target value
		PID_compute(&pos_pid);				// Calculate PID output

		bot_motor_speed = QE_get_speed();
		velPid_input = bot_motor_speed;
		velPid_setpoint = 0;
		PID_compute(&vel_pid);

		// Add offset to the target setpoint of angle, so that position and velocity is maintained
		angPid_setpoint = ANGLE_OFFSET + posPid_output + velPid_output;

		if(botStarted && PID_compute(&angle_pid)) {
			// Scale PID output to motor PWM range
			double load = scale(angPid_output , -180*ANGLE_SCALING_FACTOR , 180*ANGLE_SCALING_FACTOR , -(double)MOT_pwm_load , (double)MOT_pwm_load );
			MOT_setSpeed(load);

			UART_print_int(bot_angle);
			UART_print_int(posPid_output);
			UART_print_int(velPid_output);
			UART_print_int(angPid_setpoint);
			UART_flush();
		}

		SysCtlDelay(90000);
	}
}


