#ifndef TEMP_H
#define TEMP_H

#warning Fix me! Replace forward declarations

#include <Arduino.h>
#include "Temp.h"
#include "HWProfile.h"

void tempInit();

void updateTemps();

boolean tsReady();

boolean validAddr(byte* addr);

//This function search for an address that is not currently assigned!
void getDSAddr(byte addrRet[8]);

//Returns Int representing hundreths of degree
int read_temp(byte* addr);

#if defined MASH_AVG
void mashAvg();
#endif

#endif