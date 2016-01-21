/*
 Copyright (C) 2015 Eric Yanush
 
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
 
 Documentation, Forums and more information available at http://www.brewtroller.com
*/

#ifndef ConfigManager_hpp
#define ConfigManager_hpp

#include <stdint.h>
#include "Enum.h"
#include "Types.hpp"
#include <avr/eeprom.h>

constexpr uint8_t EEPROM_FINGERPRINT = 252;
constexpr uint8_t EEPROM_SCHEMA_VER = 4;

typedef uint8_t TempSensorAddress[TS_ADDR_BYTES];

struct output_config {
    uint8_t PGain;
    uint8_t IGain;
    uint8_t DGain;
    
    uint8_t cycleTime;
    uint8_t hysteresis;
};

typedef uint32_t VolumeCalibs[VOL_CALIB_COUNT];
typedef uint16_t VolCalibData[VOL_CALIB_COUNT];

struct program_config {
    char name[PROG_NAME_LEN];
    
    //Padding inserted for backwards compatability
    //TODO: Remove all padding
    char pad;
    
    uint8_t spargeTemp;
    
    uint16_t boilMins;
    
    uint16_t mashRatio;
    
    uint8_t mashStepTemp[MASHSTEP_COUNT];
    
    uint8_t mashStepLength[MASHSTEP_COUNT];
    
    uint32_t batchVolume;
    
    uint8_t mashLiquorHeatSource;
    
    uint8_t hltTemperature;
    
    uint8_t pitchTemperature;
    
    uint16_t boilAdditionAlarms;
    
    uint32_t grainWeight;
    
    char pad1[9];
} __attribute__((packed));

struct modbusboard_cfg {
    uint8_t address;
    uint16_t reg;
    uint8_t coilCount;
    uint8_t offset;
} __attribute__((packed));

struct config_t {
    TempSensorAddress tempSensorAddresses[NUM_TS];
    
    uint8_t pidEnabledFlags;
    output_config pidConfigs[VS_COUNT];
    
    uint32_t hltCapacity;
    uint32_t mltCapacity;
    uint32_t kettleCapacity;
    
    uint16_t hltVolLoss;
    uint16_t mltVolLoss;
    uint16_t kettleVolLoss;
    
    uint8_t boilTemp;
    uint8_t boilPower;
    
    uint8_t evapRate;
    
    uint16_t steamZero;
    uint8_t steamTarget;
    uint16_t steamPSense;
    
    VolumeCalibs hltCalibVols;
    VolumeCalibs mltCalibVols;
    VolumeCalibs kettleCalibVols;
    
    VolCalibData hltCalibDat;
    VolCalibData mltCalibDat;
    VolCalibData kettleCalibDat;
    
    uint8_t hltSetPoint;
    uint8_t mltSetPoint;
    uint8_t kettleSetPoint;
    
    uint16_t mashTimer;
    uint16_t boilTimer;
    uint8_t timerStatus;
    
    uint16_t boilAdditionsTrigger;
    
    //Padding inserted for backwards compatibility
    //TODO: Remove All Padding and reorganize EEPROM Fields
    uint8_t pad1[4];
    
    ProgramThread pgmThreads[PROGRAMTHREAD_MAX];
    
    //Padding inserted for backwards compatibility
    //TODO: Remove all padding
    uint8_t pad2[81];
    
    uint16_t delayMins;
    
    uint8_t grainTemp;
    
    uint32_t valveProfileCfg[NUM_VLVCFGS];
    
    //Padding inserted for backwards compatibility
    //TODO: Remove all padding
    uint8_t pad3[305];
    
    program_config programs[RECIPE_MAX];
    
    //Padding inserted for backwards compatibility
    //TODO: Remove all padding
    uint8_t pad4[60];
    
    uint8_t btFingerprint;
    
    uint8_t eepromSchemaVersion;
    
    uint8_t lcdBrightness;
    uint8_t lcdContrast;
    
    uint8_t triggerPins[5];
    
    //Padding inserted for backwards compatibility
    //TODO: Remove all padding
    uint8_t pad5[10];
    
    modbusboard_cfg modbusBrdConf[2];
    
} __attribute__((packed));

class ConfigManager {
    
public:
    
    /**
     init: intializes the configuration manager
     paramter config: pointer to a config struct in eeprom
     - note: All other functions in this class require this method to be called before they can be called.
     */
    static void init(config_t* config);
    
    /**
     configIsValid: Method to detrmine if the config store is valid (has the right schema version)
     - returns: true if the config store is valid, false otherwise
     */
    static bool configIsValid();
    
    /**
     initConfig: Method initializes backing config store to default values
     - returns: void
     */
    static void initConfig();
    
    /**
     loadGlobals: Ugly method for loading globals from eeprom, for compatibility with legacy code
     - returns: void
     - Note: This method will go away as soon as all the global vars can be removed, this method is NOT subject to any unit tests!!!
     */
    static void loadConfig();
    
    /**
     setBoilTemp: Update the boil temperature parameter
     - parameter newBoilTemp: the value to update the store with
     - returns: void
     */
    static void setBoilTemp(const uint8_t newBoilTemp);
    
    /**
     getBoilTemp: Return the current boil temperature parameter
     - returns: The current value of the boil temperature parameter from the config store
     */
    static uint8_t getBoilTemp();
    
    /**
     setVolumeCalib: Update a volume calibration
     - parameter vessel: The vessel index; defined in Enum.h. This will be deprecated in the future
     - parameter slot: The calibration slot to update (0 .. VOL_CALIB_COUNT)
     - parameter data: The data value to use for the calibration
     - parameter volume: The volume value for the calibration data point
     - returns: void
     */
    static void setVolumeCalib(const uint8_t vessel, const uint8_t slot, const uint16_t data, const uint32_t volume);
    
    /**
     setEvapRate: Updates the evaporation rate parameter
     - parameter newEvapRate: The value to update the config store with
     - returns: void
     */
    static void setEvapRate(const uint8_t newEvapRate);
    
    /**
     getEvapRate: Return the current EvapRate value from the store
     - returns: the current evapRate value in the store
     */
    static uint8_t getEvapRate();
    
    /**
     setPIDEnabled: Updates the PID enabled flag for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter enabled: Enable PID for the vessel index if true, else disable it
     - returns: void
     */
    static void setPIDEnabled(uint8_t vessel, bool enabled);
    
    /**
     setPIDCycle: Updates the PID Cycle time value for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter newCycleVal: The value to update the cycle time with
     - returns: void
     - precondition: init() has been called
     */
    static void setPIDCycle(uint8_t vessel, uint8_t newCycleVal);
    
    /**
     setPIDPGain: Updates the PID P Gain value for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter newPGain: The value to update the P Gain to
     - returns: void
     */
    static void setPIDPGain(uint8_t vessel, uint8_t newPGain);
    
    /**
     setPIDIGain: Updates the PID I Gain value for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter newIGain: The value to update the I Gain to
     - returns: void
     */
    static void setPIDIGain(uint8_t vessel, uint8_t newIGain);
    
    /**
     setPIDDGain: Updates the PID D Gain value for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter newPGain: The value to update the D Gain to
     - returns: void
     */
    static void setPIDDGain(uint8_t vessel, uint8_t newDGain);
    
    /**
     getPIDPGain: Retrieves the PID P Gain value for the specified vessel
     - parameter vessel: The vessel index to be retrieved for; defined in Enum.h
     - returns: the current P gain (uint8_t) for the vessel
     */
    static uint8_t getPIDPGain(uint8_t vessel);
    
    /**
     getPIDIGain: Retrieves the PID I Gain value for the specified vessel
     - parameter vessel: The vessel index to be retrieved for; defined in Enum.h
     - returns: the current I gain (uint8_t) for the vessel
     */
    static uint8_t getPIDIGain(uint8_t vessel);
    
    /**
     getPIDDGain: Retrieves the PID D Gain value for the specified vessel
     - parameter vessel: The vessel index to be retrieved for; defined in Enum.h
     - returns: the current D gain (uint8_t) for the vessel
     */
    static uint8_t getPIDDGain(uint8_t vessel);
    
    /**
     setHysteresis: Updates the Hysteresis value for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter newHysteresis: The value to update the hysteresis to
     - returns: void
     */
    static void setHysteresis(uint8_t vessel, uint8_t newHysteresis);
    
    /**
     setSteamTarget: Updates the Steam Target value
     - parameter newTarget: The value to update the Steam Target
     - returns: void
     */
    static void setSteamTarget(uint8_t newTarget);
    
    /**
     getSteamTarget: Retrieves the Steam Target value
     - returns: The current value (uint8_t)
     */
    static uint8_t getSteamTarget();
    
    /**
     setSteamZero: Updates the Steam Zero value
     - parameter newZero: The value to update the Steam Zero to
     - returns: void
     */
    static void setSteamZero(uint16_t newZero);
    
    /**
     setSteamPSense: Updates the Steam PSense value
     - parameter newPSense: The value to update the Steam Psense to
     - returns: void
     */
    static void setSteamPSense(uint16_t newPSense);
    
    /**
     setProgramName: Updates the program name for the specified program
     - parameter program: The program number to update
     - parameter newName: a c-string of length 19 Characters, padded with spaces, if necessary
     - returns: void
     */
    static void setProgramName(uint8_t program, char* newName);
    
    /**
     getProgramName: Retrieves the program name for the specified program
     - parameter program: The program number to retrieve for
     - parameter name: A char buffer of length 20 to place the name into
     - returns: void
     */
    static void getProgramName(uint8_t program, char* name);
    
    /**
     setProgramMashStepTemp: Sets the target temperature for the specified mash step of the specified program
     - parameter program: The program number to set the step temperature for
     - parameter step: The mash step index to set the temperature for
     - parameter temperature: The target temperature
     - returns: void
     */
    static void setProgramMashStepTemp(uint8_t program, uint8_t step, uint8_t temperature);
    
    /**
     getProgramMashStepTemp: Gets the target temperature for the specified mash step of the specified program
     - parameter program: The program number to get the step temperature for
     - parameter step: The mash step index to get the temperature for
     - returns: The current mash step target temperature for the program and step specified
     */
    static uint8_t getProgramMashStepTemp(uint8_t program, uint8_t step);
    
    /**
     setProgramMashStepMins: Sets the target length for the specified mash step of the specified program
     - parameter program: The program number to set the step length for
     - parameter step: The mash step index to set the length for
     - parameter mins: The target length in minutes of the step
     - returns: void
     */
    static void setProgramMashStepMins(uint8_t program, uint8_t step, uint8_t mins);
    
    /**
     getProgramMashStepMins: Gets the target length for the specified mash step of the specified program
     - parameter program: The program number to get the step length for
     - parameter step: The mash step index to get the length for
     - returns: The current mash step length for the program and step specified, in minutes
     */
    static uint8_t getProgramMashStepMins(uint8_t program, uint8_t step);
    
    /**
     setProgramBatchVol: Sets the Batch Volume of the specified program
     - parameter program: The program number to set the Batch Volume for
     - parameter newBatchVol: The new Batch Volume for the program
     - returns: void
     */
    static void setProgramBatchVol(uint8_t program, uint32_t newBatchVol);
    
    /**
     getProgramBatchVol: Gets the Batch Volume of the specified program
     - parameter program: The program number to get the Batch Volume for
     - returns: The current Batch Volume for the program
     */
    static uint32_t getProgramBatchVol(uint8_t program);
    
    /**
     setProgramMashRatio: Sets the Mash Ratio of the specified program
     - parameter program: The program number to set the Mash Ratio for
     - parameter newMashRatio: The new Mash Ratio for the program
     - returns: void
     */
    static void setProgramMashRatio(uint8_t program, uint16_t newMashRatio);
    
    /**
     getProgramMashRatio: Gets the Mash Ratio of the specified program
     - parameter program: The program number to get the Mash Ratio for
     - returns: The current Mash Ratio for the program
     */
    static uint16_t getProgramMashRatio(uint8_t program);
    
    /**
     setProgramGrainWeight: Sets the Grain Weight of the specified program
     - parameter program: The program number to set the Grain Weight for
     - parameter newGrainWeight: The new Grain Weight for the program
     - returns: void
     */
    static void setProgramGrainWeight(uint8_t program, uint32_t newGrainWeight);
    
    /**
     getProgramGrainWeight: Gets the Grain Weight of the specified program
     - parameter program: The program number to get the Grain Weight for
     - returns: The current Grain Weight for the program
     */
    static uint32_t getProgramGrainWeight(uint8_t program);
    
    /**
     setProgramSpargeTemp: Sets the Sparge Temperature of the specified program
     - parameter program: The target program number
     - parameter newSpargeTemp: The new Sparge Temperature for the program
     - returns: void
     */
    static void setProgramSpargeTemp(uint8_t program, uint8_t newSpargeTemp);
    
    /**
     getProgramSpargeTemp: Gets the Sparge Temperature of the specified program
     - parameter program: The target program number
     - returns: The current Sparge Temperature for the program
     */
    static uint8_t getProgramSpargeTemp(uint8_t program);
    
    /**
     setProgramHLTTemp: Sets the HLT Temperature of the specified program
     - parameter program: The target program number
     - parameter newHLTTemp: The new HLT Temperature for the program
     - returns: void
     */
    static void setProgramHLTTemp(uint8_t program, uint8_t newHLTTemp);
    
    /**
     getProgramHLTTemp: Gets the HLT Temperature of the specified program
     - parameter program: The target program number
     - returns: The current HLT Temperature for the program
     */
    static uint8_t getProgramHLTTemp(uint8_t program);
    
    /**
     setProgramPitchTemp: Sets the Pitch Temperature of the specified program
     - parameter program: The target program number
     - parameter newPitchTemp: The new Pitch Temperature for the program
     - returns: void
     */
    static void setProgramPitchTemp(uint8_t program, uint8_t newPitchTemp);
    
    /**
     getProgramPitchTemp: Gets the Pitch Temperature of the specified program
     - parameter program: The target program number
     - returns: The current Pitch Temperature for the program
     */
    static uint8_t getProgramPitchTemp(uint8_t program);
    
    /**
     setProgramBoilMins: Sets the target length for the Boil step of the specified program
     - parameter program: The target program number
     - parameter mins: The target length in minutes of the Boil
     - returns: void
     */
    static void setProgramBoilMins(uint8_t program, uint16_t mins);
    
    /**
     getProgramBoilMins: Gets the target length for the Boil step of the specified program
     - parameter program: The target program number
     - returns: The current Boil length for the program in minutes
     */
    static uint16_t getProgramBoilMins(uint8_t program);
    
    /**
     setProgramMLHeatSource: Sets the vessel used to heat the mash for a specified program
     - parameter program: The target program number
     - parameter vessel: The target vessel index used to heat the mash
     - returns: void
     */
    static void setProgramMLHeatSource(uint8_t program, uint8_t vessel);
    
    /**
     getProgramMLHeatSource: Gets the vessel used to heat the mash, in a specified program
     - parameter program: The target program number
     - returns: The current vessel index used for mash heat in the program
     */
    static uint8_t getProgramMLHeatSource(uint8_t program);
    
    /**
     setProgramBoilAdditionAlarms: Sets the Boil Addition alarm flags of the specified program
     - parameter program: The target program number
     - parameter flags: The new flags bitfield
     - returns: void
     */
    static void setProgramBoilAdditionAlarms(uint8_t program, uint16_t flags);
    
    /**
     getProgramBoilAdditionAlarms: Gets the Boil Addition alarm flags of the specified program
     - parameter program: The target program number
     - returns: The current Boil addtion alarm flag bitfield for the program
     */
    static uint16_t getProgramBoilAdditionAlarms(uint8_t program);

    /**
     setTempSensorAddress: Sets the unique address for the specified sensor
     - parameter sensor: The sensor index, defined in ENUM.h
     - parameter newAddr: The address to set the sensor address to
     - returns: void
     */
    static void setTempSensorAddress(uint8_t sensor, TempSensorAddress newAddr);
    
    /**
     setVesselCapacity: Sets the Volume capacity of the specified vessel
     - parameter vessel: The target vessel index, defined in ENUM.h
     - parameter newCapacity: The new Capacity for the vessel
     - returns: void
     */
    static void setVesselCapacity(uint8_t vessel, uint32_t newCapacity);
    
    /**
     getVesselCapacity: Gets the Volume Capacity of the Vessel
     - parameter vessel: The target vessel index, defined in ENUM.h
     - returns: The current Volume Capacity for the vessel
     */
    static uint32_t getVesselCapacity(uint8_t vessel);
    
    /**
     setVesselVolumeLoss: Sets the Volume Loss of the specified vessel
     - parameter vessel: The target vessel index, defined in ENUM.h
     - parameter newLoss: The new Volume Loss for the vessel
     - returns: void
     */
    static void setVesselVolumeLoss(uint8_t vessel, uint16_t newLoss);
    
    /**
     getVesselVolumeLoss: Gets the Volume Loss of the Vessel
     - parameter vessel: The target vessel index, defined in ENUM.h
     - returns: The current Volume Loss for the vessel
     */
    static uint16_t getVesselVolumeLoss(uint8_t vessel);
    
    /**
     setValveProfileConfig: Sets the config for a specified valve profile
     - parameter profile: The target profile index, defined in ENUM.h
     - parameter newConfig: The new Config for the profile
     - returns: void
     */
    static void setValveProfileConfig(uint8_t profile, uint32_t newConfig);
    
    /**
     setVesselTempSetpoint: Sets the Temperature setpoint for a specified vessel
     - parameter vessel: The target vessel index, defined in ENUM.h
     - parameter newSetpoint: The new temperature setpoint for the vessel
     - returns: void
     */
    static void setVesselTempSetpoint(uint8_t vessel, uint8_t newSetpoint);
    
    /**
     setBoilPower: Sets the Boil Power % used when boiling
     - parameter newBoilPower: The Boil Power% to update to
     - returns: void
     */
    static void setBoilPower(uint8_t newBoilPower);
    
    /**
     setDelayMins: Updates the Delay Minutes Value
     - parameter newDelayMins: The value to update Delay Minutes to
     - returns: void
     */
    static void setDelayMins(uint16_t newDelayMins);
    
    /**
     getDelayMins: Gets the Delay Minutes Value
     - returns: The current Delay minutes value
     */
    static uint16_t getDelayMins();
    
    /**
     setGrainTemperature: Updates the Grain Temperature Value
     - parameter newGrainTemp: The value to update Grain Temperature to
     - returns: void
     */
    static void setGrainTemperature(uint8_t newGrainTemp);
    
    /**
     getGrainTemperature: Gets the Grain Temperature Value
     - returns: The current Grain Temperature value
     */
    static uint8_t getGrainTemperature();
    
    /**
     setTriggerPin: Updates pin number used for the specified trigger index
     - parameter trigger: The target trigger index, defined in ENUM.h
     - parameter pinNum: The new pin number to use for the trigger
     - returns: void
     */
    static void setTriggerPin(TriggerType trigger, uint8_t pinNum);
    
    /**
     getTriggerPin: Gets the trigger pin number used for a trigger index
     - parameter trigger: The target trigger type, defined in ENUM.h
     - returns: The current pin number for the trigger
     */
    static uint8_t getTriggerPin(TriggerType trigger);
    
    /**
     setProgramThread: Updates the program thread with the specified index
     - parameter threadIndex: The target thread index
     - parameter thread: A pointer to a thread structure to save
     - returns: void
     */
    static void setProgramThread(uint8_t threadIndex, ProgramThread* thread);
    
    /**
     getProgramThread: Gets the program thread specified from storage
     - parameter threadIndex: The target program thread index
     - parameter threadOut: A pointer to a program thread structure to which in which the thread should be placed
     - returns: void
     */
    static void getProgramThread(uint8_t threadIndex, ProgramThread* threadOut);
    
    /**
     getBoilAdditionsTrigger: Gets the additions trigger bitfield
     - returns: The current additions trigger bitfield
     */
    static uint16_t getBoilAdditionsTrigger();
    
    /**
     setGrainTemperature: Updates the Boil Additions Trigger bitfield
     - returns: void
     */
    static void setBoilAdditionsTrigger(uint16_t trigger);
    
    /**
     setTimerStatus: Update the status of a Timer
     - parameter timer: The timer index to update; Defined in ENUM.h
     - parameter enabled: A boolean value representing the current state of the timer
     - returns: void
     */
    static void setTimerStatus(uint8_t timer, bool enabled);
    
    /**
     getTimerStatus: Get the status of a Timer
     - parameter timer: The timer index to update; Defined in ENUM.h
     - returns: bool indicating if the timer is enabled
     */
    static bool getTimerStatus(uint8_t timer);
    
    /**
     setCurrentTimerValue: Set the current minute value of a timer
     - parameter timer: The timer index to update; Defined in ENUM.h
     - parameter minutes: The current minutes value of the timer
     - returns: void
     */
    static void setCurrentTimerValue(uint8_t timer, uint16_t minutes);
    
    /**
     getCurrentTimerValue: Get the current minute value of a timer
     - parameter timer: The timer index to retrieve the value for; Defined in ENUM.h
     - returns: uint16_t the current timer value in minutes
     */
    static uint16_t getCurrentTimerValue(uint8_t timer);
    
    /**
     setAlarmStatus: Set the current status of the alarm
     - parameter on: A boolean value representing the state of the alarm
     - returns: void
     */
    static void setAlarmStatus(bool on);
    
    /**
     getAlarmStatus: Get the current status of the alarm
     - returns: bool; the current state of the alarm
     */
    static bool getAlarmStatus();
};

extern config_t eepromConfig;

#endif /* EEPROM_hpp */
