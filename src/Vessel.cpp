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

#include <avr/eeprom.h>
#include "BrewTroller.h"
#include "Vessel.h"
#include "EEPROM.h"	
#include "Temp.h"
#include "HardwareProfile.h"
#include "Volume.h"
#include "Outputs.h" //Needed only for minTriggerPin
#include "BrewCore.h" //Needed for heartbeat
#include "Timer.h" //Needed for setAlarm
#include "PVOut.h"

extern int temp[9]; //Needed to access aux sensors
extern unsigned long vlvConfig[NUM_VLVCFGS], actProfiles;

//Note that the includeAux numbers should be addresses e.g. TS_AUX1, not just a number (that's why they're bytes instead of booleans).
	Vessel::Vessel(byte initrole, bool initIncludeAux[], byte FFBias, float initMinVolume = 0, byte initMinTriggerPin = 0, byte initMaxPower = 100, double initMinTemperature = 30, double initMaxTemperature = 250, bool initUsePWM = false)
	{
		minTemperature = initMinTemperature;
		maxTemperature = initMaxTemperature;
		role = initrole;
		minVolume = initMinVolume;
		maxPower = initMaxPower;
		minTriggerPin = initMinTriggerPin;
		includeAux[0] = initIncludeAux[0];
		includeAux[1] = initIncludeAux[1];
		includeAux[2] = initIncludeAux[2];

		usesAuxInputs = includeAux[0] + includeAux[1] + includeAux[2];

		//Load settings from eeprom
		//TODO: use EEPROM encapsulation instead
		//Load temperature sensor information - needs to be before PID setup so we have the temp probe addresses in place
		//**********************************************************************************
		//Temperature Sensors: HLT (0-7), MASH (8-15), KETTLE (16-23), H2OIN (24-31), H2OOUT (32-39),
		//          BEEROUT (40-47), AUX1 (48-55), AUX2 (56-63), AUX3 (64-71)
		//**********************************************************************************
		EEPROMreadBytes(8 * role, sensorAddress, 8);
		setpoint = EEPROM.read(299 + role) * SETPOINT_MULT;
		if (FFBias)
		{
			feedforward = FFBias;
			EEPROMreadBytes(8 * role, feedforwardAddress, 8);
		}

		usePID = getPIDEnabled(role);
		//Always load PID values even if the setting is not currently set to PID, because otherwise they won't be available for the config UI
		float PID_P, PID_I, PID_D; //P, I and D values for PID

		PIDcycle = ::getPIDCycle(role);
		PID_P = ::getPIDp(role);
		PID_I = ::getPIDi(role);
		PID_D = ::getPIDd(role);

		//Feedforward version included but commented out for later reference
		if (feedforward)
			pid = new PID(&temperature, &PIDoutput, &setpoint, &feedforwardTemperature, PID_P, PID_I, PID_D);
		else
			pid = new PID(&temperature, &PIDoutput, &setpoint, PID_P, PID_I, PID_D);

		//If the PID load failed, we are out of memory. While we could try to push through, this could be unsafe, so bail out.
		if (!pid)
			exit(1);

		pid->SetInputLimits(0, 25500);
		pid->SetOutputLimits(0, PIDcycle * maxPower);
		pid->SetMode(PID::AUTO_MODE);
		pid->SetSampleTime(PID_CYCLE_TIME);
		
#ifdef USESTEAM
		//This references the pid object and thus must come after it is created in this function
		usesSteam = (role == VS_MASH); //The mash tun uses steam if it's present
		if (role == VS_STEAM)
		{
		  steamPressureSensitivity = EEPROMreadInt(117);
		  
		#ifdef USEMETRIC
		  pid->SetInputLimits(0, 50000 / steamPressureSensitivity);
		#else
		  pid->SetInputLimits(0, 7250 / steamPressureSensitivity);
		#endif
		}
#endif
		
#ifdef USEPWM
		usesPWM = initUsePWM;
#endif
		hysteresis = ::getHysteresis(role); 
		capacity = EEPROMreadLong(93 + role * 4);
		deadspace = EEPROMreadInt(105 + role * 2);

		//Load volume calibration ettings
		//**********************************************************************************
		//calibVols HLT (119-158), Mash (159-198), Kettle (199-238)
		//calibVals HLT (239-258), Mash (259-278), Kettle (279-298)
		//**********************************************************************************
		if (role != VS_STEAM)
		{
		  eeprom_read_block(&volumeCalibrationVolume, (unsigned char *)119+40*role, 40);
		  eeprom_read_block(&volumeCalibrationPressure, (unsigned char *)239+40*role, 20);
		}
		else
		  volumeCalibrationPressure[0] = EEPROMreadInt(114);  

		//This is kind of a hack because the volume and heat pins are hardcoded
		switch (role)
		{
		case VS_HLT:
			volumePinID = HLTVOL_APIN;
			heatPin.setup(HLTHEAT_PIN, OUTPUT);
#ifdef USEPWM_HLT
			usesPWM = true;
#endif
			break;
#ifdef MLTVOL_APIN
		case VS_MASH:
			volumePinID = MLTVOL_APIN;
			heatPin.setup(MLTHEAT_PIN, OUTPUT);
#ifdef USEPWM_MASH
			usesPWM = true;
#endif
			break;
#endif
#ifdef KETTLEVOL_APIN
		case VS_KETTLE:
			volumePinID = KETTLEVOL_APIN;
			heatPin.setup(KETTLEHEAT_PIN, OUTPUT);
#ifdef USEPWM_KETTLE
			usesPWM = true;
#endif
			break;
#endif
#ifdef USESTEAM
		case VS_STEAM:
			volumePinID = STEAMPRESS_APIN;
			heatPin.setup(STEAMHEAT_PIN, OUTPUT);
#ifdef USEPWM_STEAM
			usesPWM = true;
#endif
			break;
#endif
		}

	}

	Vessel::~Vessel() { if (pid) delete pid;}
	
	//Temperature control functions
	void Vessel::setSetpoint(double newSetPoint)
	{
		if (newSetPoint * SETPOINT_MULT > setpoint)
			preheated = false;
		setpoint = newSetPoint * SETPOINT_MULT;
		EEPROM.write(299 + role, newSetPoint);
		if (setpoint == 0)
		{
			feedforwardTemperature = 0;
			PIDoutput = 0;
		}
		updateOutput();
	}

	double Vessel::getSetpoint()
	{
		return setpoint / SETPOINT_MULT;
	}

	double Vessel::getTemperature()
	{
		return temperature;
	}

	void Vessel::setMaxPower(byte newMaxPower)
	{
		maxPower = newMaxPower;
		pid->SetOutputLimits(0, PIDcycle * maxPower);
	}

	void Vessel::setTunings(double p, double i, double d)
	{
		//These EEPROM.cpp functions write the PID value to EEPROM and do nothing else
		EEPROM.write(73 + role * 5, p);
		EEPROM.write(74 + role * 5, i);
		EEPROM.write(75 + role * 5, d);
		if (!pid) return; //This check is redundant - the PID loading code bails out if the PID isn't loaded - until someone changes it so that it doesn't :).
		pid->SetTunings(p, i, d);
	}

	void Vessel::updateTemperature()
	{
		//Fetch the latest temperature from the sensor, which returns 100ths of a degree
		temperature = read_temp(sensorAddress) / 100.0;
		
		if (usesAuxInputs)
		{
			byte tempTemp;//Get it? Temporary temperature
			for (byte i = 0; i < 3; i++)
			{
				if (includeAux[i])
				{
					tempTemp = temp[TS_AUX1+i] / 100.0;
					if (temperature == BAD_TEMP) //Note that in the case where we have a bad read, and then get another bad value, this maintains temperature == BAD_TEMP without needing to special-case it
						temperature = tempTemp;
					else
						temperature += tempTemp;
				}
			}
			if (temperature != BAD_TEMP)
				temperature /= usesAuxInputs;
		}
		
		
		//If we are suffering a temperature excursion or malfunction, set the alarm, cut off the setpoint, and turn off the outputs
		if (temperature == BAD_TEMP || temperature < minTemperature or temperature > maxTemperature)
		{
		  setAlarm(1);
		  setSetpoint(0);
		  return;
		}
		
		if (feedforward)
		{
			feedforwardTemperature = read_temp(feedforwardAddress) / 100.0;
			if (feedforwardTemperature == BAD_TEMP)
				feedforwardTemperature = temperature; //If we got a bad read on the feedforward, but have a good read on the mash itself, we can just use the mash temp as the feedforward to let us keep mashing
	
		}
		
		if (temperature > setpoint)
			preheated = true;
		
	}

	void Vessel::setTSAddress(byte newAddress[8]) 
	{
		memcpy(sensorAddress, newAddress, 8 * sizeof(byte));
		EEPROMwriteBytes(role * 8, sensorAddress, 8); 
	}
	
#ifdef USEPWM
	//IMPORTANT: this function is called from an ISR, and therefore needs to be very fast. 
	void Vessel::updatePWMOutput(bool newOutput)
	{
		if (usesPWM && isPID()) 
		{
				heatPin[role].set(newOutput); else heatPin[role].set(newOutput);
		}
	}

#endif

	//Turn output on or off based on temperature, returning whether the output is on
	void Vessel::updateOutput()
	{
		//Update temperature for use in all later calculations
		updateTemperature();

		if (estop || getVolume() < minVolume || (temperature == BAD_TEMP && heatOverride != SOFTSWITCH_MANUAL && heatOverride != SOFTSWITCH_ON) || //Allow manual control of heat even when temp sensor is broken
			(vesselMinTrigger(minTriggerPin) && !vesselMinTrigger(minTriggerPin)->get()))

			//The latter condition checks for the digital trigger input for low volume 
		{
			//Turn output off due to error condition
			heatPin.set(LOW);
			PIDoutput = 0;
			if (estop) bitClear(actProfiles, heatConfig); //Keep the valves and pumps working unless we have an estop condition. The volume triggers only affect the heating output
			return;
		}

#ifdef RGBIO8_ENABLE
		//This code is worth excluding when RGBIO8 is disabled because it runs every time we update the output
		if (heatOverride == SOFTSWITCH_OFF)
		{
			heatPin.set(LOW);
			updateHeatValves();
			return;
		}
		else if (heatOverride == SOFTSWITCH_ON)
		{
			heatPin.set(HIGH);
			updateHeatValves();
			return;
		}
		//Other possible value is SOFTSWITCH_AUTO and SOFTSWITCH_MANUAL
#endif
#ifdef USESTEAM
//Note that the steam system and mash tun heat potentially operate off of different temperature sensors (though they can also use the same sensor).
//In this case each one will activate/deactivate when it's within the right range.
//This could potentially cause both outputs to be inactive at once when the temperature is not at setpoint, if the mash temp is close to the setpoint
//but the steam sensor is reading a bigger divergence. This can be corrected by using a single sensor, or will fix itself over time.
		if (temperature + STEAM_OFFSET * 100 > setpoint && temperature < setpoint)
		{
		  if (usesSteam)
		  {
		  //If we are using a secondary mash temperature control system and we are within the range of it,
		  //turn this output off and let the steam output handle it.
		  heatPin.set(LOW);
		  bitClear(actProfiles, idleConfig);
		  bitSet(actProfiles, heatConfig); //We are below the setpoint as noted above, so we have to turn the mash heat valves on
		  return;
		  }
		}
		else if (role == VS_STEAM)
		  {
		    //If the temperature is outside the small range around the setpoint where the steam system should be active, deactivate it
		    heatPin.set(LOW);
			if (!vessels[VS_MASH]->getOutput())  //Both steam and mash are inactive, so deactivate valves
			{
				bitClear(actProfiles, heatConfig);
				bitSet(actProfiles, idleConfig);
			}
		    return;
		  }
		//Note that if the mash itself is fully active, the mash code controls the heat and valve outputs
#endif
		if (usePID || heatOverride == SOFTSWITCH_MANUAL)
		{
			//Note that this uses pointers to the temperature, output and feedforward variables
			if (heatOverride != SOFTSWITCH_MANUAL)
				pid->Compute(); //If it's manual, preserve the manual value, which is directly set by the manual input code
			
#ifdef USEPWM
			//If using PWM, the output is updated by a separate timer-driven ISR, so we should exit here.
			//It's important that we call updateTemperature and pid->Compute() above, though, because it's not recalculated in the ISR.
			if (usesPWM)
			{
				updateHeatValves();
				return;
			}
#endif
			//only 1 call to millis needed here, and if we get hit with an interrupt we still want to calculate based on the first read value of it
			unsigned long timestamp = millis();
			if (timestamp - cycleStart[role] > PIDcycle * 100) cycleStart[role] += PIDcycle * 100;

			//cycleStart is the millisecond value when the current PID cycle started.
			//We compare the 
			if (PIDoutput >= timestamp - cycleStart[role] && timestamp != cycleStart[role])
			{
				heatPin.set(HIGH);
			}
			else
			{
				heatPin.set(LOW);
			}
		}
		else
		{
			if (getTemperature() < getSetpoint()-hysteresis)
			{
				//Turn output on
				heatPin.set(HIGH);
				PIDoutput = maxPower;
			}
			else if (getTemperature() > getSetpoint()+hysteresis)
			{
				//Turn output off
				heatPin.set(LOW);
				PIDoutput = 0;
			}
		}
		updateHeatValves();
	}
	
	void Vessel::updateHeatValves()
	{
		if (PIDoutput)
		{
			bitClear(actProfiles, idleConfig);
			bitSet(actProfiles, heatConfig);
		}
		else
		{
			bitClear(actProfiles, heatConfig);
			bitSet(actProfiles, idleConfig);
		}
	}

	void Vessel::manualOutput(int newOutput)
	{
		PIDoutput = newOutput * PIDcycle;
		heatOverride = SOFTSWITCH_MANUAL;
		updateOutput();
	}

	void Vessel::setPID(bool newPID)
	{
		usePID = newPID;
		byte options = EEPROM.read(72);
		bitWrite(options, role, newPID);
		EEPROM.write(72, options);
	}

	void Vessel::setHysteresis(float newHysteresis)
	{
		EEPROM.write(77 + role * 5, newHysteresis); 
		hysteresis = newHysteresis;
	}

	void Vessel::setPIDCycle(float newPIDCycle)
	{
		EEPROM.write(76 + role * 5, newPIDCycle);
		PIDcycle = newPIDCycle;
	}

	void Vessel::setHeatOverride(SoftSwitch oride)
	{//Forces the element on or off, or sets it to auto. Used with RGBIO8 soft switches.
		heatOverride = oride;
		updateOutput();
	}

	SoftSwitch Vessel::getHeatOverride() {
		return heatOverride;
	}

	
	/////////////////////////////////////////////////////
	// Volume settings
	/////////////////////////////////////////////////////

	void Vessel::updateVolumeCalibration(byte index, unsigned long vol, int pressure)
	{
#ifdef USESTEAM
	  if (role == VS_STEAM)
	  {
	    if (index == 0)
	    {
	      volumeCalibrationPressure[0] = pressure;
	      EEPROMwriteInt(114, value);
	      return;
	    }
	    else
	      //Error!
	      return;
	  }
#endif
		volumeCalibrationVolume[index] = vol;
		volumeCalibrationPressure[index] = pressure;

		//Write to eeprom
		EEPROMwriteLong(119 + role * 40 + index * 4, vol);
		EEPROMwriteInt(239 + role * 20 + index * 2, pressure);
	}

#ifdef USESTEAM
	void Vessel::setPressureSensitivity(int newSensitivity)
	{
	  steamPressureSensitivity = newSensitivity;
	  
	  #ifdef USEMETRIC
	    pid.SetInputLimits(0, 50000 / newSensitivity);
	  #else
	    pid.SetInputLimits(0, 7250 / newSensitivity);
	  #endif
	  
	  EEPROMwriteInt(117, value);  
	}
#endif

	void Vessel::setCapacity(float newCapacity)
	{
		capacity = newCapacity;
		EEPROMwriteLong(93 + role * 4, newCapacity);
	}


	void Vessel::setDeadspace(float newDeadspace)
	{
		deadspace = newDeadspace;
		EEPROMwriteInt(105 + role * 2, newDeadspace); 
	}

	float Vessel::getVolume() 
	{
		return volume / 1000.0;
	} 
	
	//Take a sample of the volume. Note that all volume pins are assigned in all HW configs. If no sensor is attached and no calibration present, the value read is zero.
	void Vessel::takeVolumeReading()
	{
		byte reading;
		//Take volume reading
		reading = readVolume(volumePinID, volumeCalibrationVolume, volumeCalibrationPressure);

		volume = volume + (reading - volumeReadings[oldestVolumeReading]) / VOLUME_READ_COUNT;
		volumeReadings[oldestVolumeReading] = reading;
		oldestVolumeReading = (oldestVolumeReading + 1) % VOLUME_READ_COUNT; //This could be made faster by using a power of 2 as the read count and using a bitmask

		updateFlowrateCalcs();
	}

	unsigned int Vessel::getCalibrationValue() {
		unsigned int newSensorValueAverage = 0;

		for (byte i = 0; i < VOLUME_READ_COUNT; i++) {
			newSensorValueAverage += analogRead(volumePinID);
			unsigned long intervalEnd = millis() + VOLUME_READ_INTERVAL;
			while (millis() < intervalEnd) {
#ifdef HEARTBEAT
			heartbeat();
#endif
			}
		}

		return (newSensorValueAverage / VOLUME_READ_COUNT);
	}

	void initVessels()
	{
		byte pidLimits[4] = { PIDLIMIT_HLT, PIDLIMIT_MASH, PIDLIMIT_KETTLE, PIDLIMIT_STEAM };
		bool initIncludeAux[3] = { false, false, false };
		bool mashIncludeAux[3] = { MASH_AVG_AUX1, MASH_AVG_AUX2, MASH_AVG_AUX3 };

		byte triggerPin;
		
#ifdef SINGLE_VESSEL_SUPPORT
		if (vessels[0]) delete vessels[0];
		if (FEEDFORWARD)
			vessels[0] = vessels[1] = vessels[2] = new Vessel(0, mashIncludeAux, FEEDFORWARD, 0, 0, TRIGGER_HLTMIN, pidLimits[0]);
		else
			vessels[0] = vessels[1] = vessels[2] = new Vessel(0, mashIncludeAux, 0, 0, TRIGGER_HLTMIN, pidLimits[0]);
#else

		

		for (byte i = 0; i < NUM_VESSELS; i++)
		{
		  vessels[i]->setStage(FILL);
#ifdef KETTLE_AS_MASH
			if (i == VS_MASH)
			{
				continue;
			}
#endif
#ifdef HLT_AS_KETTLE
			if (i == VS_KETTLE)
			{
				vessels[i] = vessels[VS_HLT];
				continue;
			}
#endif
			if (vessels[i]) delete vessels[i];

			switch (i)
			{
				case 0:
					triggerPin = TRIGGER_HLTMIN;
					break;
				case 1:
					triggerPin = TRIGGER_MASHMIN;
					break;
				case 2:
					triggerPin = TRIGGER_KETTLEMIN;
					break;
				default:
					triggerPin = 0;
			}
			if (i == 2 && FEEDFORWARD)
				vessels[i] = new Vessel(i, mashIncludeAux, FEEDFORWARD, 0, triggerPin, pidLimits[i]); //That random 0 is minVolume which is not currently implemented due to lack of UI
			else
				vessels[i] = new Vessel(i, initIncludeAux, 0, 0, triggerPin, pidLimits[i]);
		
		}
#endif 
#ifdef KETTLE_AS_MASH
			vessels`[VS_MASH] = vessels[VS_KETTLE];
#endif
	}

	//TODO: Replace magic number 10 (number of volume samples to average over) with a defined constant
	//TODO: Running this calculation on every volume measurement could slow the system. If so, it could be sped up simply by calculating it less often - it shouldn't change much.
	void Vessel::updateFlowrateCalcs()
	{
		unsigned long tempmill = millis();
				
		//Check flowrate periodically (FLOWRATE_READ_INTERVAL)
		if (tempmill - lastFlowCheck >= FLOWRATE_READ_INTERVAL) {
				
				// note that the * 60000 is from converting thousands of a gallon / miliseconds to thousands of a gallon / minutes 
				double newVol = getVolume();
				flowRate = flowRate + newVol / 10;
				flowRateHistory[flowRateIndex] = newVol;			
				
#ifdef DEBUG_VOL_READ
				logStart_P(LOGDEBUG);
				logField_P(PSTR("VOL_Calc"));
				logFieldI(i);
				logFieldI(flowRate);
#endif
				flowRateIndex++;
				if (flowRateIndex > 9) flowRateIndex = 0;
		}
		lastFlowCheck = tempmill;
	}

	//This function updates the valve profiles to the right stage for this vessel and program stage.
	//Note: there are no valves for steam in any stage. These are purely handled by the mash valve config and single steam output. This could be changed
	//if there is demand; it would only require new UI for this stage and some minor global variable cleanup.
	void Vessel::updateValveConfigs()
	{
		//Note: there are no valves for steam in any stage. These are purely handled by the mash valve config and single steam output.

		//Zero out the valve configs
		bitClear(actProfiles, heatConfig);
		bitClear(actProfiles, idleConfig);
		bitClear(actProfiles, fillConfig);
		
		heatConfig = idleConfig = fillConfig = 0;

		//Note: a vessel may hold multiple roles. This code assigns all relevant valve configs to that vessel. This is why we do |= instead of just =
		switch(stage)
		{
			case FILL:
					if (role == VS_HLT)
					{
						fillConfig |= VLV_FILLHLT;
						heatConfig |= VLV_HLTHEAT;
						idleConfig |= VLV_HLTIDLE;
					}
					if (role == VS_MASH)
					{
						fillConfig |= VLV_FILLMASH;
						heatConfig |= VLV_MASHHEAT;
						idleConfig |= VLV_MASHIDLE;
					}
					//No valves for kettle in fill stage
					break;
				
			case MASH:
				if (role == VS_HLT)
				{
					heatConfig |= VLV_HLTHEAT;
					idleConfig |= VLV_HLTIDLE;
				}
				if (role == VS_MASH)
				{
					heatConfig |= VLV_MASHHEAT;
					idleConfig |= VLV_MASHIDLE;
				}
				//No valves for kettle in mash stage
				break;
			case SPARGE:
				if (role == VS_HLT)
				{
					heatConfig |= VLV_HLTHEAT; //These are only used if the HLT is set to heat during sparge
					idleConfig |= VLV_HLTIDLE;
					//Shouldn't need to use a fill profile for HLT during sparge, because all filling should happen during the fill and refill stages
				}
				if (role == VS_MASH)
				{
					heatConfig |= VLV_MASHHEAT;
					idleConfig |= VLV_MASHIDLE;
					fillConfig |= VLV_SPARGEIN;
				}
				if (role == VS_KETTLE)
				{
					heatConfig |= VLV_KETTLEHEAT; //These aren't used yet because there is no kettle setpoint during sparging, but could be enabled to speed the brew day by starting kettle heating during the sparge
					idleConfig |= VLV_KETTLEIDLE;
					fillConfig |= VLV_SPARGEOUT;
				}
				break;
			case BOIL:
				//HLT and MLT do nothing during boiling
				if (role == VS_KETTLE)
				{
					heatConfig |= VLV_KETTLEHEAT; //These aren't used yet because there is no kettle setpoint during sparging, but could be enabled to speed the brew day by starting kettle heating during the sparge
					idleConfig |= VLV_KETTLEIDLE;
					//Shouldn't be moving any liquid into the kettle during boiling
				}
				break;
		}
	}
