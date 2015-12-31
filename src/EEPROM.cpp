/*  
   Copyright (C) 2009, 2010 Matt Reba, Jeremiah Dillingham

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
Software Lead: Matt Reba (matt_AT_brewtroller_DOT_com)
Hardware Lead: Jeremiah Dillingham (jeremiah_AT_brewtroller_DOT_com)

Documentation, Forums and more information available at http://www.brewtroller.com
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <avr/eeprom.h>
#include "BrewTroller.h"
#include "Events.h"
#include "EEPROM.h"
#include "HardwareProfile.h"

void loadSetup() {
  //**********************************************************************************
  //TSensors: HLT (0-7), MASH (8-15), KETTLE (16-23), H2OIN (24-31), H2OOUT (32-39),
  //          BEEROUT (40-47), AUX1 (48-55), AUX2 (56-63), AUX3 (64-71)
  //**********************************************************************************
  //Vessel sensors are read in here but aren't used.
	EEPROMreadBytes(0, *tSensor, 72);
  
 
  //**********************************************************************************
  //PID Enabled (72); Bit 1 = HLT, Bit 2 = Mash, Bit 3 = Kettle, Bit 4 = Steam
  //PIDp HLT (73), Mash (78), Kettle (83), Steam (88)
  //PIDi HLT (74), Mash (79), Kettle (84), Steam (89)
  //PIDd HLT (75), Mash (80), Kettle (85), Steam (90)
  //PIDCycle HLT (76), Mash (81), Kettle (86), Steam (91)
  //Hysteresis HLT (77), Mash (82), Kettle (87), Steam (92)
  //**********************************************************************************
	//Loaded directly by vessels  
  
  //**********************************************************************************
  //boilPwr (112)
  //**********************************************************************************
  boilPwr = EEPROM.read(112);
  //**********************************************************************************
  //steamZero (114)
  //**********************************************************************************
  //Loaded by vessels
  //**********************************************************************************
  //steamPSens (117-118)
  //**********************************************************************************

  //**********************************************************************************
  //calibVols HLT (119-158), Mash (159-198), Kettle (199-238)
  //calibVals HLT (239-258), Mash (259-278), Kettle (279-298)
  //**********************************************************************************
	//Loaded directly by vessels

  //**********************************************************************************
  //setpoints (299-301)
  //**********************************************************************************
  for (byte i=VS_HLT; i<=NUM_VESSELS; i++) { //TODO: Make this operate only for the number of vessels, not always for all 3 vessels
	  //TODO: why is EEPROM code setting an event handler? This should be somewhere else.
    //Setting loaded directly by vessels
    eventHandler(EVENT_SETPOINT, i);
  }
  
  
  //**********************************************************************************
  //timers (302-305)
  //**********************************************************************************
 
  for (byte i=TIMER_MASH; i<=TIMER_BOIL; i++) { timerValue[i] = EEPROMreadInt(302 + i * 2) * 60000; }

  //**********************************************************************************
  //Timer/Alarm Status (306)
  //**********************************************************************************
  byte options = EEPROM.read(306);
  for (byte i = TIMER_MASH; i <= TIMER_BOIL; i++) {
    timerStatus[i] = bitRead(options, i);
    lastTime[i] = millis();
  }
  alarmStatus = bitRead(options, 2);
#ifdef ALARM_PIN
  alarmPin.set(alarmStatus);
#endif
  #ifdef DEBUG_TIMERALARM
    logStart_P(LOGDEBUG);
    logField("TimerAlarmStatus");
    logFieldI(bitRead(options, 0));
    logFieldI(bitRead(options, 1));
    logFieldI(bitRead(options, 2));
    logEnd();
  #endif
  


  //**********************************************************************************
  //401-480 Valve Profiles
  //**********************************************************************************
  #ifdef PVOUT
    loadVlvConfigs();
  
    #ifdef PVOUT_TYPE_MODBUS
      for (byte i = 0; i < PVOUT_MODBUS_MAXBOARDS; i++) 
        loadVlvModbus(i);
    #endif
  #endif
}

#ifdef PVOUT
  void loadVlvConfigs() {
    eeprom_read_block(&vlvConfig, (unsigned char *) 401, 80);
  }
  
  #ifdef PVOUT_TYPE_MODBUS
    void loadVlvModbus(byte board) {
      if (ValvesMB[board]) {
        delete ValvesMB[board];
        ValvesMB[board] = NULL;
      }
      byte addr = getVlvModbusAddr(board);
      if (addr != PVOUT_MODBUS_ADDRNONE)
        ValvesMB[board] = new PVOutMODBUS(addr, getVlvModbusReg(board), getVlvModbusCoilCount(board), getVlvModbusOffset(board));
    }
  #endif 
#endif

//*****************************************************************************************************************************
// Individual EEPROM Get/Set Variable Functions
//*****************************************************************************************************************************

//**********************************************************************************
//TSensors: HLT (0-7), MASH (8-15), KETTLE (16-23), H2OIN (24-31), H2OOUT (32-39), 
//          BEEROUT (40-47), AUX1 (48-55), AUX2 (56-63), AUX3 (64-71)
//**********************************************************************************
void setTSAddr(byte sensor, byte addr[8]) {
	if (sensor < NUM_VESSELS)
		vessels[sensor]->setTSAddress(addr);
	else
	{
		memcpy(tSensor[sensor], addr, 8);
		EEPROMwriteBytes(sensor * 8, addr, 8);
	}
}

//**********************************************************************************
//PID Enabled (72); Bit 1 = HLT, Bit 2 = Mash, Bit 3 = Kettle, Bit 4 = Steam
//**********************************************************************************
//Set moved to vessel code
bool getPIDEnabled(byte vessel) {
	byte options = EEPROM.read(72);
	return bitRead(options, vessel);
}

//**********************************************************************************
//PIDp HLT (73), Mash (78), Kettle (83), Steam (88)
//**********************************************************************************
//Moved to vessel code
byte getPIDp(byte vessel) { return EEPROM.read(73 + vessel * 5); }

//**********************************************************************************
//PIDi HLT (74), Mash (79), Kettle (84), Steam (89)
//**********************************************************************************
//Set moved  to vessel code
byte getPIDi(byte vessel) { return EEPROM.read(74 + vessel * 5); }

//**********************************************************************************
//PIDd HLT (75), Mash (80), Kettle (85), Steam (90)
//**********************************************************************************
//Set moved  to vessel code
byte getPIDd(byte vessel) { return EEPROM.read(75 + vessel * 5); }

//**********************************************************************************
//PIDCycle HLT (76), Mash (81), Kettle (86), Steam (91)
//**********************************************************************************
//Set moved to vessel code
byte getPIDCycle(byte vessel)
{
	byte options = EEPROM.read(76 + vessel * 5);
	return bitRead(options, vessel);
}

//**********************************************************************************
//Hysteresis HLT (77), Mash (82), Kettle (87), Steam (92)
//**********************************************************************************
//Set moved to vessel code
byte getHysteresis(byte vessel)
{
	byte options = EEPROM.read(77 + vessel*5);
	return bitRead(options, vessel);
}

//**********************************************************************************
//Capacity HLT (93-96), Mash (97-100), Kettle (101-104)
//**********************************************************************************
//Moved to vessel code
//**********************************************************************************
//volLoss HLT (105-106), Mash (107-108), Kettle (109-110)
//**********************************************************************************
//Moved to vessel code

//**********************************************************************************
//Boil Temp (111)
//**********************************************************************************
byte getBoilTemp() { return EEPROM.read(111); }
void setBoilTemp(byte boilTemp) { EEPROM.write(111, boilTemp); }

//**********************************************************************************
//Boil Power (112)
//**********************************************************************************
void setBoilPwr(byte value) { 
  boilPwr = value;
  vessels[VS_KETTLE]->setMaxPower(boilPwr);
  EEPROM.write(112, value); 
}

//**********************************************************************************
//evapRate (113)
//**********************************************************************************
void setEvapRate(byte value) {
  EEPROM.write(113, value);
}
byte getEvapRate() { return EEPROM.read(113); }

//**********************************************************************************
//steamZero (114-115)
//**********************************************************************************
void setSteamZero(unsigned int value) {
#ifdef USESTEAM
  vessels[VS_STEAM]->updateVolumeCalibration(0,0,value);
#endif
}

//**********************************************************************************
//steamTgt (116)
//**********************************************************************************
void setSteamTgt(byte value) { 
#ifdef USESTEAM
  vessels[VS_STEAM]->setSetpoint(value);

	#endif
EEPROM.write(116, value); 
}
byte getSteamTgt() { return EEPROM.read(116); }

inline void setPumpTgt(byte value) { 

	flowController[0]->setTargetFlowRate(value);
	setSteamTgt(value); 
}

inline byte getPumpTgt() {
	return getSteamTgt();
}

//**********************************************************************************
//steamPSens (117-118)
//**********************************************************************************
//Handled in vessel class

//**********************************************************************************
//calibVols HLT (119-158), Mash (159-198), Kettle (199-238)
//calibVals HLT (239-258), Mash (259-278), Kettle (279-298)
//**********************************************************************************
//Moved to vessel code

//*****************************************************************************************************************************
// Power Loss Recovery Functions
//*****************************************************************************************************************************

//**********************************************************************************
//setpoints (299-301)
//**********************************************************************************
void setSetpoint(byte vessel, int value) {
  vessels[vessel]->setSetpoint(value);
    
  eventHandler(EVENT_SETPOINT, vessel);
}

//**********************************************************************************
//timers (302-305)
//**********************************************************************************
void setTimerRecovery(byte timer, unsigned int newMins) {
    EEPROMwriteInt(302 + timer * 2, newMins);
}

//**********************************************************************************
//Timer/Alarm Status (306)
//**********************************************************************************
void setTimerStatus(byte timer, boolean value) {
  timerStatus[timer] = value;
  byte options = EEPROM.read(306);
  bitWrite(options, timer, value);
  EEPROM.write(306, options);
  
  #ifdef DEBUG_TIMERALARM
    logStart_P(LOGDEBUG);
    logField("setTimerStatus");
    logFieldI(value);
    options = EEPROM.read(306);
    logFieldI(bitRead(options, timer));    
    logEnd();
  #endif
}

void setAlarmStatus(boolean value) {
  alarmStatus = value;
  byte options = EEPROM.read(306);
  bitWrite(options, 2, value);
  EEPROM.write(306, options);
  
  #ifdef DEBUG_TIMERALARM
    logStart_P(LOGDEBUG);
    logField("setAlarmStatus");
    logFieldI(value);
    options = EEPROM.read(306);
    logFieldI(bitRead(options, 2));
    logEnd();
  #endif
}



//**********************************************************************************
//Triggered Boil Addition Alarms (307-308)
//**********************************************************************************
unsigned int getBoilAddsTrig() { return EEPROMreadInt(307); }
void setBoilAddsTrig(unsigned int adds) { EEPROMwriteInt(307, adds); }

//**********************************************************************************
// ***OPEN*** (309-312)
//**********************************************************************************


//**********************************************************************************
//Program Threads (313-316)
//**********************************************************************************

void eepromLoadProgramThread(byte index, struct ProgramThread *thread) {
  eeprom_read_block((void *) thread, (unsigned char *) 313 + index * sizeof(struct ProgramThread), sizeof(struct ProgramThread));
}

void eepromSaveProgramThread(byte index, struct ProgramThread *thread) {
  eeprom_write_block((void *) thread, (unsigned char *) 313 + index * sizeof(struct ProgramThread), sizeof(struct ProgramThread));
}

//**********************************************************************************
// ***OPEN*** (317-397)
//**********************************************************************************

//**********************************************************************************
//Delay Start (Mins) (398-399)
//**********************************************************************************
unsigned int getDelayMins() { return EEPROMreadInt(398); }
void setDelayMins(unsigned int mins) {
    EEPROMwriteInt(398, mins);
}

//**********************************************************************************
//Grain Temp (400)
//**********************************************************************************
void setGrainTemp(byte grainTemp) { EEPROM.write(400, grainTemp); }
byte getGrainTemp() { return EEPROM.read(400); }

//*****************************************************************************************************************************
// Valve Profile Configuration (401-480; 481-785 Reserved)
//*****************************************************************************************************************************
void setValveCfg(byte profile, unsigned long value) {
  #ifdef PVOUT
    vlvConfig[profile] = value;
    EEPROMwriteLong(401 + profile * 4, value);
  #endif
}

//*****************************************************************************************************************************
// Program Load/Save Functions (786-1985) - 20 Program Slots Total
//*****************************************************************************************************************************
#define PROGRAM_SIZE 60
#define PROGRAM_START_ADDR 786

//**********************************************************************************
//Program Name (P:1-19)
//**********************************************************************************
void setProgName(byte preset, char *name) {
  for (byte i = 0; i < 19; i++) EEPROM.write(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + i, name[i]);
}

void getProgName(byte preset, char *name) {
  for (byte i = 0; i < 19; i++) name[i] = EEPROM.read(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + i);
  name[19] = '\0';
}

//**********************************************************************************
//OPEN (P:20)
//**********************************************************************************

//**********************************************************************************
//Sparge Temp (P:21)
//**********************************************************************************
void setProgSparge(byte preset, byte sparge) { EEPROM.write(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 21, sparge); }
byte getProgSparge(byte preset) { return EEPROM.read(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 21) * SETPOINT_MULT; }

//**********************************************************************************
//Boil Mins (P:22-23)
//**********************************************************************************
void setProgBoil(byte preset, int boilMins) { 
  if (boilMins != -1) EEPROMwriteInt(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 22, boilMins); 
}
unsigned int getProgBoil(byte preset) { return EEPROMreadInt(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 22); }

//**********************************************************************************
//Mash Ratio (P:24-25)
//**********************************************************************************
void setProgRatio(byte preset, unsigned int ratio) { EEPROMwriteInt(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 24, ratio); }
unsigned int getProgRatio(byte preset) { return EEPROMreadInt(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 24); }

//**********************************************************************************
//Mash Temps (P:26-31)
//**********************************************************************************
void setProgMashTemp(byte preset, byte mashStep, byte mashTemp) { EEPROM.write(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 26 + mashStep, mashTemp); }
byte getProgMashTemp(byte preset, byte mashStep) { return EEPROM.read(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 26 + mashStep) * SETPOINT_MULT; }

//**********************************************************************************
//Mash Times (P:32-37)
//**********************************************************************************
void setProgMashMins(byte preset, byte mashStep, byte mashMins) { 
  //This one is very tricky. Since it is better to avoid memory allocation changes. Here is the trick. 
  //setProgMashMins is not supposed to received a value larger than 119 unless someone change it. But it can receive -1 
  //when the user CANCEL its action of editing the mashing time value. -1 is converted as 255 (in a byte format). That is why
  //the condition is set on 255 instead of -1. 
  if (mashMins != 255) EEPROM.write(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 32 + mashStep, mashMins); 
}
byte getProgMashMins(byte preset, byte mashStep) { return EEPROM.read(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 32 + mashStep); }

//**********************************************************************************
//Batch Vol (P:38-41)
//**********************************************************************************
unsigned long getProgBatchVol(byte preset) { return EEPROMreadLong(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 38); }
void setProgBatchVol (byte preset, unsigned long vol) { EEPROMwriteLong(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 38, vol); }

//**********************************************************************************
//Mash Liquor Heat Source (P:42)
//**********************************************************************************
void setProgMLHeatSrc(byte preset, byte vessel) { EEPROM.write(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 42, vessel); }
byte getProgMLHeatSrc(byte preset) { return EEPROM.read(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 42); }

//**********************************************************************************
//HLT Temp (P:43)
//**********************************************************************************
void setProgHLT(byte preset, byte HLT) { EEPROM.write(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 43, HLT); }
byte getProgHLT(byte preset) { return EEPROM.read(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 43) * SETPOINT_MULT; }

//**********************************************************************************
//Pitch Temp (P:44)
//**********************************************************************************
void setProgPitch(byte preset, byte pitch) { EEPROM.write(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 44, pitch); }
byte getProgPitch(byte preset) { return EEPROM.read(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 44) * SETPOINT_MULT; }

//**********************************************************************************
//Boil Addition Alarms (P:45-46)
//**********************************************************************************
void setProgAdds(byte preset, unsigned int adds) { EEPROMwriteInt(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 45, adds); }
unsigned int getProgAdds(byte preset) { return EEPROMreadInt(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 45); }

//**********************************************************************************
//Grain Weight (P:47-50)
//**********************************************************************************
void setProgGrain(byte preset, unsigned long grain) { EEPROMwriteLong(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 47, grain); }
unsigned long getProgGrain(byte preset) { return EEPROMreadLong(PROGRAM_START_ADDR + preset * PROGRAM_SIZE + 47); }

//**********************************************************************************
//OPEN (P:51-59)
//**********************************************************************************

//**********************************************************************************
//BrewTroller Fingerprint (2046)
//**********************************************************************************

//**********************************************************************************
//EEPROM Version (2047)
//**********************************************************************************

//**********************************************************************************
//LCD Bright/Contrast (2048-2049) ATMEGA1284P Only
//**********************************************************************************

//**********************************************************************************
//Trigger Pins (2050-2054) ATMEGA1284P Only  +Reserved: 2055-2064
//**********************************************************************************
#ifdef DIGITAL_INPUTS
  byte getTriggerPin(byte triggerIndex) {
    #if defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega1284__)
      return EEPROM.read(2050 + triggerIndex);
    #else
      return 0;
    #endif
  }
  
  void setTriggerPin(byte triggerIndex, byte inputIndex) {
    #if defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega1284__)
      EEPROM.write(2050 + triggerIndex, inputIndex);
      triggerSetup(); //Call triggerSetup() to reattach
    #endif
  }
#endif


//**********************************************************************************
//Modbus Relay Boards (2065-2074) ATMEGA1284P Only + Reserved: 2075-2084 
//**********************************************************************************
byte getVlvModbusAddr(byte board) {
  return EEPROM.read(2065 + board * 5);
}

unsigned int getVlvModbusReg(byte board) {
  return EEPROMreadInt(2065 + board * 5 + 1);
}

byte getVlvModbusCoilCount(byte board) {
  return EEPROM.read(2065 + board * 5 + 3);
}

byte getVlvModbusOffset(byte board) {
  return EEPROM.read(2065 + board * 5 + 4);
}

void setVlvModbusAddr(byte board, byte addr) {
  EEPROM.write(2065 + board * 5, addr);
}

void setVlvModbusReg(byte board, unsigned int reg) {
  EEPROMwriteInt(2065 + board * 5 + 1, reg);
}

void setVlvModbusCoilCount(byte board, byte count) {
  EEPROM.write(2065 + board * 5 + 3, count);
}

void setVlvModbusOffset(byte board, byte offset) {
  EEPROM.write(2065 + board * 5 + 4, offset);
}

void setVlvModbusDefaults(byte board) {
  setVlvModbusAddr(board, PVOUT_MODBUS_ADDRNONE);
  setVlvModbusReg(board, PVOUT_MODBUS_DEFCOILREG);
  setVlvModbusCoilCount(board, PVOUT_MODBUS_DEFCOILCOUNT);
  byte defaultOffset = PVOUT_COUNT;
  if (board)
    for (byte i = 0; i < board; i++)
      defaultOffset += getVlvModbusOffset(board);
  setVlvModbusOffset(board, defaultOffset);
}

//*****************************************************************************************************************************
// Check/Update/Format EEPROM
//*****************************************************************************************************************************
boolean checkConfig() {
  byte cfgVersion = EEPROM.read(2047);
  byte BTFinger = EEPROM.read(2046);

  //If the BT 1.3 fingerprint is missing force a init of EEPROM
  //FermTroller will bump to a cfgVersion starting at 7
  if (BTFinger != 252 || cfgVersion == 255) {
    //Force default LCD Bright/Contrast to allow user to see 'Missing Config' EEPROM prompt
    #if (defined __AVR_ATmega1284P__ || defined __AVR_ATmega1284__) && defined UI_DISPLAY_SETUP && defined UI_LCD_4BIT
      EEPROM.write(2048, LCD_DEFAULT_BRIGHTNESS);
      EEPROM.write(2049, LCD_DEFAULT_CONTRAST);
    #endif
    return 1;
  }

  //In the future, incremental EEPROM settings will be included here
  switch(cfgVersion) {
    case 0:
      //Supported PID cycle is changing from 1-255 to .1-25.5
      //All current PID cycle settings will be multiplied by 10 to represent tenths (s)
      for (byte vessel = VS_HLT; vessel <= VS_STEAM; vessel++) EEPROM.write(76 + vessel * 5, EEPROM.read(76 + vessel * 5) * 10);
      //Set cfgVersion = 1
      EEPROM.write(2047, 1);
    case 1:
      //Set triggers to disabled by default
      for (byte trig = 0; trig < NUM_TRIGGERS; trig++) EEPROM.write(2050 + trig, 0);
      EEPROM.write(2047, 2);
    case 2:
      for (byte i = 0; i < PVOUT_MODBUS_MAXBOARDS; i++)
        setVlvModbusDefaults(i);
      EEPROM.write(2047, 3);
    case 3:
      for (uint8_t i = 0; i < PVOUT_MODBUS_MAXBOARDS; i++) {
          setVlvModbusDefaults(i);
      }
      EEPROM.write(2057, 4);
  }
  return 0;
}

void initEEPROM() {
  //Format EEPROM to 0's
#if defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega1284__)
  for (int i = 0; i < 4096; i++) EEPROM.write(i, 0);
#else
  for (int i = 0; i < 2048; i++) EEPROM.write(i, 0);
#endif

  //Set BT 1.3 Fingerprint (252)
  EEPROM.write(2046, 252);

  //Default Output Settings: p: 3, i: 4, d: 2, cycle: 4s, Hysteresis 0.3C(0.5F)
  for (byte vessel = VS_HLT; vessel <= NUM_VESSELS; vessel++) {
	  
#ifdef USEMETRIC
		  vessels[vessel]->setHysteresis(3);
#else
		  vessels[vessel]->setHysteresis(5);
#endif
		  vessels[vessel]->setTunings(3,4,2);
		  vessels[vessel]->setPIDCycle(4);

	  
  }

  //Default Grain Temp = 60F/16C
  //If F else C
  #ifdef USEMETRIC
    setGrainTemp(16);
  #else
    setGrainTemp(60);
  #endif

  //Set Default Boil temp 212F/100C
  #ifdef USEMETRIC
    setBoilTemp(100);
  #else
    setBoilTemp(212);
  #endif

  setBoilPwr(100);

  //Set all steps idle
  for (byte i = 0; i < PROGRAMTHREAD_MAX; i++) {
    struct ProgramThread thread;
    thread.activeStep = BREWSTEP_NONE;
    thread.recipe = RECIPE_NONE;
    eepromSaveProgramThread(i, &thread);
  }
  
  //Set default LCD Bright/Contrast
  #if (defined __AVR_ATmega1284P__ || defined __AVR_ATmega1284__) && defined UI_DISPLAY_SETUP && defined UI_LCD_4BIT
    EEPROM.write(2048, LCD_DEFAULT_BRIGHTNESS);
    EEPROM.write(2049, LCD_DEFAULT_CONTRAST);
  #endif
  
  //Set cfgVersion = 0
  EEPROM.write(2047, 0);

  //restart
  softReset();
}

//*****************************************************************************************************************************
// EEPROM Type Read/Write Functions
//*****************************************************************************************************************************
long EEPROMreadLong(int address) {
  long out;
  eeprom_read_block((void *) &out, (unsigned char *) address, 4);
  return out;
}

void EEPROMwriteLong(int address, long value) {
  eeprom_write_block((void *) &value, (unsigned char *) address, 4);
}

int EEPROMreadInt(int address) {
  int out;
  eeprom_read_block((void *) &out, (unsigned char *) address, 2);
  return out;
}

void EEPROMwriteInt(int address, int value) {
  eeprom_write_block((void *) &value, (unsigned char *) address, 2);
}

void EEPROMwriteBytes(int addr, byte bytes[], byte numBytes) {
  for (byte i = 0; i < numBytes; i++) {
    EEPROM.write(addr + i, bytes[i]);
  }
}

void EEPROMreadBytes(int addr, byte bytes[], byte numBytes) {
  for (byte i = 0; i < numBytes; i++) {
    bytes[i] = EEPROM.read(addr + i);
  }
}

