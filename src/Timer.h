#ifndef TIMER_H
#define TIMER_H

#warning Fix me! Replace Forward Declarations

void setAlarm(boolean alarmON);
void updateTimers();
void setTimer(byte timer, int minutes);
void clearTimer(byte timer);
void pauseTimer(byte timer);
void updateBuzzer();
void setBuzzer(boolean alarmON);

#endif