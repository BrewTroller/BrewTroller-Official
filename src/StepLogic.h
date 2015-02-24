#ifndef STEPLOGIC_H
#define STEPLOGIC_H

#warning Fix me! Replace Forward Declarations

void programThreadsInit();
void programThreadsUpdate();

/**
* Used to determine if the given step is the active step in the program.
*/
boolean brewStepIsActive(byte brewStep);

/**
* Usd to determine if the given ZONE is the active ZONE in the program.
* Returns true is any step in the given ZONE is the active step, false otherwise.
*/
boolean zoneIsActive(byte brewZone);

byte programThreadActiveStep(byte threadIndex);

void programThreadRecipeName(byte threadIndex, char *returnValue);

byte programThreadRecipeIndex(byte threadIndex);

struct ProgramThread *programThreadAcquire();

void (*brewStepFunc(byte brewStep))(enum StepSignal, struct ProgramThread *);

struct ProgramThread *programThreadInit(byte recipe);

struct ProgramThread *programThreadInit(byte recipe, byte brewStep);

void programThreadSave(struct ProgramThread *thread);

void programThreadSetStep(struct ProgramThread *thread, byte brewStep);

boolean brewStepZoneInUse(byte brewStep);

void programThreadSignal(struct ProgramThread *thread, enum StepSignal signal);

//Supports BTnic commands against brewsteps with no knowledge of threads
void brewStepSignal(byte brewStep, enum StepSignal signal);

void programThreadResetAll();

void brewStepFill(enum StepSignal signal, struct ProgramThread *thread);

void brewStepDelay(enum StepSignal signal, struct ProgramThread *thread);

void brewStepPreheat(enum StepSignal signal, struct ProgramThread *thread);

void brewStepGrainIn(enum StepSignal signal, struct ProgramThread *thread);

void brewStepRefill(enum StepSignal signal, struct ProgramThread *thread);

void brewStepMashHelper(byte mashStep, enum StepSignal signal, struct ProgramThread *thread);

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

void resetSpargeValves();

#ifdef SMART_HERMS_HLT
void smartHERMSHLT();
#endif

unsigned long calcStrikeVol(byte recipe);
unsigned long calcSpargeVol(byte recipe);
unsigned long calcRefillVolume(byte recipe);
unsigned long calcPreboilVol(byte recipe);
unsigned long calcGrainLoss(byte recipe);
unsigned long calcGrainVolume(byte recipe);

/**
* Calculates the strike temperature for the mash.
*/
byte calcStrikeTemp(byte recipe);

byte getFirstStepTemp(byte recipe);


#endif