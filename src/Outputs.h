#ifndef OUTPUTS_H
#define OUTPUTS_H

#warning Fix ME! Replace Forward declarations with class

#include <Arduino.h>
#include <pin.h>

byte vesselAV(byte vessel);
byte vesselVLVHeat(byte vessel);
byte vesselVLVIdle(byte vessel);

void resetOutputs();
void resetHeatOutput(byte vessel);

boolean vlvConfigIsActive(byte profile);
void processHeatOutputs();
void pinInit();
void pidInit();

void boilController ();
void setValves(unsigned long vlvBits);
void updateValves();
void resetHeatOutput(byte vessel);
unsigned long computeValveBits();
void processAutoValve();

pin * vesselMinTrigger(byte vessel);

#endif