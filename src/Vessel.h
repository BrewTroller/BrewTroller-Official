#ifndef VESSEL_H
#define VESSEL_H

/*
The vessel class stores and manages data and actions pertaining to vessels, such as the HLT, MLT and kettle. It is a concrete class
and should not be subclassed, for performance reasons and because it's important for other parts of the code to be able to interact
with vessels without regard to the role they are playing (HLT vs. MLT vs. kettle) so that we can easily support different vessel configurations.

Also, this class is designed to read all eeprom settings on load. If settings such as which temperature sensor is assigned to this vessel are changed 
after the system is booted, the object should be destroyed and recreated.

NOTE: Some functionality supported in BT2.6 is not yet supported in this version. This includes:
 - PWM
 - Steam
 - Maximum temperature cutoff and alarm
*/

#include "Config.h"

#include <pin.h>
#include <PID_Beta6.h>


class Vesseld
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
	
	//Temperature
	pin heatPin;
	byte setpoint; //The setpoint for this vessel
	bool usePID; //TRUE = use PID mode, FALSE = on/off mode
	PID* pid = NULL; //PID object for use with PID
	byte feedforward = 0; //The ID of the feedforward sensor. Valid values are 1-3, corresponding to AUX1-3. Anything outside that range will be ignored

	float PIDcycle;
	float hysteresis;
	byte includeAux[3]; //Whether to average AUX1-3; stores their indices

	//Volume
	//Using int and ulong here to stay consistent with existing code. These could probably be converted to byte and float.
	int volumeCalibrationPressure[10]; //The pressures used for calibration
	unsigned long volumeCalibrationVolume[10]; //The volumes used for calibration

	//Valves require broader state awareness (e.g. MLT valve config might be different for mash vs. sparge) and are handled outside this class.

	//Working statuses
	float volumeReadings[10]; //To-do: allow user to customize number of volume readings to use in sample
	byte oldestVolumeReading; //Array index of the oldest volume reading, which will get overwritten by the next one
	float temperature;
	float feedforwtardTemperature;
	double PIDoutput; //The current output level of the PID

	//Cached values for performance
	float volume;
	byte usesAuxInputs; //The count of aux inputs used for averaging

	void updateTemperature(); //Fetch the latest temperature from the sensor

public:
	//No default constructor because we need to know which eeprom index to use
	Vessel(byte initEepromIndex, byte initIncludeAux[], byte FFBias, float initMinVolume, byte initMinTriggerPin, byte initMaxPower);
	~Vessel() ;

	//Temperature control functions
	void setSetpoint(byte );
	byte getSetpoint();
	float getTemperature();
	void setMaxPower(byte );

	bool updateOutput(); //Turn output on or off based on temperature, returning whether the output is on

	void updateVolumeCalibration(byte , unsigned long , int );

	float getVolume(); //Return the volume, as calculated based on this 
	void takeVolumeReading(); //Take a sample of the volume
};
#endif