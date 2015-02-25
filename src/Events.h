#ifndef EVENTS_H
#define EVENTS_H

void eventHandler(byte eventID, int eventParam);
void triggerSetup();
void eStopISR();
void spargeMaxISR();
void hltMinISR();
void mashMinISR();
void kettleMinISR();

#endif