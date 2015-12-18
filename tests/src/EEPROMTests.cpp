//
//  EEPROMTests.cpp
//  BrewTroller-Xcode
//
//  Created by Eric Yanush on 2015-12-17.
//  Copyright © 2015 EricYanush. All rights reserved.
//

#include "gtest/gtest.h"
#include "EEPROM.hpp"

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
    ASSERT_EQ(2048, offsetof(config_t, lcdBrightContrast));
    ASSERT_EQ(2050, offsetof(config_t, triggerPins));
    ASSERT_EQ(2065, offsetof(config_t, modbusBrdConf));
}