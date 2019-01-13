/**********************************************************************************************
 * Arduino PID Library - Version 1.1.1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/
#include "pid.h"

void Initialize(PID * pidp);



#define DWT_O_CYCCNT 0x00000004
void EnableTiming(void){
  static int enabled = 0;
  if (!enabled){
     HWREG(NVIC_DBG_INT) |= 0x01000000;    /*enable TRCENA bit in NVIC_DBG_INT*/
     HWREG(DWT_BASE + DWT_O_CYCCNT) = 0;   /* reset the counter */
     HWREG(DWT_BASE) |= 0x01;              /* enable the counter */
     enabled = 1;
   }
}
uint64_t millis() {
  uint64_t val = HWREG(DWT_BASE + DWT_O_CYCCNT);
  val = val*1000;
  val /= (uint64_t)(SysCtlClockGet());

  return val; // at the beginning of the code
}

/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up 
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
PID PID_construct(double* Input, double* Output, double* Setpoint,
        double Kp, double Ki, double Kd, int ControllerDirection)
{

  PID pid;

  EnableTiming();

    pid.output = Output;
    pid.input = Input;
    pid.setpoint = Setpoint;
    pid.inAuto = 0;
  
  PID_setOutputLimits(&pid, 0, 255);        //default output limit corresponds to
                        //the arduino pwm limits

  pid.SampleTime = 10;              //default Controller Sample Time is 0.1 seconds

    PID_setControllerDirection(&pid, ControllerDirection);
    PID_setTunings(&pid, Kp, Ki, Kd);

    pid.lastTime = millis()-pid.SampleTime;

    return pid;
}
 
 
/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/ 
int PID_compute(PID * pidp)
{
   if(!pidp->inAuto) return 0;
   unsigned long now = millis();
   unsigned long timeChange = (now - pidp->lastTime);
   if(timeChange>=pidp->SampleTime)
   {
      /*Compute all the working error variables*/
    double input = *(pidp->input);
      double error = *(pidp->setpoint) - input;
      pidp->ITerm+= (pidp->ki * error);
      if(pidp->ITerm > pidp->outMax) pidp->ITerm= pidp->outMax;
      else if(pidp->ITerm < pidp->outMin) pidp->ITerm= pidp->outMin;
      double dInput = (input - pidp->lastInput);
 
      /*Compute PID Output*/
      double output = pidp->kp * error + pidp->ITerm - pidp->kd * dInput;
      
    if(output > pidp->outMax) output = pidp->outMax;
      else if(output < pidp->outMin) output = pidp->outMin;
    *(pidp->output) = output;
    
      /*Remember some variables for next time*/
    pidp->lastInput = input;
      pidp->lastTime = now;
    return 1;
   }
   else return 0;
}


/* SetTunings(...)*************************************************************
 * This function allows the controller's dynamic performance to be adjusted. 
 * it's called automatically from the constructor, but tunings can also
 * be adjusted on the fly during normal operation
 ******************************************************************************/ 
void PID_setTunings(PID *pidp, double Kp, double Ki, double Kd)
{
   if (Kp<0 || Ki<0 || Kd<0) return;
 
   pidp->dispKp = Kp; pidp->dispKi = Ki; pidp->dispKd = Kd;
   
   double SampleTimeInSec = ((double)pidp->SampleTime)/1000;
   pidp->kp = Kp;
   pidp->ki = Ki * SampleTimeInSec;
   pidp->kd = Kd / SampleTimeInSec;
 
  if(pidp->controllerDirection == PID_REVERSE)
   {
    pidp->kp = (0 - pidp->kp);
    pidp->ki = (0 - pidp->ki);
    pidp->kd = (0 - pidp->kd);
   }
}
  
/* SetSampleTime(...) *********************************************************
 * sets the period, in Milliseconds, at which the calculation is performed  
 ******************************************************************************/
void SetSampleTime(PID *pidp, int NewSampleTime)
{
   if (NewSampleTime > 0)
   {
      double ratio  = (double)NewSampleTime
                      / (double)pidp->SampleTime;
      pidp->ki *= ratio;
      pidp->kd /= ratio;
      pidp->SampleTime = (unsigned long)NewSampleTime;
   }
}
 
/* SetOutputLimits(...)****************************************************
 *     This function will be used far more often than SetInputLimits.  while
 *  the input to the controller will generally be in the 0-1023 range (which is
 *  the default already,)  the output will be a little different.  maybe they'll
 *  be doing a time window and will need 0-8000 or something.  or maybe they'll
 *  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
 *  here.
 **************************************************************************/
void PID_setOutputLimits(PID *pidp, double Min, double Max)
{
   if(Min >= Max) return;
   pidp->outMin = Min;
   pidp->outMax = Max;
 
   if(pidp->inAuto)
   {
     if(*(pidp->output) > pidp->outMax) *(pidp->output) = pidp->outMax;
     else if(*(pidp->output) < pidp->outMin) *(pidp->output) = pidp->outMin;
   
     if(pidp->ITerm > pidp->outMax) pidp->ITerm = pidp->outMax;
     else if(pidp->ITerm < pidp->outMin) pidp->ITerm = pidp->outMin;
   }
}

/* SetMode(...)****************************************************************
 * Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
 * when the transition from manual to auto occurs, the controller is
 * automatically initialized
 ******************************************************************************/ 
void PID_setMode(PID *pidp, int Mode)
{
    int newAuto = (Mode == PID_AUTOMATIC);
    if(newAuto == !pidp->inAuto)
    {  /*we just went from manual to auto*/
        Initialize(pidp);
    }
    pidp->inAuto = newAuto;
}
 
/* Initialize()****************************************************************
 *  does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/ 
void Initialize(PID *pidp)
{
  pidp->ITerm = *(pidp->output);
  pidp->lastInput = *(pidp->input);
  if(pidp->ITerm > pidp->outMax) pidp->ITerm = pidp->outMax;
  else if(pidp->ITerm < pidp->outMin) pidp->ITerm = pidp->outMin;
}

/* SetControllerDirection(...)*************************************************
 * The PID will either be connected to a DIRECT acting process (+Output leads 
 * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
 * know which one, because otherwise we may increase the output when we should
 * be decreasing.  This is called from the constructor.
 ******************************************************************************/
void PID_setControllerDirection(PID *pidp, int Direction)
{
   if(pidp->inAuto && Direction != pidp->controllerDirection)
   {
     pidp->kp = (0 - pidp->kp);
     pidp->ki = (0 - pidp->ki);
     pidp->kd = (0 - pidp->kd);
   }   
   pidp->controllerDirection = Direction;
}
/*
 Status Funcions*************************************************************
 * Just because you set the Kp=-1 doesn't mean it actually happened.  these
 * functions query the internal state of the PID.  they're here for display 
 * purposes.  this are the functions the PID Front-end uses for example
 *****************************************************************************
double GetKp(){ return  dispKp; }
double GetKi(){ return  dispKi;}
double GetKd(){ return  dispKd;}
int GetMode(){ return  inAuto ? AUTOMATIC : MANUAL;}
int GetDirection(){ return controllerDirection;}*/

