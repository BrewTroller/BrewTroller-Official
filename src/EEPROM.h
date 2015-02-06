#TODO Fix me! Replace Forward Declarations

#include <Arduino.h>

void loadSetup();

long EEPROMreadLong(int address);

void EEPROMwriteLong(int address, long value);

int EEPROMreadInt(int address);

void EEPROMwriteInt(int address, int value);

void EEPROMwriteBytes(int addr, byte bytes[], byte numBytes);

void EEPROMreadBytes(int addr, byte bytes[], byte numBytes);

boolean checkConfig();

void loadVlvConfigs();
byte getVlvModbusAddr(byte board);
void loadVlvModbus(byte board);
unsigned int getVlvModbusReg(byte board);
byte getVlvModbusCoilCount(byte board);
byte getVlvModbusOffset(byte board);
