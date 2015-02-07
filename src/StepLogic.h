#ifndef STEPLOGIC_H
#define STEPLOGIC_H

#warning Fix me! Replace Forward Declarations

void programThreadsUpdate();
void programThreadsInit();
struct ProgramThread *programThreadInit(byte recipe);
struct ProgramThread *programThreadInit(byte recipe, byte brewStep);
byte getFirstStepTemp(byte recipe);

unsigned long calcGrainVolume(byte recipe);
unsigned long calcGrainLoss(byte recipe);

unsigned long calcRefillVolume(byte recipe);
unsigned long calcPreboilVol(byte recipe);

void brewStepFill(enum StepSignal signal, struct ProgramThread *thread);
void brewStepDelay(enum StepSignal signal, struct ProgramThread *thread);
void brewStepPreheat(enum StepSignal signal, struct ProgramThread *thread);
void brewStepGrainIn(enum StepSignal signal, struct ProgramThread *thread);
void brewStepRefill(enum StepSignal signal, struct ProgramThread *thread);
void brewStepDoughIn(enum StepSignal signal, struct ProgramThread *thread);
void brewStepAcid(enum StepSignal signal, struct ProgramThread *thread);
void brewStepProtein(enum StepSignal signal, struct ProgramThread *thread);
void brewStepSacch(enum StepSignal signal, struct ProgramThread *thread);
void brewStepSacch2(enum StepSignal signal, struct ProgramThread *thread);
void brewStepMashOut(enum StepSignal signal, struct ProgramThread *thread);
void brewStepMashHold(enum StepSignal signal, struct ProgramThread *thread);
void brewStepSparge(enum StepSignal signal, struct ProgramThread *thread);
void brewStepBoil(enum StepSignal signal, struct ProgramThread *thread);
void brewStepChill(enum StepSignal signal, struct ProgramThread *thread);


unsigned long calcStrikeVol(byte recipe);
byte calcStrikeTemp(byte recipe);
unsigned long calcSpargeVol(byte recipe);

boolean brewStepIsActive(byte brewStep);
boolean zoneIsActive(byte brewZone);

boolean brewStepZoneInUse(byte brewStep);

void brewStepSignal(byte brewStep, enum StepSignal signal);
void programThreadResetAll();
void programThreadSignal(struct ProgramThread *thread, enum StepSignal signal);
void programThreadSave(struct ProgramThread *thread);

void resetSpargeValves();

#endif