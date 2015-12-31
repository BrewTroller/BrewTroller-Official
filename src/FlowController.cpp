#include "FlowController.h"
#include "BrewTroller.h"
#include <PID_Beta6.h>

extern FlowController* flowController[2];

void FlowController::initPID()
{
	pid = new PID(&pidInput, &pidOutput, &pidSetpoint, 3, 4, 1);
#ifdef USEMETRIC
	pid->SetInputLimits(0, 255000); // equivalent of 25.5 LPM (255 * 100)
#else
	pid->SetInputLimits(0, 6375); // equivalent of 6.375 GPM (255 * 25)
#endif

	pid->SetOutputLimits(PID_FLOW_MIN, pid->GetSampleTime() * PIDLIMIT_PUMP);

#ifdef PID_CONTROL_MANUAL
	pid->SetMode(PID::MANUAL_MODE);
#else
	pid->SetMode(PID::AUTO_MODE);
#endif
	pid->SetSampleTime(FLOWRATE_READ_INTERVAL);
#ifdef PID_CONTROL_MANUAL
	nextcompute = millis() + FLOWRATE_READ_INTERVAL;
#endif
}
FlowController::FlowController(Vessel* initSource, Vessel* initDestination, pin* controlPin, bool initUsePID, bool initusePWM, bool initPidValves)
{
	source = initSource;
	destination = initDestination;
	flowControlPin = controlPin;
	usesPWM = initusePWM;
	pidValves = initPidValves;
	hysteresis = 250;
	if (initUsePID)
		initPID();
}

FlowController::~FlowController() {
	if (pid) delete pid;
}

void FlowController::usePID(bool newPID)
{
	if (newPID && !pid)
		initPID();
	else if (!newPID && pid)
		delete pid;
	update();
}
void FlowController::setTargetFlowRate(double newRate)
{
	targetFlowRate = newRate;
	pidSetpoint = newRate;
	update();
}

void FlowController::matchFlow(FlowController* partnerFlow) //Matches this controller's flow rate to the flow of the partner; call with NULL to turn off
{
	partner = partnerFlow;
	if (partner)
	{
		if (partner->isPID())
			pidSetpoint = partner->getTargetFlowRate();
		else
			pidSetpoint = partner->getFlowRate();
	}
	else
		pidSetpoint = targetFlowRate;
	update();
}

double FlowController::getVolumeChange()
{
	double volChange = 0;
	if (source)
		volChange += abs(source->getVolume() - lastSourceVolume);
	if (destination)
		volChange += abs(destination->getVolume() - lastDestVolume);
	
	//A disconnected volume sensor reads as exactly 0. We should not divide by 2 in that case. A real sensor in an empty vessel never reads as exactly 0.
	//However it doesn't really matter because an exact sensor reading of 0 will simply be ignored in favor of the other vessel's sensor.
	if (source && destination && source->getVolume() != 0 && lastSourceVolume != 0 && destination->getVolume() != 0 && lastDestVolume != 0) 
		volChange /= 2; //If we have two volume measurements, average them
	return volChange;
}

void FlowController::update()
{
  //TODO: Actually calculate flow rates! 
#ifdef HLT_FLY_SPARGE_STOP
	const bool minVol = HLT_FLY_SPARGE_STOP_VOLUME;
#else
	const bool minVol = source->getDeadspace();
#endif
	if (estop || sSwitch == SOFTSWITCH_OFF || (source->getVolume() <= minVol && sSwitch != SOFTSWITCH_ON)) //Don't dry run the pump, unless the soft switch forces it
	{
		pidOutput = 0;
		if (flowControlPin) flowControlPin->set(LOW);
		if (destination) destination->stopFilling();
		return;
	}

	if (sSwitch == SOFTSWITCH_ON)
	{
		pidOutput = 100;
		if (flowControlPin) flowControlPin->set(HIGH);
		updateValves();

		return;
	}

	if (isPID())
	{
		//Note that for auto fly sparging, this is managed by setting identical setpoints for both controllers

		//If this controller is PID and its partner isn't, match the actual flow rate.
		//Note that this will be kinda wonky if the partner's flow rate isn't constant, either due to variations in the system or due to
		//sensor noise. It'll work but the PID will be forever chasing a moving target.
		if (partner && !partner->isPID())
			pidSetpoint = partner->getFlowRate();

#ifdef PID_CONTROL_MANUAL

		millistemp = millis();
		if (millistemp >= nextcompute) {
			nextcompute += FLOWRATE_READ_INTERVAL;
			if (pid->GetSetpoint() == 0) pidOutput = 0;
			else {
				if ((long)pid->GetSetpoint() - getFlowRate() > 100) {
					additioncount[0]++;
					additioncount[1] = 0;
					if (additioncount[0] > 5) {    // this is here to break a case where adding 10 causes a change of 100 but lowering 10 causes a change of 100 off the setpoint and we just oscilate. 
						additioncount[0] = 0;
						pidOutput += 5;
					}
					else pidOutput += 10;
				}
				else if ((long)pid->GetSetpoint() - flowRate[VS_KETTLE] < -100) {
					additioncount[0]++;
					additioncount[1] = 0;
					if (additioncount[0] > 5) {    // this is here to break a case where adding 10 causes a change of 100 but lowering 10 causes a change of 100 off the setpoint and we just oscilate. 
						additioncount[0] = 0;
						pidOutput -= 5;
					}
					else pidOutput -= 10;
				}
				else if ((long)pid->GetSetpoint() - flowRate[VS_KETTLE] > 50) {
					additioncount[0] = 0;
					additioncount[1]++;
					if (additioncount[0] > 5) {    // this is here to break a case where adding 5 causes a change of 50 but lowering 5 causes a change of 50 off the setpoint and we just oscilate. 
						additioncount[1] = 0;
						pidOutput += 1;
					}
					else pidOutput += 5;
				}
				else if ((long)pid->GetSetpoint() - flowRate[VS_KETTLE] < -50) {
					additioncount[0] = 0;
					additioncount[1]++;
					if (additioncount[0] > 5) {    // this is here to break a case where adding 5 causes a change of 50 but lowering 5 causes a change of 50 off the setpoint and we just oscilate. 
						additioncount[1] = 0;
						pidOutput -= 1;
					}
					else pidOutput -= 5;
				}
				else if ((long)pid->GetSetpoint() - flowRate[VS_KETTLE] > 10) pidOutput += 1;
				else if ((long)pid->GetSetpoint() - flowRate[VS_KETTLE] < -10) pidOutput -= 1;

				if (pidOutput > pid->GetOUTMax()) pidOutput = pid->GetOUTMax();
				else if (pidOutput < pid->GetOUTMin()) pidOutput = pid->GetOUTMin();
			}
		}
#else

		//Note that this uses pointers to the temperature, output and feedforward variables
		pid->Compute(); 

#ifdef USEPWM
		//If using PWM, the output is updated by a separate timer-driven ISR, so we should exit here.
		//It's important that we call pid->Compute() above, though, because it's not recalculated in the ISR.
		if (usesPWM && isPID())
		{
			updateValves();
			return;
		}
#endif

		//only 1 call to millis needed here, and if we get hit with an interrupt we still want to calculate based on the first read value of it
		unsigned long timestamp = millis();
		byte cycleIndex = LAST_HEAT_OUTPUT+(this==flowController[1]); //The PID pumps are stored after the heat outputs in the cycle timing array. Since there are only two, this code checks to see if this is the second one and adds one if so.
		if (timestamp - cycleStart[cycleIndex] > (unsigned int)pid->GetSampleTime() * 100) cycleStart[cycleIndex] += pid->GetSampleTime() * 100;

		//cycleStart is the millisecond value when the current PID cycle started.
		if (pidOutput >= timestamp - cycleStart[cycleIndex] && timestamp != cycleStart[cycleIndex])
		{
			flowControlPin->set(HIGH);
		}
		else
		{
			flowControlPin->set(LOW);
		}
#endif
		if (pidValves) updateValves();

	}
	else
	{
		if (partner)
		{
			if (!flowControlPin->get() && partner->getVolumeChange() > hysteresis)
			{
				flowControlPin->set(HIGH);
				pidOutput = 100;
				if (source) lastSourceVolume = source->getVolume(); else lastSourceVolume = 0;
				if (destination) lastDestVolume = destination->getVolume(); else lastDestVolume = 0;
			}
			else if (getVolumeChange() > hysteresis && partner->getVolumeChange() < hysteresis)
			{
				flowControlPin->set(LOW);
				pidOutput = 0;
				if (source) lastSourceVolume = source->getVolume(); else lastSourceVolume = 0;
				if (destination) lastDestVolume = destination->getVolume(); else lastDestVolume = 0;
			}
		}
		else
		{
			//No partner and no pid so just fill until full/empty
			if (!isAtTarget())
			{
				//Turn output on
				flowControlPin->set(HIGH);
				pidOutput = 100;
			}
			else 
			{
				//Turn output off
				flowControlPin->set(LOW);
				pidOutput = 0;
			}
		}
	}

	//If we aren't PID-controlling the valves, control them here. Note that in a wierd case where usePID = true, pidValves = false, and the
	//output rate is very low (e.g. 2%), the natural fluctuation around that low output could cause the valves to toggle often. This is such a wierd case that
	//I doubt anyone will ever encounter it. Which means someday someone will read this comment and find it to be the source of the bug they're investigating. :)
	if (!pidValves) //If pidValves == TRUE the valves are updated in the pid loop
		updateValves();
}

#ifdef USEPWM
//IMPORTANT: this function is called from an ISR, and therefore needs to be very fast
void FlowController::updatePWMOutput(bool newOutput)
{
	if (usesPWM && isPID())
		flowControlPin.set(newOutput);
}

#endif

void FlowController::updateValves()
{
	if (!destination) return; //Should never happen, but check just in case...
	if (pidOutput)
		destination->fill();
	else
		destination->stopFilling();
}

bool FlowController::isAtTarget()
{
	return (source && source->getVolume() < source->getTargetVolume()) || 
	  (destination && destination->getVolume() > destination->getTargetVolume());
}
void FlowController::setPID(double cycle, double p, double i, double d)
{
	pid->SetSampleTime(cycle);
	pid->SetTunings(p, i, d);
}

void initFlowControllers()
{
  #ifdef PID_PUMP1
		const bool pump1_PID = true;
#else
		const bool pump1_PID = false;
#endif
#ifdef PID_PUMP2
		const bool pump2_PID = true;
#else
		const bool pump2_PID = false;
#endif
#ifdef USEPWM_PUMP1
		const bool pump1_PWM = true;
#else
		const bool pump1_PWM = false;
#endif
#ifdef USEPWM_PUMP2
		const bool pump2_PWM = true;
#else
		const bool pump2_PWM = false;
#endif
		
	//TODO: Make pidValves configurable (probably should be done in UI and EEPROM...)
	if (vessels[VS_HLT] != vessels[VS_MASH])
	{
		//Note that in some cases these are the same vessel. This is fine - the FlowController class can handle it.
		flowController[0] = new FlowController(vessels[VS_HLT], vessels[VS_MASH], Valves.getPin(0), pump1_PID, pump1_PWM, false); 
			
		if (vessels[VS_MASH] != vessels[VS_KETTLE])
			flowController[1] = new FlowController(vessels[VS_MASH], vessels[VS_KETTLE], Valves.getPin(1), pump2_PID, pump2_PWM, false);
		else
			flowController[1] = flowController[0];
	}
	else
		if (vessels[VS_MASH] != vessels[VS_KETTLE])
			//If we only have mash and kettle, set up flow controller in slot 0 on pin0
			flowController[0] = new FlowController(vessels[VS_MASH], vessels[VS_KETTLE], Valves.getPin(0), pump1_PID, pump1_PWM, false);

	fillController[0] = new FlowController(NULL, vessels[VS_HLT], NULL, false, false, false);
	fillController[1] = new FlowController(NULL, vessels[VS_MASH], NULL, false, false, false);
}