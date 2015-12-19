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

#ifndef EEPROM_hpp
#define EEPROM_hpp

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
    
    uint16_t lcdBrightContrast;
    
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
     */
    static void init(config_t* config);
    
    /**
     configIsValid: Method to detrmine if the config store is valid (has the right schema version)
     - precondition: init() has been called
     - returns: true if the config store is valid, false otherwise
     */
    static bool configIsValid();
    
    /**
     initConfig: Method initializes backing config store to default values
     - precondition: init() has been called
     - returns: void
     */
    //static void initConfig();
    
    /**
     loadGlobals: Ugly method for loading globals from eeprom, for compatibility with legacy code
     - precondition: init() has been called
     - returns: void
     - Note: This method will go away as soon as all the global vars can be removed, this method is NOT subject to any unit tests!!!
     */
    static void loadConfig();
    
    /**
     setBoilTemp: Update the boil temperature parameter
     - parameter newBoilTemp: the value to update the store with
     - precondition: init() has been called
     - returns: void
     */
    static void setBoilTemp(const uint8_t newBoilTemp);
    
    /**
     getBoilTemp: Return the current boil temperature parameter
     - precondition: init() has been called
     - returns: The current value of the boil temperature parameter from the config store
     */
    static uint8_t getBoilTemp();
    
    /**
     setVolumeCalib: Update a volume calibration
     - parameter vessel: The vessel index; defined in Enum.h. This will be deprecated in the future
     - parameter slot: The calibration slot to update (0 .. VOL_CALIB_COUNT)
     - parameter data: The data value to use for the calibration
     - parameter volume: The volume value for the calibration data point
     - precondition: init() has been called
     - returns: void
     */
    static void setVolumeCalib(const uint8_t vessel, const uint8_t slot, const uint16_t data, const uint32_t volume);
    
    /**
     setEvapRate: Updates the evaporation rate parameter
     - parameter newEvapRate: The value to update the config store with
     - returns: void
     - precondition: init() has been called
     */
    static void setEvapRate(const uint8_t newEvapRate);
    
    /**
     getEvapRate: Return the current EvapRate value from the store
     - returns: the current evapRate value in the store
     - precondition: init() has been called
     */
    static uint8_t getEvapRate();
    
    /**
     setPIDEnabled: Updates the PID enabled flag for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter enabled: Enable PID for the vessel index if true, else disable it
     - returns: void
     - precondition: init() has been called
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
     - precondition: init() has been called
     */
    static void setPIDPGain(uint8_t vessel, uint8_t newPGain);
    
    /**
     setPIDIGain: Updates the PID I Gain value for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter newIGain: The value to update the I Gain to
     - returns: void
     - precondition: init() has been called
     */
    static void setPIDIGain(uint8_t vessel, uint8_t newIGain);
    
    /**
     setPIDDGain: Updates the PID D Gain value for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter newPGain: The value to update the D Gain to
     - returns: void
     - precondition: init() has been called
     */
    static void setPIDDGain(uint8_t vessel, uint8_t newDGain);
    
    /**
     setHysteresis: Updates the Hysteresis value for the specified vessel
     - parameter vessel: The vessel index to be updated; defined in Enum.h
     - parameter newHysteresis: The value to update the hysteresis to
     - returns: void
     - precondition: init() has been called
     */
    static void setHysteresis(uint8_t vessel, uint8_t newHysteresis);
    
    /**
     setSteamTarget: Updates the Steam Target value
     - parameter newTarget: The value to update the Steam Target
     - returns: void
     - precondition: init() has been called
     */
    static void setSteamTarget(uint8_t newTarget);
    
    /**
     setSteamZero: Updates the Steam Zero value
     - parameter newZero: The value to update the Steam Zero to
     - returns: void
     - precondition: init() has been called
     */
    static void setSteamZero(uint16_t newZero);
    
    /**
     setSteamPSense: Updates the Steam PSense value
     - parameter newPSense: The value to update the Steam Psense to
     - returns: void
     - precondition: init() has been called
     */
    static void setSteamPSense(uint16_t newPSense);
    
};

static config_t eepromConfig EEMEM;

#endif /* EEPROM_hpp */
