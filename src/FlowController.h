#ifndef FLOWCONTROLLER_H
#define FLOWCONTROLLER_H

#include "Enum.h"
#include "Vessel.h"

constexpr unsigned long FLOWRATE_READ_INTERVAL = 1000;

class FlowController
{
private:
	Vessel* source;
	Vessel* destination;
	double flowRate, targetFlowRate;
	double lastSourceVolume, lastDestVolume;

	FlowController* partner = NULL; //Another flow controller with which to try to equalize rates

	pin* flowControlPin;
	bool usesPWM;
	
	bool pidValves; //Should we apply the PID pulsing to the valves, or should they be on/off?
	byte valveConfig; //The valve config to apply for transfers
	
	PID* pid = NULL;
	double pidInput, pidOutput, pidSetpoint;
	double hysteresis; //used for non-PID mode
	
	SoftSwitch sSwitch = SOFTSWITCH_AUTO;
	

#ifdef PID_CONTROL_MANUAL
	unsigned long nextcompute = 0;
	byte additioncount[2] = { 0,0 };
#endif
	
	void initPID();
	void updateValves();

public:
	FlowController(Vessel* initSource, Vessel* initDestination, pin* controlPin, bool initUsePID, bool initUsesPWM, bool initPidValves);
	~FlowController();

	void matchFlow(FlowController*); //Matches this controller's flow rate to the flow of the partner;
	void update();
	void updatePWMOutput(bool);
	bool isAtTarget(); //Returns whether flow is currently active/needed

	inline bool isPID() { return (pid == NULL); }
	void usePID(bool);
	inline bool usePWM() { return usesPWM; }
	inline double getTargetFlowRate() { return targetFlowRate; }
	inline double getFlowRate() { return flowRate;}
	void setTargetFlowRate(double newRate);
	void setPID(double cycle, double p, double i, double d);
	
	inline void setCycle(double cycle) { pid->SetSampleTime(cycle); }
	inline double getCycle() { return pid->GetSampleTime(); }
	inline double getHysteresis() { return hysteresis; }
	inline void setHysteresis(double newHysteresis) {  hysteresis = newHysteresis; }
	inline double getP() { return pid->GetP_Param(); }
	inline double getI() { return pid->GetI_Param(); }
	inline double getD() { return pid->GetD_Param(); }
	inline double getOutput() { return pidOutput; }
	double getVolumeChange();
	inline SoftSwitch getSwitch() { return sSwitch; }
	inline void setSwitch(SoftSwitch s) { sSwitch = s; update(); }

	inline void startAuto() { sSwitch = SOFTSWITCH_AUTO; update(); }
	inline void startOn() { sSwitch = SOFTSWITCH_ON; update(); }
	inline void stop() { sSwitch = SOFTSWITCH_OFF; update(); }

	//inline byte getMode() { return pid->GetMode(); }
	//inline void setMode(int newMode) { pid->SetMode(newMode); }
};

void initFlowControllers();
#endif

