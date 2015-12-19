//
//  globals.cpp
//  BrewTroller
//
//  Created by Eric Yanush on 2015-12-18.
//  Copyright © 2015 EricYanush. All rights reserved.
//

#include <stdint.h>

/**
 Globals: This file is a hack to make the test suite compile inspite of all the global vars defined in the target code.
 - Note: This header will no longer be needed when all the globals have been cleaned out of the target code.
 */

uint8_t tSensor[9][8];
bool PIDEnabled[4];
uint8_t PIDCycle[4], hysteresis[4];
uint8_t boilPower;
uint16_t steamZero;
uint16_t steamPSens;
uint32_t calibVols[3][10];
uint16_t calibVals[3][10];
double setpoint[4];
uint32_t timerValue[2];
bool timerStatus[2];
bool alarmStatus;
uint32_t lastTime[2];