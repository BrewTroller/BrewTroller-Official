#ifndef VESSEL_H
#define VESSEL_H

/*
The vessel class stores and manages data and actions pertaining to vessels, such as the HLT, MLT and kettle. It is a concrete class
and should not be subclassed, for performance reasons and because it's important for other parts of the code to be able to interact
with vessels without regard to the role they are playing (HLT vs. MLT vs. kettle) so that we can easily support different vessel configurations.

This class is designed to read all eeprom settings on load and generally hnadle its own eeprom reading.

NOTE: Some functionality supported in BT2.6 is not yet supported in this version. This includes:
 - PWM
 - Steam
 - Maximum temperature cutoff and alarm
*/

#include "Config.h"
#include "Enum.h"
#include <pin.h>
#include <PID_Beta6.h>


class Vessel
{
private:
	//Config
	byte eepromIndex; //The index in the eeprom to look up settings for this vessel
	byte sensorAddress[8]; //The address of the temperature sensor assigned to this vessel. To-do: replace with class
	byte feedforwardAddress[8]; //The address of the feedforward sensor if one is used.

	float minVolume; //Minimum volume at which to run the element
	byte maxPower; //Maximum power at which to run the associated element
	byte volumePinID; //The pin supplying volume readings for this vessel
	float capacity; //Maximum volume
	float deadspace; //Dead space
	byte minTriggerPin; //Pin that triggers a low volume condition
	
	SoftSwitch heatOverride = SOFTSWITCH_AUTO; // SOFTSWITCH_OFF, _ON, _AUTO 

	//Temperature
	//Note that we use doubles here for type compatibility with the PID library. On most Arduino systems, double and float use the same precision (and on the systems where they don't there is plenty of memory).

	pin heatPin;
	double setpoint = 0; //The setpoint for this vessel
	bool usePID; //TRUE = use PID mode, FALSE = on/off mode
	PID* pid = NULL; //PID object for use with PID
	byte feedforward = 0; //The ID of the feedforward sensor. Valid values are 1-3, corresponding to AUX1-3. Anything outside that range will be ignored

	float PIDcycle;
	float hysteresis;
	byte includeAux[3]; //Whether to average AUX1-3; stores their indices

	//Volume
	//Using int and ulong here to stay consistent with existing code. These could probably be converted to byte and float.
	unsigned int volumeCalibrationPressure[10]; //The pressures used for calibration
	unsigned long volumeCalibrationVolume[10]; //The volumes used for calibration
	float targetVolume = 0;
	


	//Valves require broader state awareness (e.g. MLT valve config might be different for mash vs. sparge) and are handled outside this class.

	//Working statuses
	float volumeReadings[VOLUME_READ_COUNT]; //To-do: allow user to customize number of volume readings to use in sample
	byte oldestVolumeReading; //Array index of the oldest volume reading, which will get overwritten by the next one

	double temperature = 0;
	double feedforwardTemperature = 0;

	double PIDoutput = 0; //The current output level of the PID

	//Cached values for performance
	float volume = 0;
	byte usesAuxInputs; //The count of aux inputs used for averaging

	void updateTemperature(); //Fetch the latest temperature from the sensor

public:
	//All set functions also write the value to EEPROM

	//No default constructor because we need to know which eeprom index to use
	Vessel(byte initEepromIndex, byte initIncludeAux[], byte FFBias, float initMinVolume, byte initMinTriggerPin, byte initMaxPower);
	~Vessel();

	//Temperature control functions
	void setSetpoint(double); //Sets the setpoint and updates outputs accordingly
	double getSetpoint(); //Returns the current setpoint
	double getTemperature(); //Returns the current temperature reading
	byte getOutput(); //Returns the current output level
	byte getPercentOutput(); //Returns the output scaled 0-100
	void setMaxPower(byte);
	inline byte getMaxPower() {
		return maxPower;
	};
	inline float getPIDCycle() { return PIDcycle; }
	void setPIDCycle(float);
	inline float getHysteresis() { return hysteresis; }
	void setHysteresis(float);
	inline float getP() { if (pid) return pid->GetP_Param(); else return 3; } //3,4,1 are default values for PID control. Arguably we should return an error code and let the caller figure it out.
	inline float getI() { if (pid) return pid->GetI_Param(); else return 4; }
	inline float getD() { if (pid) return pid->GetD_Param(); else return 1; }
	void setTunings(double p, double i, double d); //Also writes tunings to eeprom
	void setTSAddress(byte*); //Also writes to EEPROM

	inline bool isPID() { return usePID; }
	void setPID(bool);

	bool updateOutput(void); //Turn output on or off based on temperature, returning whether the output is on
	void manualOutput(int); //Manual output control, sets the output to a fixed PID value

	//Volume functions
	void updateVolumeCalibration(byte, unsigned long, int); //Update a volume calibration value, including writing to eeprom

	float getVolume(); //Return the volume, as calculated based on this 
	void takeVolumeReading(); //Take a sample of the volume
	inline void setTargetVolume(float target) { targetVolume = target * 1000.0; };
	inline float getTargetVolume() { return targetVolume / 1000.0; };
	inline float getDeadspace() { return deadspace; }
	void setDeadspace(float);
	
	inline float getCapacity() { return capacity; }
	void setCapacity(float capacity);

	inline unsigned long getCalibrationVolume(byte index) { return volumeCalibrationVolume[index]; }
	inline unsigned int getCalibrationPressure(byte index) { return volumeCalibrationVolume[index]; }
	
	bool isOn(); //Returns whether the heating element is on at this very moment (cycles on and off with PID). Use getOutput() to see the exact level.
	void setHeatOverride(byte); //Forces the element on or off, or sets it to auto. Used with RGBIO8 soft switches.
	byte getHeatOverride(); 
};

//Subroutine to initialize the system's vessels
void initVessels();
#endif