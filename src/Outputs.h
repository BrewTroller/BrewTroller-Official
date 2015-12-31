#ifndef OUTPUTS_H
#define OUTPUTS_H


#include <Arduino.h>
#include <pin.h>

#ifdef USEPWM
void pwmInit( void );
#endif //USEPWM


void pinInit();

void pidInit();

void resetOutputs();

void resetHeatOutput(byte vessel);

void processHeatOutputsPIDEnabled(const byte vessel[]);

void processHeatOutputsNonPIDEnabledWithHeatOn(const byte vessel[]);

void processHeatOutputsNonPIDEnabledWithHeatOff(const byte vessel[]);
void processHeatOutputsNonPIDEnabled(const byte vessel[]);

void processHeatOutputs();

#ifdef PVOUT
void updateValves();

void setValves(unsigned long vlvBits);

void processAutoValve();
#endif //#ifdef PVOUT

unsigned long computeValveBits();

boolean vlvConfigIsActive(byte profile);

void boilController ();

//Map AutoValve Profiles to Vessels
byte vesselAV(byte vessel);
byte vesselVLVHeat(byte vessel);
byte vesselVLVIdle(byte vessel);
pin * vesselMinTrigger(byte vessel);
byte autoValveBitmask(void);
byte getHeatPower (byte vessel);


#endif
