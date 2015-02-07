#ifndef BT_EEPROM_H
#define BT_EEPROM_H

#warning Fix me! Replace Forward Declarations

#include <Arduino.h>

void loadSetup();

long EEPROMreadLong(int address);

void EEPROMwriteLong(int address, long value);

int EEPROMreadInt(int address);

void EEPROMwriteInt(int address, int value);

void EEPROMwriteBytes(int addr, byte bytes[], byte numBytes);

void EEPROMreadBytes(int addr, byte bytes[], byte numBytes);

boolean checkConfig();

unsigned int getBoilAddsTrig();
void setBoilAddsTrig(unsigned int adds);

void eepromLoadProgramThread(byte index, struct ProgramThread *thread);
void eepromSaveProgramThread(byte index, struct ProgramThread *thread);

void loadVlvConfigs();
byte getVlvModbusAddr(byte board);
void loadVlvModbus(byte board);
unsigned int getVlvModbusReg(byte board);
byte getVlvModbusCoilCount(byte board);
byte getVlvModbusOffset(byte board);

void setTSAddr(byte sensor, byte addr[8]);

byte getTriggerPin(byte triggerIndex);
void setPIDEnabled(byte vessel, boolean setting);
byte getPIDp(byte vessel);
byte getPIDi(byte vessel);
byte getPIDd(byte vessel);
void setPIDp(byte vessel, byte value);
void setPIDi(byte vessel, byte value);
void setPIDd(byte vessel, byte value);
void setPIDCycle(byte vessel, byte value);
void setHysteresis(byte vessel, byte value);

void setSetpoint(byte vessel, int value);
void setAlarmStatus(boolean value);
void setTimerRecovery(byte timer, unsigned int newMins);
void setTimerStatus(byte timer, boolean value);

void setSteamTgt(byte value);
byte getSteamTgt();


void setVolCalib(byte vessel, byte slot, unsigned int value, unsigned long vol);

void setProgName(byte preset, char *name);
void getProgName(byte preset, char *name);
void setProgSparge(byte preset, byte sparge);
byte getProgSparge(byte preset);
void setProgBoil(byte preset, int boilMins);
unsigned int getProgBoil(byte preset);
void setProgRatio(byte preset, unsigned int ratio);
unsigned int getProgRatio(byte preset);
void setProgMashMins(byte preset, byte mashStep, byte mashMins);
byte getProgMashMins(byte preset, byte mashStep);
void setProgMashTemp(byte preset, byte mashStep, byte mashTemp);
byte getProgMashTemp(byte preset, byte mashStep);
unsigned long getProgBatchVol(byte preset);
void setProgBatchVol (byte preset, unsigned long vol);
void setProgMLHeatSrc(byte preset, byte vessel);
byte getProgMLHeatSrc(byte preset);
void setProgHLT(byte preset, byte HLT);
byte getProgHLT(byte preset);
void setProgPitch(byte preset, byte pitch);
byte getProgPitch(byte preset);
void setProgAdds(byte preset, unsigned int adds);
unsigned int getProgAdds(byte preset);
void setProgGrain(byte preset, unsigned long grain);
unsigned long getProgGrain(byte preset);
void setGrainTemp(byte grainTemp);
byte getGrainTemp();
void setValveCfg(byte profile, unsigned long value);
unsigned int getDelayMins();
void setDelayMins(unsigned int mins);

byte getTriggerPin(byte triggerIndex);
void setTriggerPin(byte triggerIndex, byte inputIndex);

void setEvapRate(byte value);
byte getEvapRate();

void setCapacity(byte vessel, unsigned long value);
unsigned long getCapacity(byte vessel);

void setVolLoss(byte vessel, unsigned int value);
unsigned int getVolLoss(byte vessel);

void setBoilPwr(byte value);
byte getBoilTemp();
void setBoilTemp(byte boilTemp);

byte getVlvModbusAddr(byte board);
unsigned int getVlvModbusReg(byte board);
byte getVlvModbusCoilCount(byte board);
byte getVlvModbusOffset(byte board);
void setVlvModbusAddr(byte board, byte addr);
void setVlvModbusReg(byte board, unsigned int reg);
void setVlvModbusCoilCount(byte board, byte count);
void setVlvModbusOffset(byte board, byte offset);
void setVlvModbusDefaults(byte board);

void initEEPROM();

#endif