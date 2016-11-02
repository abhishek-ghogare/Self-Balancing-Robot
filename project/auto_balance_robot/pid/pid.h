#ifndef PID_v1_h
#define PID_v1_h
#define LIBRARY_VERSION	1.1.1

#include "../common.h"

//Constants used in some of the functions below
#define PID_AUTOMATIC	1
#define PID_MANUAL		0
#define PID_DIRECT  	0
#define PID_REVERSE  	1

typedef struct PID {
	double dispKp;				// * we'll hold on to the tuning parameters in user-entered
	double dispKi;				//   format for display purposes
	double dispKd;				//

	double kp;                  // * (P)roportional Tuning Parameter
	double ki;                  // * (I)ntegral Tuning Parameter
	double kd;                  // * (D)erivative Tuning Parameter

	int controllerDirection;

	double *input;              // * Pointers to the Input, Output, and Setpoint variables
	double *output;             //   This creates a hard link between the variables and the
	double *setpoint;           //   PID, freeing the user from having to constantly tell us
								//   what these values are.  with pointers we'll just know.

	unsigned long lastTime;
	double ITerm, lastInput;

	unsigned long SampleTime;
	double outMin, outMax;
	int inAuto;
} PID;


//commonly used functions **************************************************************************
PID PID_construct(double*, double*, double*,        // * constructor.  links the PID to the Input, Output, and
	double, double, double, int);     //   Setpoint.  Initial tuning parameters are also set here

void PID_setMode(PID *, int Mode);    // * sets PID to either Manual (0) or Auto (non-0)

int PID_compute(PID *);               // * performs the PID calculation.  it should be
									  //   called every time loop() cycles. ON/OFF and
									  //   calculation frequency can be set using SetMode
									  //   SetSampleTime respectively

void PID_setOutputLimits(PID *, double, double); //clamps the output to a specific range. 0-255 by default, but
									  //it's likely the user will want to change this depending on
									  //the application



//available but not commonly used functions ********************************************************

// * While most users will set the tunings once in the
//   constructor, this function gives the user the option
//   of changing tunings during runtime for Adaptive control
void PID_setTunings(PID *, double, double, double);

// * Sets the Direction, or "Action" of the controller. DIRECT
//   means the output will increase when error is positive. REVERSE
//   means the opposite.  it's very unlikely that this will be needed
//   once it is set in the constructor.
void PID_setControllerDirection(PID *, int);

// * sets the frequency, in Milliseconds, with which
//   the PID calculation is performed.  default is 100
void PID_setSampleTime(PID *, int);

uint64_t millis();

#endif

