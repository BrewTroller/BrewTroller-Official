#TODO Fix ME! Replace Forward declarations with class

#include <Arduino.h>

byte vesselAV(byte vessel);
byte vesselVLVHeat(byte vessel);
byte vesselVLVIdle(byte vessel);

boolean vlvConfigIsActive(byte profile);
void processHeatOutputs();