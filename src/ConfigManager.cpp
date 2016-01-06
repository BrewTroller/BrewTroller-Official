//
//  EEPROM.cpp
//  BrewTroller-Xcode
//
//  Created by Eric Yanush on 2015-12-17.
//  Copyright © 2015 EricYanush. All rights reserved.
//

#include "ConfigManager.hpp"
#include <avr/eeprom.h>
#include "Enum.h"
#include "Config.h"

static config_t* configStore;

void ConfigManager::init(config_t *config) {
    configStore = config;
}

bool ConfigManager::configIsValid() {
    uint8_t storeFingerprint = eeprom_read_byte(&configStore->btFingerprint);
    uint8_t schemaVer = eeprom_read_byte(&configStore->eepromSchemaVersion);
    
    if (storeFingerprint != EEPROM_FINGERPRINT || schemaVer != EEPROM_SCHEMA_VER) {
        return false;
    }
    
    return true;
}

void ConfigManager::setBoilTemp(const uint8_t newBoilTemp) {
    eeprom_update_byte(&configStore->boilTemp, newBoilTemp);
}

uint8_t ConfigManager::getBoilTemp() {
    return eeprom_read_byte(&configStore->boilTemp);
}

void ConfigManager::setVolumeCalib(const uint8_t vessel, const uint8_t slot, const uint16_t data, const uint32_t volume) {
    
    uint16_t* dataDest;
    uint32_t* volDest;
    
    switch (vessel) {
        case VS_HLT:
            dataDest = &configStore->hltCalibDat[slot];
            volDest = &configStore->hltCalibVols[slot];
            break;
        case VS_MASH:
            dataDest = &configStore->mltCalibDat[slot];
            volDest = &configStore->mltCalibVols[slot];
            break;
        case VS_KETTLE:
            dataDest = &configStore->kettleCalibDat[slot];
            volDest = &configStore->kettleCalibVols[slot];
            break;
        default:
            return;
    }
    //TODO: Declare type for each of these instead of using hardcoded types here
    eeprom_update_block(&volume, volDest, sizeof(uint32_t));
    eeprom_update_block(&data, dataDest, sizeof(uint16_t));
}

void ConfigManager::setEvapRate(const uint8_t newEvapRate) {
    eeprom_update_byte(&configStore->evapRate, newEvapRate);
}

uint8_t ConfigManager::getEvapRate() {
    return eeprom_read_byte(&configStore->evapRate);
}

void ConfigManager::setPIDEnabled(uint8_t vessel, bool enabled) {
    uint8_t currSetting = eeprom_read_byte(&configStore->pidEnabledFlags);
    currSetting = enabled ? currSetting | (0x1 << vessel) : currSetting & ~(0x1 << vessel);
    eeprom_update_byte(&configStore->pidEnabledFlags, currSetting);
}

void ConfigManager::setPIDCycle(uint8_t vessel, uint8_t newCycleVal) {
    eeprom_update_byte(&configStore->pidConfigs[vessel].cycleTime, newCycleVal);
}

void ConfigManager::setPIDPGain(uint8_t vessel, uint8_t newPGain) {
    eeprom_update_byte(&configStore->pidConfigs[vessel].PGain, newPGain);
}

uint8_t ConfigManager::getPIDPGain(uint8_t vessel) {
    return eeprom_read_byte(&configStore->pidConfigs[vessel].PGain);
}

void ConfigManager::setPIDIGain(uint8_t vessel, uint8_t newIGain) {
    eeprom_update_byte(&configStore->pidConfigs[vessel].IGain, newIGain);
}

uint8_t ConfigManager::getPIDIGain(uint8_t vessel) {
    return eeprom_read_byte(&configStore->pidConfigs[vessel].IGain);
}

void ConfigManager::setPIDDGain(uint8_t vessel, uint8_t newDGain) {
    eeprom_update_byte(&configStore->pidConfigs[vessel].DGain, newDGain);
}

uint8_t ConfigManager::getPIDDGain(uint8_t vessel) {
    return eeprom_read_byte(&configStore->pidConfigs[vessel].DGain);
}

void ConfigManager::setHysteresis(uint8_t vessel, uint8_t newHysteresis) {
    eeprom_update_byte(&configStore->pidConfigs[vessel].hysteresis, newHysteresis);
}

void ConfigManager::setSteamTarget(uint8_t newTarget) {
    eeprom_update_byte(&configStore->steamTarget, newTarget);
}

uint8_t ConfigManager::getSteamTarget() {
    return eeprom_read_byte(&configStore->steamTarget);
}

void ConfigManager::setSteamZero(uint16_t newZero) {
    eeprom_update_block(&newZero, &configStore->steamZero, sizeof(config_t::steamZero));
}

void ConfigManager::setSteamPSense(uint16_t newPSense) {
    eeprom_update_block(&newPSense, &configStore->steamPSense, sizeof(config_t::steamPSense));
}

void ConfigManager::setProgramName(uint8_t program, char* newName) {
    //ensure that last character is a null
    newName[PROG_NAME_LEN - 1] = '\0';
    eeprom_update_block(newName, &configStore->programs[program].name, PROG_NAME_LEN);
}

void ConfigManager::getProgramName(uint8_t program, char *name) {
    eeprom_read_block(name, &configStore->programs[program].name, PROG_NAME_LEN);
}

void ConfigManager::setProgramMashStepTemp(uint8_t program, uint8_t step, uint8_t temperature) {
    eeprom_update_byte(&configStore->programs[program].mashStepTemp[step], temperature);
}

uint8_t ConfigManager::getProgramMashStepTemp(uint8_t program, uint8_t step) {
    return eeprom_read_byte(&configStore->programs[program].mashStepTemp[step]);
}

void ConfigManager::setProgramMashStepMins(uint8_t program, uint8_t step, uint8_t mins) {
    eeprom_update_byte(&configStore->programs[program].mashStepLength[step], mins);
}

uint8_t ConfigManager::getProgramMashStepMins(uint8_t program, uint8_t step) {
    return eeprom_read_byte(&configStore->programs[program].mashStepLength[step]);
}

void ConfigManager::setProgramBatchVol(uint8_t program, uint32_t newBatchVol) {
    eeprom_update_block(&newBatchVol, &configStore->programs[program].batchVolume, sizeof(uint32_t));
}

uint32_t ConfigManager::getProgramBatchVol(uint8_t program) {
    uint32_t temp;
    eeprom_read_block(&temp, &configStore->programs[program].batchVolume, sizeof(uint32_t));
    return temp;
}

void ConfigManager::setProgramMashRatio(uint8_t program, uint16_t newMashRatio) {
    eeprom_update_block(&newMashRatio, &configStore->programs[program].mashRatio, sizeof(uint16_t));
}

uint16_t ConfigManager::getProgramMashRatio(uint8_t program) {
    uint16_t temp;
    eeprom_read_block(&temp, &configStore->programs[program].mashRatio, sizeof(uint16_t));
    return temp;
}

void ConfigManager::setProgramGrainWeight(uint8_t program, uint32_t newGrainWeight) {
    eeprom_update_block(&newGrainWeight, &configStore->programs[program].grainWeight, sizeof(uint32_t));
}

uint32_t ConfigManager::getProgramGrainWeight(uint8_t program) {
    uint32_t temp;
    eeprom_read_block(&temp, &configStore->programs[program].grainWeight, sizeof(uint32_t));
    return temp;
}

void ConfigManager::loadConfig() {
    //Load the temperature sensor addresses
    extern uint8_t tSensor[8][9];
    eeprom_read_block(&tSensor, &configStore->tempSensorAddresses, sizeof(config_t::tempSensorAddresses));
    #ifdef HLT_AS_KETTLE
        eeprom_read_block(tSensor[TS_KETTLE], &configStore->tempSensorAddresses[TS_HLT], sizeof(TempSensorAddress));
    #elif defined KETTLE_AS_MASH
        eeprom_read_block(tSensor[TS_MASH], &configStore->tempSensorAddresses[TS_KETTLE], sizeof(TempSensorAddress));
    #elif defined SINGLE_VESSEL_SUPPORT
        eeprom_read_block(tSensor[TS_MASH], &configStore->tempSensorAddresses[TS_HLT], sizeof(TempSensorAddress));
        eeprom_read_block(tSensor[TS_KETTLE], &configStore->tempSensorAddresses[TS_HLT], sizeof(TempSensorAddress));
    #endif
    
    //Load the pid options
    extern bool PIDEnabled[VS_COUNT];
    extern uint8_t PIDCycle[VS_COUNT], hysteresis[VS_COUNT];
    uint8_t pidOpts = eeprom_read_byte(&configStore->pidEnabledFlags);
    
    for (uint8_t i = VS_HLT; i < VS_COUNT; i++) {
        PIDEnabled[i] = (pidOpts >> i) & 0x1;
        output_config curr;
        PIDCycle[i] = eeprom_read_byte(&configStore->pidConfigs[i].cycleTime);
        hysteresis[i] = eeprom_read_byte(&configStore->pidConfigs[i].hysteresis);
    }
    
    //Load boil power
    extern uint8_t boilPower;
    boilPower = eeprom_read_byte(&configStore->boilPower);
    
    //Load steam params
    extern uint16_t steamZero;
    extern uint16_t steamPSens;
    eeprom_read_block(&steamZero, &configStore->steamZero, sizeof(config_t::steamZero));
    eeprom_read_block(&steamPSens, &configStore->steamPSense, sizeof(config_t::steamPSense));
    
    //Load Volume configs
    extern uint32_t calibVols[3][10];
    extern uint16_t calibVals[3][10];
    eeprom_read_block(&calibVols[VS_HLT], &configStore->hltCalibVols, sizeof(VolumeCalibs));
    eeprom_read_block(&calibVals[VS_HLT], &configStore->hltCalibDat, sizeof(VolCalibData));
    
    #ifdef KETTLE_AS_MASH
        eeprom_read_block(&calibVols[VS_MASH], &configStore->kettleCalibVols, sizeof(VolumeCalibs));
        eeprom_read_block(&calibVals[VS_MASH], &configStore->kettleCalibDat, sizeof(VolCalibData));
    #elif defined SINGLE_VESSEL_SUPPORT
        eeprom_read_block(&calibVols[VS_MASH], &configStore->hltCalibVols, sizeof(VolumeCalibs));
        eeprom_read_block(&calibVals[VS_MASH], &configStore->hltCalibDat, sizeof(VolCalibData));
    #else
        eeprom_read_block(&calibVols[VS_MASH], &configStore->mltCalibVols, sizeof(VolumeCalibs));
        eeprom_read_block(&calibVals[VS_MASH], &configStore->mltCalibDat, sizeof(VolCalibData));
    #endif
    
    #ifdef HLT_AS_KETTLE
        eeprom_read_block(&calibVols[VS_KETTLE], &configStore->hltCalibVols, sizeof(VolumeCalibs));
        eeprom_read_block(&calibVals[VS_KETTLE], &configStore->hltCalibDat, sizeof(VolCalibData));
    #elif defined SINGLE_VESSEL_SUPPORT
        eeprom_read_block(&calibVols[VS_KETTLE], &configStore->hltCalibVols, sizeof(VolumeCalibs));
        eeprom_read_block(&calibVals[VS_KETTLE], &configStore->hltCalibDat, sizeof(VolCalibData));
    #else
        eeprom_read_block(&calibVols[VS_KETTLE], &configStore->kettleCalibVols, sizeof(VolumeCalibs));
        eeprom_read_block(&calibVals[VS_KETTLE], &configStore->kettleCalibDat, sizeof(VolCalibData));
    #endif
    
    //Load the setpoints
    extern double setpoint[4];
    setpoint[VS_HLT] = eeprom_read_byte(&configStore->hltSetPoint) * SETPOINT_MULT;
    setpoint[VS_MASH] = eeprom_read_byte(&configStore->mltSetPoint) * SETPOINT_MULT;
    setpoint[VS_KETTLE] = eeprom_read_byte(&configStore->kettleSetPoint) * SETPOINT_MULT;
    
    //Load the timers
    extern uint32_t timerValue[2];
    uint16_t temp;
    eeprom_read_block(&temp, &configStore->mashTimer, sizeof(config_t::mashTimer));
    timerValue[0] = temp * 60000; // I'm not sure why this actually needs to be multiplied by 60,000
    eeprom_read_block(&temp, &configStore->boilTimer, sizeof(config_t::boilTimer));
    timerValue[1] = temp * 60000;
    
    //Load timer status
    extern bool timerStatus[2];
    extern bool alarmStatus;
    extern uint32_t lastTime[2];
    uint8_t timerStatuses = eeprom_read_byte(&configStore->timerStatus);
    timerStatus[0] = timerStatuses & 0x1;
    lastTime[0] = 0; // Setup the last time stamps to 0;
    lastTime[1] = 0; // loadConfig should only be called @ startup, so we should be close to 0
                     //  initializing timer variables with millis() should definitely not be eeprom code's responsibility
    timerStatus[1] = (timerStatuses >> 1) & 0x1;
    alarmStatus = (timerStatuses >> 2) & 0x1;
    //TODO: move alarm status check/trigger somewhere else! In old eeprom code it was done here, it shouldn't be
    
    #ifdef PVOUT
        extern uint32_t vlvConfig[NUM_VLVCFGS];
        eeprom_read_block(&vlvConfig, &configStore->valveProfileCfg, sizeof(config_t::valveProfileCfg));
    #endif
    
    //TODO: Add modbus relay board config loading; it isn't done because for some reason the old eeprom board was deleteing the current configs and new'ing new ones, I'm not sure where or IF they were originally allocated.
}

void ConfigManager::setProgramSpargeTemp(uint8_t program, uint8_t newSpargeTemp) {
    eeprom_update_byte(&configStore->programs[program].spargeTemp, newSpargeTemp);
}

uint8_t ConfigManager::getProgramSpargeTemp(uint8_t program) {
    return eeprom_read_byte(&configStore->programs[program].spargeTemp);
}

void ConfigManager::setProgramHLTTemp(uint8_t program, uint8_t newHLTTemp) {
    eeprom_update_byte(&configStore->programs[program].hltTemperature, newHLTTemp);
}

uint8_t ConfigManager::getProgramHLTTemp(uint8_t program) {
    return eeprom_read_byte(&configStore->programs[program].hltTemperature);
}

void ConfigManager::setProgramPitchTemp(uint8_t program, uint8_t newPitchTemp) {
    eeprom_update_byte(&configStore->programs[program].pitchTemperature, newPitchTemp);
}

uint8_t ConfigManager::getProgramPitchTemp(uint8_t program) {
    return eeprom_read_byte(&configStore->programs[program].pitchTemperature);
}

void ConfigManager::setProgramBoilMins(uint8_t program, uint16_t mins) {
    eeprom_update_block(&mins, &configStore->programs[program].boilMins, sizeof(uint16_t));
}

uint16_t ConfigManager::getProgramBoilMins(uint8_t program) {
    uint16_t temp;
    eeprom_read_block(&temp, &configStore->programs[program].boilMins, sizeof(uint16_t));
    return temp;
}

void ConfigManager::setProgramMLHeatSource(uint8_t program, uint8_t vessel) {
    eeprom_update_byte(&configStore->programs[program].mashLiquorHeatSource, vessel);
}

uint8_t ConfigManager::getProgramMLHeatSource(uint8_t program) {
    return eeprom_read_byte(&configStore->programs[program].mashLiquorHeatSource);
}

void ConfigManager::setProgramBoilAdditionAlarms(uint8_t program, uint16_t flags) {
    eeprom_update_block(&flags, &configStore->programs[program].boilAdditionAlarms, sizeof(uint16_t));
}

uint16_t ConfigManager::getProgramBoilAdditionAlarms(uint8_t program) {
    uint16_t temp;
    eeprom_read_block(&temp, &configStore->programs[program].boilAdditionAlarms, sizeof(uint16_t));
    return temp;
}

void ConfigManager::setTempSensorAddress(uint8_t sensor, TempSensorAddress newAddr) {
    eeprom_update_block(newAddr, &configStore->tempSensorAddresses[sensor], sizeof(TempSensorAddress));
}

void ConfigManager::setVesselCapacity(uint8_t vessel, uint32_t newCapacity) {
    uint32_t* targetParam;
    switch (vessel) {
        case VS_HLT:
            targetParam = &configStore->hltCapacity;
            break;
        case VS_MASH:
            targetParam = &configStore->mltCapacity;
            break;
        case VS_KETTLE:
            targetParam = &configStore->kettleCapacity;
            break;
        default:
            return;
    }
    eeprom_update_block(&newCapacity, targetParam, sizeof(uint32_t));
}

uint32_t ConfigManager::getVesselCapacity(uint8_t vessel) {
    uint32_t* targetParam;
    switch (vessel) {
        case VS_HLT:
            targetParam = &configStore->hltCapacity;
            break;
        case VS_MASH:
            targetParam = &configStore->mltCapacity;
            break;
        case VS_KETTLE:
            targetParam = &configStore->kettleCapacity;
            break;
        default:
            return 0;
    }
    uint32_t temp;
    eeprom_read_block(&temp, targetParam, sizeof(uint32_t));
    return temp;
}

void ConfigManager::setVesselVolumeLoss(uint8_t vessel, uint16_t newLoss) {
    uint16_t* targetParam;
    switch (vessel) {
        case VS_HLT:
            targetParam = &configStore->hltVolLoss;
            break;
        case VS_MASH:
            targetParam = &configStore->mltVolLoss;
            break;
        case VS_KETTLE:
            targetParam = &configStore->kettleVolLoss;
            break;
        default:
            return;
    }
    eeprom_update_block(&newLoss, targetParam, sizeof(uint16_t));
}

uint16_t ConfigManager::getVesselVolumeLoss(uint8_t vessel) {
    uint16_t* targetParam;
    switch (vessel) {
        case VS_HLT:
            targetParam = &configStore->hltVolLoss;
            break;
        case VS_MASH:
            targetParam = &configStore->mltVolLoss;
            break;
        case VS_KETTLE:
            targetParam = &configStore->kettleVolLoss;
            break;
        default:
            return 0;
    }
    uint16_t temp;
    eeprom_read_block(&temp, targetParam, sizeof(uint16_t));
    return temp;
}

void ConfigManager::setValveProfileConfig(uint8_t profile, uint32_t newConfig){
    eeprom_update_block(&newConfig, &configStore->valveProfileCfg[profile], sizeof(uint32_t));
}

void ConfigManager::setVesselTempSetpoint(uint8_t vessel, uint8_t newSetpoint) {
    uint8_t* targetParam;
    switch (vessel) {
        case VS_HLT:
            targetParam = &configStore->hltSetPoint;
            break;
        case VS_MASH:
            targetParam = &configStore->mltSetPoint;
            break;
        case VS_KETTLE:
            targetParam = &configStore->kettleSetPoint;
            break;
        default:
            return;
    }
    eeprom_update_byte(targetParam, newSetpoint);
}

void ConfigManager::setBoilPower(uint8_t newBoilPower) {
    eeprom_update_byte(&configStore->boilPower, newBoilPower);
}

void ConfigManager::setDelayMins(uint16_t newDelayMins) {
    eeprom_update_block(&newDelayMins, &configStore->delayMins, sizeof(uint16_t));
}

uint16_t ConfigManager::getDelayMins() {
    uint16_t temp;
    eeprom_read_block(&temp, &configStore->delayMins, sizeof(uint16_t));
    return temp;
}

void ConfigManager::setGrainTemperature(uint8_t newGrainTemp) {
    eeprom_update_byte(&configStore->grainTemp, newGrainTemp);
}

uint8_t ConfigManager::getGrainTemperature() {
    return eeprom_read_byte(&configStore->grainTemp);
}

void ConfigManager::setTriggerPin(TriggerType trigger, uint8_t pinNum) {
    eeprom_update_byte(&configStore->triggerPins[trigger], pinNum);
}

uint8_t ConfigManager::getTriggerPin(TriggerType trigger) {
    return eeprom_read_byte(&configStore->triggerPins[trigger]);
}

void ConfigManager::initConfig() {
    //Zero the config structure out
    for (size_t i = 0; i < sizeof(config_t); i++) {
        eeprom_update_byte(((uint8_t*)configStore) + i, 0);
    }
    
    //Set the Defaults for PID configs
    for (uint8_t i = VS_HLT; i < VS_STEAM; i++) {
        ConfigManager::setPIDPGain(i, 3);
        ConfigManager::setPIDIGain(i, 4);
        ConfigManager::setPIDDGain(i, 2);
        ConfigManager::setPIDCycle(i, 4);
        ConfigManager::setHysteresis(i, 5);
    }
    
    //Mark the BT Fingerprint
    eeprom_update_byte(&configStore->btFingerprint, 252);
    
    //Set the defaults for Grain temperature and boil temperature
    setGrainTemperature(DEFAULT_GRAINTEMP);
    setBoilTemp(DEFAULT_BOILTEMP);
    
    //Set the default boil power
    setBoilPower(100);
    
    //Ensure all program threads are inactive
    for (uint8_t i = 0; i < PROGRAMTHREAD_MAX; i++) {
        eeprom_update_byte(&configStore->pgmThreads[i].activeStep, BREWSTEP_NONE);
        eeprom_update_byte(&configStore->pgmThreads[i].recipe, RECIPE_NONE);
    }
    
    //Set the default LCD brightness and contrast
    //  These should get refactored out into constants, probably into UI header files, but that's too much of a mess right now; They were/are defined in HWProfile.h, but are the same for every profile, so they probably shouldn't be there.
    eeprom_update_byte(&configStore->lcdBrightness, 255);
    eeprom_update_byte(&configStore->lcdContrast, 100);
}