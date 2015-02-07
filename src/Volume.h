#ifndef VOLUME_H
#define VOLUME_H

#warning Fix me! Replace forward declaraions

void updateVols();
unsigned int GetCalibrationValue(byte vessel);
unsigned long readVolume( byte pin, unsigned long calibrationVols[10], unsigned int calibrationValues[10] );

#endif