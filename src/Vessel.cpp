/*
Copyright (C) 2015 David Seidman

This file is part of BrewTroller.

BrewTroller is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BrewTroller is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BrewTroller.  If not, see <http://www.gnu.org/licenses/>.


BrewTroller - Open Source Brewing Computer

Documentation, Forums and more information available at http://www.brewtroller.com
*/


/*
The vessel class stores and manages data and actions pertaining to vessels, such as the HLT, MLT and kettle. It is a concrete class
and should not be subclassed, for performance reasons and because it's important for other parts of the code to be able to interact
with vessels without regard to the role they are playing (HLT vs. MLT vs. kettle) so that we can easily support different vessel configurations.
*/

#include <EEPROM.h>

#include "Vessel.h"
#include "EEPROM.h"	
#include "Temp.h"
#include "HardwareProfile.h"
#include "Volume.h"
#include "Outputs.h" //Needed only for minTriggerPin

extern unsigned int cycleStart[4];
extern bool estop;
extern int temp[9];


// set what the PID cycle time should be based on how fast the temp sensors will respond
#if TS_ONEWIRE_RES == 12
#define PID_CYCLE_TIME 750
#elif TS_ONEWIRE_RES == 11
#define PID_CYCLE_TIME 375
#elif TS_ONEWIRE_RES == 10
#define PID_CYCLE_TIME 188
#elif TS_ONEWIRE_RES == 9
#define PID_CYCLE_TIME 94
#else
// should not be this value, fail the compile
#ERROR
#endif
//Note that the includeAux numbers should be addresses e.g. TS_AUX1, not just a number (that's why they're bytes instead of booleans).
	Vessel::Vessel(byte initEepromIndex, byte initIncludeAux[], byte FFBias, float initMinVolume = 0, byte initMinTriggerPin = 0, byte initMaxPower = 100)
	{

		for (int i = 0; i < 10; i++)
			volumeReadings[i] = 0;

		eepromIndex = initEepromIndex;
		minVolume = initMinVolume;
		maxPower = initMaxPower;
		minTriggerPin = initMinTriggerPin;
		includeAux[0] = initIncludeAux[0];
		includeAux[1] = initIncludeAux[1];
		includeAux[2] = initIncludeAux[2];
		includeAux[3] = initIncludeAux[3];

		usesAuxInputs = includeAux[0] + includeAux[1] + includeAux[2];

		//Load settings from eeprom

		//Load temperature sensor information - needs to be before PID setup so we have the temp probe addresses in place
		//**********************************************************************************
		//Temperature Sensors: HLT (0-7), MASH (8-15), KETTLE (16-23), H2OIN (24-31), H2OOUT (32-39),
		//          BEEROUT (40-47), AUX1 (48-55), AUX2 (56-63), AUX3 (64-71)
		//**********************************************************************************
		EEPROMreadBytes(8 * eepromIndex, sensorAddress, 8);
		if (FFBias)
		{
			feedforward = FFBias;
			EEPROMreadBytes(8 * eepromIndex, feedforwardAddress, 8);
		}

		usePID = getPIDEnabled(eepromIndex);
		if (usePID)
		{ 
			float PID_P, PID_I, PID_D; //P, I and D values for PID

			PIDcycle = getPIDCycle(eepromIndex);
			PID_P = getPIDp(eepromIndex);
			PID_I = getPIDi(eepromIndex);
			PID_D = getPIDd(eepromIndex);

			//Feedforward version included but commented out for later reference
			if (feedforward)
				pid = new PID(&temperature, &PIDoutput, &setpoint, &feedforwardTemperature, PID_P, PID_I, PID_D);
			else
				pid = new PID(&temperature, &PIDoutput, &setpoint, PID_P, PID_I, PID_D);

			pid->SetInputLimits(0, 25500);
			pid->SetOutputLimits(0, PIDcycle * maxPower);
			pid->SetMode(PID::AUTO_MODE);
			pid->SetSampleTime(PID_CYCLE_TIME);
		}
		hysteresis = getHysteresis(eepromIndex); 
		capacity = getCapacity(eepromIndex);
		deadspace = getVolLoss(eepromIndex);

		//Load volume calibration ettings
		//**********************************************************************************
		//calibVols HLT (119-158), Mash (159-198), Kettle (199-238)
		//calibVals HLT (239-258), Mash (259-278), Kettle (279-298)
		//**********************************************************************************
		eeprom_read_block(&volumeCalibrationVolume, (unsigned char *)119+40*eepromIndex, 40);
		eeprom_read_block(&volumeCalibrationPressure, (unsigned char *)239+40*eepromIndex, 20);


		//This is kind of a hack because the volume and heat pins are hardcoded
		switch (eepromIndex)
		{
		case 0:
			volumePinID = HLTVOL_APIN;
			heatPin.setup(HLTHEAT_PIN, OUTPUT);
			break;
#ifdef MLTVOL_APIN
		case 1:
			volumePinID = MLTVOL_APIN;
			heatPin.setup(MLTHEAT_PIN, OUTPUT);
			break;
#endif
#ifdef KETTLEVOL_APIN
		case 2:
			volumePinID = KETTLEVOL_APIN;
			heatPin.setup(KETTLEVOL_PIN, OUTPUT);
			break;
#endif
		}

	}

	Vessel::~Vessel() { if (pid) free(pid );}
	
	//Temperature control functions
	void Vessel::setSetpoint(byte newSetPoint)
	{
		setpoint = newSetPoint;
	}

	byte Vessel::getSetpoint()
	{
		return setpoint;
	}

	float Vessel::getTemperature()
	{
		return temperature;
	}

	void Vessel::setMaxPower(byte newMaxPower)
	{
		maxPower = newMaxPower;
	}

	void Vessel::updateTemperature()
	{
		//Fetch the latest temperature from the sensor, which returns 100ths of a degree
		temperature = 100.0 * read_temp(sensorAddress);
		
		if (usesAuxInputs)
		{
			byte tempTemp;//Get it? Temporary temperature
			for (byte i = 0; i < 3; i++)
			{
				if (includeAux[i])
				{
					tempTemp = 100.0 * temp[includeAux[i]];
					if (temperature == BAD_TEMP) //Note that in the case where we have a bad read, and then get another bad value, this maintains temperature == BAD_TEMP without needing to special-case it
						temperature = tempTemp;
					else
						temperature += tempTemp;
				}
			}
			if (temperature != BAD_TEMP)
				temperature /= usesAuxInputs;
		}
		if (feedforward)
		{
			feedforwardTemperature = 100.0 * read_temp(feedforwardAddress);
			if (feedforwardTemperature == BAD_TEMP)
				feedforwardTemperature = temperature; //If we got a bad read on the feedforward, but have a good read on the mash itself, we can just use the mash temp as the feedforward to let us keep mashing
	}

	//Turn output on or off based on temperature, returning whether the output is on
	bool Vessel::updateOutput()
	{
		//Update temperature for use in all later calculations
		updateTemperature();


		if (estop || getVolume() < minVolume || temperature == BAD_TEMP  ||
			(minTriggerPin && !vesselMinTrigger(minTriggerPin)->get()))

			//The latter condition checks for the digital trigger input for low volume 
		{
			//Turn output off due to error condition
			heatPin.set(LOW);
			return false;
		}

		if (usePID)
		{
			//Note that this uses pointers to 
			pid->Compute();
			//only 1 call to millis needed here, and if we get hit with an interrupt we still want to calculate based on the first read value of it
			unsigned long timestamp = millis();

			//cycleStart is the millisecond value when the current PID cycle started.
			//We compare the 
			if (PIDoutput >= timestamp - cycleStart[eepromIndex] && timestamp != cycleStart[eepromIndex])
			{
				heatPin.set(HIGH);
				return true;
			}
			else
			{
				heatPin.set(LOW);
				return false;
			}
		}
		else
		{
			if (getTemperature() < getSetpoint()-hysteresis)
			{
				//Turn output on
				heatPin.set(HIGH);
				return true;
			}
			else if (getTemperature() > getSetpoint()+hysteresis)
			{
				//Turn output off
				heatPin.set(LOW);
				return false;
			}
		}
	}
	

	void Vessel::updateVolumeCalibration(byte index, unsigned long vol, int pressure)
	{
		volumeCalibrationVolume[index] = vol;
		volumeCalibrationPressure[index] = pressure;

		//Write to eeprom
		EEPROMwriteLong(119 + eepromIndex * 40 + index * 4, vol);
		EEPROMwriteInt(239 + eepromIndex * 20 + index * 2, pressure);
	}

	float Vessel::getVolume() 
	{
		return volume;
	} 
	
	//Take a sample of the volume
	void Vessel::takeVolumeReading()
	{
		byte reading;
		//Take volume reading
		reading = readVolume(volumePinID, volumeCalibrationVolume, volumeCalibrationPressure);

		volume = volume + (reading - volumeReadings[oldestVolumeReading]) / 10;
		volumeReadings[oldestVolumeReading] = reading;
		oldestVolumeReading = (oldestVolumeReading + 1) % 10; //This could be made faster by switching to a power of 2 and using a bitmask
	}



