//
//  EEPROMTests.cpp
//  BrewTroller-Xcode
//
//  Created by Eric Yanush on 2015-12-17.
//  Copyright © 2015 EricYanush. All rights reserved.
//

#include "gtest/gtest.h"
#include "ConfigManager.hpp"
#include "Config.h"
#include <iostream>

class ConfigManagerTests : public ::testing::Test {
protected:
    virtual void SetUp() {
        ConfigManager::init(&conf);
    }
    
    config_t conf = {0};
    config_t untouched = {0};
    
    bool confIsInInitState() {
        return memcmp(&conf, &untouched, sizeof(config_t)) == 0;
    }
};

TEST(ProgramStruct, TestSize) {
    ASSERT_EQ(60, sizeof(program_config));
}

TEST(ProgramStruct, TestLayout) {
    ASSERT_EQ(0, offsetof(program_config, name));
    ASSERT_EQ(21, offsetof(program_config, spargeTemp));
    ASSERT_EQ(22, offsetof(program_config, boilMins));
    ASSERT_EQ(24, offsetof(program_config, mashRatio));
    ASSERT_EQ(26, offsetof(program_config, mashStepTemp));
    ASSERT_EQ(32, offsetof(program_config, mashStepLength));
    ASSERT_EQ(38, offsetof(program_config, batchVolume));
    ASSERT_EQ(42, offsetof(program_config, mashLiquorHeatSource));
    ASSERT_EQ(43, offsetof(program_config, hltTemperature));
    ASSERT_EQ(44, offsetof(program_config, pitchTemperature));
    ASSERT_EQ(45, offsetof(program_config, boilAdditionAlarms));
    ASSERT_EQ(47, offsetof(program_config, grainWeight));
}

TEST(MODBUSCfg, TestSize) {
    ASSERT_EQ(5, sizeof(modbusboard_cfg));
}

TEST(MODBUSCfg, TestLayout) {
    ASSERT_EQ(0, offsetof(modbusboard_cfg, address));
    ASSERT_EQ(1, offsetof(modbusboard_cfg, reg));
    ASSERT_EQ(3, offsetof(modbusboard_cfg, coilCount));
    ASSERT_EQ(4, offsetof(modbusboard_cfg, offset));
}

TEST(ConfigStruct, TestSize) {
    //We should have an overall size of 2075 currently, for backwards compatability
    ASSERT_EQ(2075, sizeof(config_t));
}

TEST(ConfigStruct, TestLayout) {
    ASSERT_EQ(0, offsetof(config_t, tempSensorAddresses));
    ASSERT_EQ(72, offsetof(config_t, pidEnabledFlags));
    ASSERT_EQ(73, offsetof(config_t, pidConfigs));
    ASSERT_EQ(93, offsetof(config_t, hltCapacity));
    ASSERT_EQ(97, offsetof(config_t, mltCapacity));
    ASSERT_EQ(101, offsetof(config_t, kettleCapacity));
    ASSERT_EQ(105, offsetof(config_t, hltVolLoss));
    ASSERT_EQ(107, offsetof(config_t, mltVolLoss));
    ASSERT_EQ(109, offsetof(config_t, kettleVolLoss));
    ASSERT_EQ(111, offsetof(config_t, boilTemp));
    ASSERT_EQ(112, offsetof(config_t, boilPower));
    ASSERT_EQ(113, offsetof(config_t, evapRate));
    ASSERT_EQ(114, offsetof(config_t, steamZero));
    ASSERT_EQ(116, offsetof(config_t, steamTarget));
    ASSERT_EQ(117, offsetof(config_t, steamPSense));
    ASSERT_EQ(119, offsetof(config_t, hltCalibVols));
    ASSERT_EQ(159, offsetof(config_t, mltCalibVols));
    ASSERT_EQ(199, offsetof(config_t, kettleCalibVols));
    ASSERT_EQ(239, offsetof(config_t, hltCalibDat));
    ASSERT_EQ(259, offsetof(config_t, mltCalibDat));
    ASSERT_EQ(279, offsetof(config_t, kettleCalibDat));
    ASSERT_EQ(299, offsetof(config_t, hltSetPoint));
    ASSERT_EQ(300, offsetof(config_t, mltSetPoint));
    ASSERT_EQ(301, offsetof(config_t, kettleSetPoint));
    ASSERT_EQ(302, offsetof(config_t, mashTimer));
    ASSERT_EQ(304, offsetof(config_t, boilTimer));
    ASSERT_EQ(306, offsetof(config_t, timerStatus));
    ASSERT_EQ(307, offsetof(config_t, boilAdditionsTrigger));
    ASSERT_EQ(313, offsetof(config_t, pgmThreads));
    ASSERT_EQ(398, offsetof(config_t, delayMins));
    ASSERT_EQ(400, offsetof(config_t, grainTemp));
    ASSERT_EQ(401, offsetof(config_t, valveProfileCfg));
    ASSERT_EQ(786, offsetof(config_t, programs));
    ASSERT_EQ(2046, offsetof(config_t, btFingerprint));
    ASSERT_EQ(2047, offsetof(config_t, eepromSchemaVersion));
    ASSERT_EQ(2048, offsetof(config_t, lcdBrightness));
    ASSERT_EQ(2049, offsetof(config_t, lcdContrast));
    ASSERT_EQ(2050, offsetof(config_t, triggerPins));
    ASSERT_EQ(2065, offsetof(config_t, modbusBrdConf));
}

TEST_F(ConfigManagerTests, TestConfigValidation) {
    //Fingerprint and schema match, should be true
    conf.btFingerprint = EEPROM_FINGERPRINT;
    conf.eepromSchemaVersion = EEPROM_SCHEMA_VER;
    ASSERT_TRUE(ConfigManager::configIsValid());
    
    //Fingerprint matches, schema doesn't, should be false
    conf.eepromSchemaVersion = EEPROM_SCHEMA_VER + 1;
    ASSERT_FALSE(ConfigManager::configIsValid());
    
    //fingerprint doesn't match, schema does, should be false
    conf.eepromSchemaVersion = EEPROM_SCHEMA_VER;
    conf.btFingerprint = EEPROM_FINGERPRINT + 1;
    ASSERT_FALSE(ConfigManager::configIsValid());

    //Fingerprint and schema both don't match, should be false
    conf.eepromSchemaVersion = EEPROM_SCHEMA_VER + 1;
    ASSERT_FALSE(ConfigManager::configIsValid());
}

TEST_F(ConfigManagerTests, TestGetBoilTemp) {
    conf.boilTemp = 12;
    ASSERT_EQ(12, ConfigManager::getBoilTemp());
}

TEST_F(ConfigManagerTests, SetBoilTemp) {
    conf.boilTemp = 21;
    ConfigManager::setBoilTemp(18);
    ASSERT_EQ(18, conf.boilTemp);
    
    //Ensure nothing else was touched!
    conf.boilTemp = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestSetVolumeCalibration) {
    for (uint8_t i = VS_HLT; i <= VS_KETTLE; i++) {
        for (uint8_t j = 0; j < VOL_CALIB_COUNT; j++) {
            ConfigManager::setVolumeCalib(i, j, 155, 88888888);
            switch (i) {
                case VS_HLT:
                    ASSERT_EQ(88888888, conf.hltCalibVols[j]);
                    ASSERT_EQ(155, conf.hltCalibDat[j]);
                    conf.hltCalibDat[j] = 0;
                    conf.hltCalibVols[j] = 0;
                    break;
                case VS_MASH:
                    ASSERT_EQ(88888888, conf.mltCalibVols[j]);
                    ASSERT_EQ(155, conf.mltCalibDat[j]);
                    conf.mltCalibDat[j] = 0;
                    conf.mltCalibVols[j] = 0;
                    break;
                case VS_KETTLE:
                    ASSERT_EQ(88888888, conf.kettleCalibVols[j]);
                    ASSERT_EQ(155, conf.kettleCalibDat[j]);
                    conf.kettleCalibDat[j] = 0;
                    conf.kettleCalibVols[j] = 0;
                    break;
            }
            //Make sure nothing else was touched
            ASSERT_TRUE(confIsInInitState());
        }
    }
}

TEST_F(ConfigManagerTests, TestSetEvapRate) {
    ConfigManager::setEvapRate(52);
    ASSERT_EQ(52, conf.evapRate);
    
    conf.evapRate = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestGetEvapRate) {
    conf.evapRate = 85;
    ASSERT_EQ(85, ConfigManager::getEvapRate());
}

TEST_F(ConfigManagerTests, TestSetPIDEnabled) {
    for(uint8_t i = 0; i < VS_KETTLE; i++) {
        ConfigManager::setPIDEnabled(i, true);
        ASSERT_EQ(0x1 << i, conf.pidEnabledFlags);
        ConfigManager::setPIDEnabled(i, false);
        ASSERT_EQ(0, conf.pidEnabledFlags);
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestSetPIDCycle) {
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setPIDCycle(i, 111);
        ASSERT_EQ(111, conf.pidConfigs[i].cycleTime);
        conf.pidConfigs[i].cycleTime = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestSetPIDPGain) {
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setPIDPGain(i, 123);
        ASSERT_EQ(123, conf.pidConfigs[i].PGain);
        conf.pidConfigs[i].PGain = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestSetPIDIGain) {
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setPIDIGain(i, 234);
        ASSERT_EQ(234, conf.pidConfigs[i].IGain);
        conf.pidConfigs[i].IGain = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestSetPIDDGain) {
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setPIDDGain(i, 189);
        ASSERT_EQ(189, conf.pidConfigs[i].DGain);
        conf.pidConfigs[i].DGain = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetPIDPGain) {
    conf.pidConfigs[VS_HLT].PGain = 111;
    conf.pidConfigs[VS_KETTLE].PGain = 88;
    ASSERT_EQ(111, ConfigManager::getPIDPGain(VS_HLT));
    ASSERT_EQ(88, ConfigManager::getPIDPGain(VS_KETTLE));
}

TEST_F(ConfigManagerTests, TestGetPIDIGain) {
    conf.pidConfigs[VS_HLT].IGain = 111;
    conf.pidConfigs[VS_KETTLE].IGain = 88;
    ASSERT_EQ(111, ConfigManager::getPIDIGain(VS_HLT));
    ASSERT_EQ(88, ConfigManager::getPIDIGain(VS_KETTLE));
}

TEST_F(ConfigManagerTests, TestGetPIDDGain) {
    conf.pidConfigs[VS_HLT].DGain = 111;
    conf.pidConfigs[VS_KETTLE].DGain = 88;
    ASSERT_EQ(111, ConfigManager::getPIDDGain(VS_HLT));
    ASSERT_EQ(88, ConfigManager::getPIDDGain(VS_KETTLE));
}

TEST_F(ConfigManagerTests, TestSetHysteresis) {
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setHysteresis(i, 101);
        ASSERT_EQ(101, conf.pidConfigs[i].hysteresis);
        conf.pidConfigs[i].hysteresis = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestSetSteamTarget) {
    ConfigManager::setSteamTarget(14);
    ASSERT_EQ(14, conf.steamTarget);
    
    conf.steamTarget = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestGetSteamTarget) {
    conf.steamTarget = 77;
    ASSERT_EQ(77, ConfigManager::getSteamTarget());
}

TEST_F(ConfigManagerTests, TestSetSteamZero) {
    ConfigManager::setSteamZero(5123);
    ASSERT_EQ(5123, conf.steamZero);
    
    conf.steamZero = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestSetSteamPSense) {
    ConfigManager::init(&conf);
    
    ConfigManager::setSteamPSense(7654);
    ASSERT_EQ(7654, conf.steamPSense);
    
    conf.steamPSense = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestSetProgramName) {
    char nameZero[20] = {0};
    char nameTest[20] = "Test Name      1234";
    ASSERT_EQ(PROG_NAME_LEN, sizeof(nameTest));
    ASSERT_EQ('\0', nameTest[19]);
    
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramName(i, nameTest);
        ASSERT_EQ(0, memcmp(&conf.programs[i].name, &nameTest, sizeof(nameTest)));
        
        //Zero the name back out and ensure nothing else was modified
        memcpy(&conf.programs[i].name, nameZero, sizeof(nameZero));
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramName) {
    char nameTest[20] = "Test Name Test Name";
    ASSERT_EQ(PROG_NAME_LEN, sizeof(nameTest));
    ASSERT_EQ('\0', nameTest[19]);
    
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        char temp[20] = {0};
        memcpy(conf.programs[i].name, nameTest, sizeof(nameTest));
        ConfigManager::getProgramName(i, temp);
        ASSERT_EQ(0, memcmp(temp, nameTest, sizeof(nameTest)));
    }
}

TEST_F(ConfigManagerTests, TestSetProgramMashTemp) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        for (uint8_t j = 0; j < MASHSTEP_COUNT; j++) {
            ConfigManager::setProgramMashStepTemp(i, j, 21);
            ASSERT_EQ(21, conf.programs[i].mashStepTemp[j]);
            conf.programs[i].mashStepTemp[j] = 0;
            ASSERT_TRUE(confIsInInitState());
        }
    }
}

TEST_F(ConfigManagerTests, TestGetProgramMashTemp) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        for (uint8_t j = 0; j < MASHSTEP_COUNT; j++) {
            conf.programs[i].mashStepTemp[j] = 21;
            ASSERT_EQ(21, ConfigManager::getProgramMashStepTemp(i, j));
            conf.programs[i].mashStepTemp[j] = 0;
        }
    }
}

TEST_F(ConfigManagerTests, TestSetProgramMashMins) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        for (uint8_t j = 0; j < MASHSTEP_COUNT; j++) {
            ConfigManager::setProgramMashStepMins(i, j, 77);
            ASSERT_EQ(77, conf.programs[i].mashStepLength[j]);
            conf.programs[i].mashStepLength[j] = 0;
            ASSERT_TRUE(confIsInInitState());
        }
    }
}

TEST_F(ConfigManagerTests, TestGetProgramMashMins) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        for (uint8_t j = 0; j < MASHSTEP_COUNT; j++) {
            conf.programs[i].mashStepLength[j] = 66;
            ASSERT_EQ(66, ConfigManager::getProgramMashStepMins(i, j));
            conf.programs[i].mashStepLength[j] = 0;
        }
    }
}

TEST_F(ConfigManagerTests, TestSetProgramBatchVol) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramBatchVol(i, 0xFEEDBEEF);
        ASSERT_EQ(0xFEEDBEEF, conf.programs[i].batchVolume);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].batchVolume = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramBatchVol) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].batchVolume = 0xABCDEF81;
        ASSERT_EQ(0xABCDEF81, ConfigManager::getProgramBatchVol(i));
        conf.programs[i].batchVolume = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetProgramMashRatio) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramMashRatio(i, 0xABCD);
        ASSERT_EQ(0xABCD, conf.programs[i].mashRatio);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].mashRatio = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramMashRatio) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].mashRatio = 0xFF88;
        ASSERT_EQ(0xFF88, ConfigManager::getProgramMashRatio(i));
        conf.programs[i].mashRatio = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetProgramGrainWeight) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramGrainWeight(i, 0xFEEDBEEF);
        ASSERT_EQ(0xFEEDBEEF, conf.programs[i].grainWeight);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].grainWeight = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramGrainWeight) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].grainWeight = 0xABCDEF81;
        ASSERT_EQ(0xABCDEF81, ConfigManager::getProgramGrainWeight(i));
        conf.programs[i].grainWeight = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetProgramSpargeTemp) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramSpargeTemp(i, 177);
        ASSERT_EQ(177, conf.programs[i].spargeTemp);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].spargeTemp = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramSpargeTemp) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].spargeTemp = 88;
        ASSERT_EQ(88, ConfigManager::getProgramSpargeTemp(i));
        conf.programs[i].spargeTemp = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetProgramHLTTemp) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramHLTTemp(i, 177);
        ASSERT_EQ(177, conf.programs[i].hltTemperature);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].hltTemperature = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramHLTTemp) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].hltTemperature = 88;
        ASSERT_EQ(88, ConfigManager::getProgramHLTTemp(i));
        conf.programs[i].hltTemperature = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetProgramPitchTemp) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramPitchTemp(i, 177);
        ASSERT_EQ(177, conf.programs[i].pitchTemperature);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].pitchTemperature = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramPitchTemp) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].pitchTemperature = 88;
        ASSERT_EQ(88, ConfigManager::getProgramPitchTemp(i));
        conf.programs[i].pitchTemperature = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetProgramBoilMins) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramBoilMins(i, 0x1234);
        ASSERT_EQ(0x1234, conf.programs[i].boilMins);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].boilMins = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramBoilMins) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].boilMins = 0x4466;
        ASSERT_EQ(0x4466, ConfigManager::getProgramBoilMins(i));
        conf.programs[i].boilMins = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetProgramMLHeatSource) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramMLHeatSource(i, VS_MASH);
        ASSERT_EQ(VS_MASH, conf.programs[i].mashLiquorHeatSource);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].mashLiquorHeatSource = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramMLHeatSource) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].mashLiquorHeatSource = VS_KETTLE;
        ASSERT_EQ(VS_KETTLE, ConfigManager::getProgramMLHeatSource(i));
        conf.programs[i].mashLiquorHeatSource = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetProgramBoilAdditonAlarms) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        ConfigManager::setProgramBoilAdditionAlarms(i, 0x1234);
        ASSERT_EQ(0x1234, conf.programs[i].boilAdditionAlarms);
        
        //Zero the change back out and ensure nothing else was modified
        conf.programs[i].boilAdditionAlarms = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramBoilAdditionAlarms) {
    for (uint8_t i = 0; i < RECIPE_MAX; i++) {
        conf.programs[i].boilAdditionAlarms = 0x4466;
        ASSERT_EQ(0x4466, ConfigManager::getProgramBoilAdditionAlarms(i));
        conf.programs[i].boilAdditionAlarms = 0;
    }
}

TEST_F(ConfigManagerTests, TestSetTempSensorAddress) {
    TempSensorAddress addrZero = {0};
    TempSensorAddress testAddr = {5, 10, 15, 20,
                                  25, 30, 35, 40};
    
    for (uint8_t i = 0; i < NUM_TS; i++) {
        ConfigManager::setTempSensorAddress(i, testAddr);
        ASSERT_EQ(0, memcmp(testAddr, conf.tempSensorAddresses[i], sizeof(TempSensorAddress)));
        
        //Zero the change back out and ensure nothing else was modified
        memcpy(conf.tempSensorAddresses[i], addrZero, sizeof(TempSensorAddress));
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestSetVesselCapacity) {
    //This is only defined for HLT, MASH and KETTLE!!!
    ConfigManager::setVesselCapacity(VS_HLT, 0xFEEDBEEF);
    ASSERT_EQ(0xFEEDBEEF, conf.hltCapacity);
    //Zero the change back out and ensure nothing else was modified
    conf.hltCapacity = 0;
    ASSERT_TRUE(confIsInInitState());
    ConfigManager::setVesselCapacity(VS_MASH, 0xFEEDBEEF);
    ASSERT_EQ(0xFEEDBEEF, conf.mltCapacity);
    //Zero the change back out and ensure nothing else was modified
    conf.mltCapacity = 0;
    ASSERT_TRUE(confIsInInitState());
    ConfigManager::setVesselCapacity(VS_KETTLE, 0xFEEDBEEF);
    ASSERT_EQ(0xFEEDBEEF, conf.kettleCapacity);
    //Zero the change back out and ensure nothing else was modified
    conf.kettleCapacity = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestGetVesselCapacity) {
    //This is only defined for HLT, MASH, and  KETTLE!!!!
    conf.hltCapacity = 0x12344321;
    ASSERT_EQ(0x12344321, ConfigManager::getVesselCapacity(VS_HLT));
    conf.hltCapacity = 0;
    conf.mltCapacity = 0x12344321;
    ASSERT_EQ(0x12344321, ConfigManager::getVesselCapacity(VS_MASH));
    conf.mltCapacity = 0;
    conf.kettleCapacity = 0x12344321;
    ASSERT_EQ(0x12344321, ConfigManager::getVesselCapacity(VS_KETTLE));
}

TEST_F(ConfigManagerTests, TestSetVesselVolumeLoss) {
    //This is only defined for HLT, MASH and KETTLE!!!
    ConfigManager::setVesselVolumeLoss(VS_HLT, 0xBEEF);
    ASSERT_EQ(0xBEEF, conf.hltVolLoss);
    //Zero the change back out and ensure nothing else was modified
    conf.hltVolLoss = 0;
    ASSERT_TRUE(confIsInInitState());
    ConfigManager::setVesselVolumeLoss(VS_MASH, 0xBEEF);
    ASSERT_EQ(0xBEEF, conf.mltVolLoss);
    //Zero the change back out and ensure nothing else was modified
    conf.mltVolLoss = 0;
    ASSERT_TRUE(confIsInInitState());
    ConfigManager::setVesselVolumeLoss(VS_KETTLE, 0xBEEF);
    ASSERT_EQ(0xBEEF, conf.kettleVolLoss);
    //Zero the change back out and ensure nothing else was modified
    conf.kettleVolLoss = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestGetVesselVolumeLoss) {
    //This is only defined for HLT, MASH, and  KETTLE!!!!
    conf.hltVolLoss = 0x4321;
    ASSERT_EQ(0x4321, ConfigManager::getVesselVolumeLoss(VS_HLT));
    conf.hltVolLoss = 0;
    conf.mltVolLoss = 0x4321;
    ASSERT_EQ(0x4321, ConfigManager::getVesselVolumeLoss(VS_MASH));
    conf.mltVolLoss = 0;
    conf.kettleVolLoss = 0x4321;
    ASSERT_EQ(0x4321, ConfigManager::getVesselVolumeLoss(VS_KETTLE));
}

TEST_F(ConfigManagerTests, TestSetValveProfileConfig) {
    for (uint8_t i = 0; i < NUM_VLVCFGS; i++) {
        ConfigManager::setValveProfileConfig(i, 0xAABBCCDD);
        ASSERT_EQ(0xAABBCCDD, conf.valveProfileCfg[i]);
        conf.valveProfileCfg[i] = 0;
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestSetVesselTempSetpoint) {
    //This is only defined for HLT, MASH and KETTLE!!!
    ConfigManager::setVesselTempSetpoint(VS_HLT, 121);
    ASSERT_EQ(121, conf.hltSetPoint);
    //Zero the change back out and ensure nothing else was modified
    conf.hltSetPoint = 0;
    ASSERT_TRUE(confIsInInitState());
    ConfigManager::setVesselTempSetpoint(VS_MASH, 121);
    ASSERT_EQ(121, conf.mltSetPoint);
    //Zero the change back out and ensure nothing else was modified
    conf.mltSetPoint = 0;
    ASSERT_TRUE(confIsInInitState());
    ConfigManager::setVesselTempSetpoint(VS_KETTLE, 121);
    ASSERT_EQ(121, conf.kettleSetPoint);
    //Zero the change back out and ensure nothing else was modified
    conf.kettleSetPoint = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestSetBoilPower) {
    ConfigManager::setBoilPower(69);
    ASSERT_EQ(69, conf.boilPower);
    
    //Zero the change back out and ensure nothing else was modified
    conf.boilPower = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestSetDelayMins) {
    ConfigManager::setDelayMins(0xABBA);
    ASSERT_EQ(0xABBA, conf.delayMins);
    
    conf.delayMins = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestGetDelayMins) {
    conf.delayMins = 0xBADA;
    ASSERT_EQ(0xBADA, ConfigManager::getDelayMins());
}

TEST_F(ConfigManagerTests, TestSetGrainTemperature) {
    ConfigManager::setGrainTemperature(199);
    ASSERT_EQ(199, conf.grainTemp);
    
    conf.grainTemp = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestGetGrainTemperature) {
    conf.grainTemp = 55;
    ASSERT_EQ(55, ConfigManager::getGrainTemperature());
}

TEST_F(ConfigManagerTests, TestSetTriggerPin) {
for (uint8_t i = 0; i < NUM_TRIGGERS; i++) {
        ConfigManager::setTriggerPin((TriggerType)i, 8);
        ASSERT_EQ(8, conf.triggerPins[i]);
        conf.triggerPins[i] = 0;
    }
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestGetTriggerPin) {
for (uint8_t i = 0; i < NUM_TRIGGERS; i++) {
        conf.triggerPins[i] = 21;
        ASSERT_EQ(21, ConfigManager::getTriggerPin((TriggerType)i));
        conf.triggerPins[i] = 0;
    }
}

TEST_F(ConfigManagerTests, SetProgramThread) {
    const ProgramThread zeroThread{0};
    ProgramThread testThread;
    testThread.activeStep = MASHSTEP_MASHOUT;
    testThread.recipe = 5;
    
    for (uint8_t i = 0; i <= PROGRAMTHREAD_MAX; i++) {
        ConfigManager::setProgramThread(i, &testThread);
        ASSERT_EQ(0, memcmp(&testThread, &conf.pgmThreads[i], sizeof(ProgramThread)));
        memcpy(&conf.pgmThreads[i], &zeroThread, sizeof(ProgramThread));
        ASSERT_TRUE(confIsInInitState());
    }
}

TEST_F(ConfigManagerTests, TestGetProgramThread) {
    ProgramThread zeroThread{0};
    ProgramThread testThread;
    testThread.activeStep = MASHSTEP_MASHOUT;
    testThread.recipe = 5;
    
    for (uint8_t i = 0; i <= PROGRAMTHREAD_MAX; i++) {
        conf.pgmThreads[i] = testThread;
        ProgramThread ret;
        ConfigManager::getProgramThread(i, &ret);
        ASSERT_EQ(0, memcmp(&testThread, &ret, sizeof(ProgramThread)));
        memcpy(&conf.pgmThreads[i], &zeroThread, sizeof(ProgramThread));
    }
}

TEST_F(ConfigManagerTests, TestGetBoilAdditionsTrigger) {
    conf.boilAdditionsTrigger = 0xF891;
    ASSERT_EQ(0xF891, ConfigManager::getBoilAdditionsTrigger());
}

TEST_F(ConfigManagerTests, TestSetBoilAdditionsTrigger) {
    ConfigManager::setBoilAdditionsTrigger(0xABCD);
    ASSERT_EQ(0xABCD, conf.boilAdditionsTrigger);
    conf.boilAdditionsTrigger = 0;
    ASSERT_TRUE(confIsInInitState());
}

TEST_F(ConfigManagerTests, TestConfigInit) {
    ConfigManager::initConfig();
    //Test the PID, Cycle, and Hysteresis Values for each vessel are set to defaults
    for (uint8_t i = 0; i <= VS_KETTLE; i++) {
        ASSERT_EQ(3, conf.pidConfigs[i].PGain);
        conf.pidConfigs[i].PGain = 0;
        ASSERT_EQ(4, conf.pidConfigs[i].IGain);
        conf.pidConfigs[i].IGain = 0;
        ASSERT_EQ(2, conf.pidConfigs[i].DGain);
        conf.pidConfigs[i].DGain = 0;
        ASSERT_EQ(4, conf.pidConfigs[i].cycleTime);
        conf.pidConfigs[i].cycleTime = 0;
        ASSERT_EQ(5, conf.pidConfigs[i].hysteresis);
        conf.pidConfigs[i].hysteresis = 0;
    }
    
    //Test default Grain temp
    ASSERT_EQ(DEFAULT_GRAINTEMP, conf.grainTemp);
    conf.grainTemp = 0;
    
    //Test default boil temp
    ASSERT_EQ(DEFAULT_BOILTEMP, conf.boilTemp);
    conf.boilTemp = 0;
    
    //Test default boil power
    ASSERT_EQ(100, conf.boilPower);
    conf.boilPower = 0;
    
    //Test all program threads have been set to idle
    for (uint8_t i = 0; i < PROGRAMTHREAD_MAX; i++) {
        ASSERT_EQ(BREWSTEP_NONE, conf.pgmThreads[i].activeStep);
        conf.pgmThreads[i].activeStep = 0;
        ASSERT_EQ(RECIPE_NONE, conf.pgmThreads[i].recipe);
        conf.pgmThreads[i].recipe = 0;
    }
    
    //Test the lcd brightness and contrast are set to default values
    ASSERT_EQ(255, conf.lcdBrightness);
    conf.lcdBrightness = 0;
    ASSERT_EQ(100, conf.lcdContrast);
    conf.lcdContrast = 0;
    
    //Test the config version number
    ASSERT_EQ(0, conf.eepromSchemaVersion);
    
    //Test the Fingerprint has been set
    ASSERT_EQ(252, conf.btFingerprint);
    conf.btFingerprint = 0;
    
    //Ensure that init has zeroed out the rest of the config structure
    //  We don't care about any data that lies outside of the config structure, as we aren't using it
    ASSERT_TRUE(confIsInInitState());
}
