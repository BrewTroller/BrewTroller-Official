#ifndef VOLUME_H
#define VOLUME_H

void updateVols();
unsigned int GetCalibrationValue(byte vessel);
unsigned long readVolume( byte pin, unsigned long calibrationVols[10], unsigned int calibrationValues[10] );

#endif