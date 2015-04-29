#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <menu.h>

void uiInit();
void unlockUI();
void uiCore();
void setActive(byte screen);
void uiEvent(byte eventID, byte eventParam);
void volCalibMenu(char sTitle[], byte vessel);
void volCalibEntryMenu(byte vessel, byte entry);
void cfgMODBUSOutputs();
void cfgMODBUSOutputBoard(byte board);
void cfgMODBUSOutputAssign(byte board);

void setPIDGain(char sTitle[], byte vessel);
unsigned long cfgValveProfile (char sTitle[], unsigned long defValue);

void assignSensor();
void displayAssignSensorTemp(int sensor);
void cfgTriggers();
void adjustLCD();
void cfgValves();
void cfgVolumes();
void cfgOutputs();
void cfgOutputs();

void menuSetup();

void uiInit();
void uiEstop();
void boilControlMenu();
void continueClick();
void stepAdvanceFailDialog();
void editProgramMenu();
void startProgramMenu();
void editProgram(byte pgm);
void showProgCalcs(byte pgm);
void editMashSchedule(byte pgm);
unsigned int editHopSchedule (unsigned int sched);
byte MLHeatSrcMenu(byte MLHeatSrc);
void warnHLT(unsigned long spargeVol);
void warnMash(unsigned long mashVol, unsigned long grainVol);
void warnBoil(unsigned long preboilVol);

void screenRefresh();
void screenInit();
void screenEnter();

byte scrollMenu(const char* sTitle, menu *objMenu);
void drawMenu(const char* sTitle, menu *objMenu);
byte getChoice(menu *objMenu, byte iRow);
boolean confirmChoice(const char *choice, byte row);
boolean confirmAbort();
boolean confirmDel();
int getTimerValue(const char *sTitle, int defMins, byte maxHours);
void printTimer(byte timer, byte iRow, byte iCol);
void getString(const char *sTitle, char defValue[], byte chars);

unsigned long getValue_P(const char *sTitle, unsigned long defValue, unsigned int divisor, unsigned long maxValue, const char *dispUnit);
unsigned long getValue(char sTitle[], unsigned long defValue, unsigned int divisor, unsigned long maxValue, const char *dispUnit);
unsigned long ulpow(unsigned long base, unsigned long exponent);
unsigned long getHexValue(char sTitle[], unsigned long defValue);

/**
 * Concatenate two strings from flash, placing result in RAM
 *
 * WARNING: It is not typically more SPACE efficient to concatenate two separate flash strings
 *          than using one static concatenated string. This should only be used to simiplify code
 *          and enhace readability
 */
char* concatPSTRS(char* dst, const char* one, const char* two);

void UIinitEEPROM();

byte ASCII2enc(byte charin);
byte enc2ASCII(byte charin);

#endif