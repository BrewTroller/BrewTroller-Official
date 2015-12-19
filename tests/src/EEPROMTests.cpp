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

TEST(ConfigManager, TestConfigValidation) {
    config_t conf;
    ConfigManager::init(&conf);
    
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

TEST(ConfigManager, TestGetBoilTemp) {
    config_t conf;
    ConfigManager::init(&conf);
    
    conf.boilTemp = 12;
    ASSERT_EQ(12, ConfigManager::getBoilTemp());
}

TEST(ConfigManager, SetBoilTemp) {
    config_t conf = {0};
    config_t confUntouched = {0};
    
    ConfigManager::init(&conf);
    
    conf.boilTemp = 21;
    ConfigManager::setBoilTemp(18);
    ASSERT_EQ(18, conf.boilTemp);
    
    //Ensure nothing else was touched!
    conf.boilTemp = confUntouched.boilTemp;
    ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
}

TEST(ConfigManager, TestSetVolumeCalibration) {
    config_t conf = {0};
    config_t confUntouched = {0};
    
    ConfigManager::init(&conf);
    
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
            ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
        }
    }
}

TEST(ConfigManager, TestSetEvapRate) {
    config_t conf = {0};
    config_t confUntouched = {0};
    
    ConfigManager::init(&conf);
    
    ConfigManager::setEvapRate(52);
    ASSERT_EQ(52, conf.evapRate);
    
    conf.evapRate = 0;
    ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
}

TEST(ConfigManager, TestGetEvapRate) {
    config_t conf;
    ConfigManager::init(&conf);
    
    conf.evapRate = 85;
    ASSERT_EQ(85, ConfigManager::getEvapRate());
}

TEST(ConfigManager, TestSetPIDEnabled) {
    config_t conf = {0};
    config_t confUntouched = {0};
    ConfigManager::init(&conf);
    
    for(uint8_t i = 0; i < VS_KETTLE; i++) {
        ConfigManager::setPIDEnabled(i, true);
        ASSERT_EQ(0x1 << i, conf.pidEnabledFlags);
        ConfigManager::setPIDEnabled(i, false);
        ASSERT_EQ(0, conf.pidEnabledFlags);
        ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
    }
}

TEST(ConfigManager, TestSetPIDCycle) {
    config_t conf = {0};
    config_t confUntouched = {0};
    ConfigManager::init(&conf);
    
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setPIDCycle(i, 111);
        ASSERT_EQ(111, conf.pidConfigs[i].cycleTime);
        conf.pidConfigs[i].cycleTime = 0;
        ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
    }
}

TEST(ConfigManager, TestSetPIDPGain) {
    config_t conf = {0};
    config_t confUntouched = {0};
    ConfigManager::init(&conf);
    
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setPIDPGain(i, 123);
        ASSERT_EQ(123, conf.pidConfigs[i].PGain);
        conf.pidConfigs[i].PGain = 0;
        ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
    }
}

TEST(ConfigManager, TestSetPIDIGain) {
    config_t conf = {0};
    config_t confUntouched = {0};
    ConfigManager::init(&conf);
    
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setPIDIGain(i, 234);
        ASSERT_EQ(234, conf.pidConfigs[i].IGain);
        conf.pidConfigs[i].IGain = 0;
        ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
    }
}

TEST(ConfigManager, TestSetPIDDGain) {
    config_t conf = {0};
    config_t confUntouched = {0};
    ConfigManager::init(&conf);
    
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setPIDDGain(i, 189);
        ASSERT_EQ(189, conf.pidConfigs[i].DGain);
        conf.pidConfigs[i].DGain = 0;
        ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
    }
}

TEST(ConfigManager, TestSetHysteresis) {
    config_t conf = {0};
    config_t confUntouched = {0};
    ConfigManager::init(&conf);
    
    for (uint8_t i = 0; i < VS_COUNT; i++) {
        ConfigManager::setHysteresis(i, 101);
        ASSERT_EQ(101, conf.pidConfigs[i].hysteresis);
        conf.pidConfigs[i].hysteresis = 0;
        ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
    }
}

TEST(ConfigManager, TestSetSteamTarget) {
    config_t conf = {0};
    config_t confUntouched = {0};
    
    ConfigManager::init(&conf);
    
    ConfigManager::setSteamTarget(14);
    ASSERT_EQ(14, conf.steamTarget);
    
    conf.steamTarget = 0;
    ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
}

TEST(ConfigManager, TestSetSteamZero) {
    config_t conf = {0};
    config_t confUntouched = {0};
    
    ConfigManager::init(&conf);
    
    ConfigManager::setSteamZero(5123);
    ASSERT_EQ(5123, conf.steamZero);
    
    conf.steamZero = 0;
    ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
}

TEST(ConfigManager, TestSetSteamPSense) {
    config_t conf = {0};
    config_t confUntouched = {0};
    
    ConfigManager::init(&conf);
    
    ConfigManager::setSteamPSense(7654);
    ASSERT_EQ(7654, conf.steamPSense);
    
    conf.steamPSense = 0;
    ASSERT_EQ(0, memcmp(&conf, &confUntouched, sizeof(config_t)));
}