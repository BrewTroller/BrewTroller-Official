#ifndef TIMER_H
#define TIMER_H

#warning Fix me! Replace Forward Declarations
#include <Arduino.h>

void setTimer(byte timer, int minutes);

void pauseTimer(byte timer);

void clearTimer(byte timer);

void updateTimers();

//This function allows modulation of buzzer when the alarm is on.
void updateBuzzer();

void setAlarm(boolean alarmON);

void setBuzzer(boolean alarmON);


#endif