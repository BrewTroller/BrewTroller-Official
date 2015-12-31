/*  +
   Copyright (C) 2009, 2010 Matt Reba, Jeremiah Dillingham

    This file is part of BrewTroller.

    BrewTroller is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BrewTroller is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BrewTroller.  If not, see <http://www.gnu.org/licenses/>.


BrewTroller - Open Source Brewing Computer
Software Lead: Matt Reba (matt_AT_brewtroller_DOT_com)
Hardware Lead: Jeremiah Dillingham (jeremiah_AT_brewtroller_DOT_com)

Documentation, Forums and more information available at http://www.brewtroller.com
*/
#include <Arduino.h>
#include "Config.h"
#include "BrewTroller.h"
#include "Events.h"
#include "EEPROM.h"
#include "Outputs.h"
#include "Timer.h"
#include "StepLogic.h"

unsigned long lastHop, grainInStart;
unsigned int boilAdds, triggered;

struct ProgramThread programThread[PROGRAMTHREAD_MAX];

void programThreadsInit() {
  for(byte i = 0; i < PROGRAMTHREAD_MAX; i++) {
    eepromLoadProgramThread(i, &programThread[i]);
    if (programThread[i].activeStep != BREWSTEP_NONE) {
      programThreadSignal(programThread + i, STEPSIGNAL_INIT);
      eventHandler(EVENT_STEPINIT, programThread[i].activeStep);  
    }
  }
}

void programThreadsUpdate() {
  for (byte i = 0; i < PROGRAMTHREAD_MAX; i++)
    if (programThread[i].activeStep != BREWSTEP_NONE)
      programThreadSignal(programThread + i, STEPSIGNAL_UPDATE);
}

/**
 * Used to determine if the given step is the active step in the program.
 */
boolean brewStepIsActive(byte brewStep) {
  for (byte i = 0; i < PROGRAMTHREAD_MAX; i++)
    if (programThread[i].activeStep == brewStep)
      return true; 
  return false;
}

/**
 * Usd to determine if the given ZONE is the active ZONE in the program.
 * Returns true is any step in the given ZONE is the active step, false otherwise.
 */
boolean zoneIsActive(byte brewZone) {
  byte stepMin, stepMax;
  switch (brewZone) {
    case ZONE_MASH:
      stepMin = BREWSTEP_FILL;
      stepMax = BREWSTEP_SPARGE;
      break;
    case ZONE_BOIL:
      stepMin = BREWSTEP_FILL;
      stepMax = BREWSTEP_SPARGE;
      break;
  }    

  for (byte i = 0; i < PROGRAMTHREAD_MAX; i++)
    if (programThread[i].activeStep >= stepMin && programThread[i].activeStep <= stepMax)
      return true;
  return false;
}

byte programThreadActiveStep(byte threadIndex) {
  return programThread[threadIndex].activeStep;
}

void programThreadRecipeName(byte threadIndex, char *returnValue) {
  getProgName(programThread[threadIndex].recipe, returnValue);
}

byte programThreadRecipeIndex(byte threadIndex) {
  return programThread[threadIndex].recipe;  
}

struct ProgramThread *programThreadAcquire() {
  for (byte i = 0; i < PROGRAMTHREAD_MAX; i++)
    if (programThread[i].activeStep == BREWSTEP_NONE)
      return programThread + i;
  return 0;
}

void (*brewStepFunc(byte brewStep))(enum StepSignal, struct ProgramThread *) {
  static void (*brewStepFunctionMap[BREWSTEP_COUNT])(enum StepSignal, struct ProgramThread *) = {
    &brewStepFill,
    &brewStepDelay,
    &brewStepPreheat,
    &brewStepGrainIn,
    &brewStepRefill,
    &brewStepDoughIn,
    &brewStepAcid,
    &brewStepProtein,
    &brewStepSacch,
    &brewStepSacch2,
    &brewStepMashOut,
    &brewStepMashHold,
    &brewStepSparge,
    &brewStepBoil,
    &brewStepChill
  };
  
  if (brewStep < BREWSTEP_COUNT)
    return brewStepFunctionMap[brewStep];
  return 0;
}

struct ProgramThread *programThreadInit(byte recipe) {
  return programThreadInit(recipe, BREWSTEP_FILL);
}

struct ProgramThread *programThreadInit(byte recipe, byte brewStep) {
  //Invalid recipe or Recipe 'None'
  if(recipe >= RECIPE_MAX)
    return 0;

  if (brewStepZoneInUse(brewStep))
    return 0;

  struct ProgramThread *thread = programThreadAcquire();
  if (!thread)
    return 0;

  //Determine what function we need to call
  void (*stepFunc)(enum StepSignal, struct ProgramThread *) = brewStepFunc(brewStep);
  if (!stepFunc)
    return 0;

  //If we made it without an abort, save the thread without an activeStep
  thread->activeStep = BREWSTEP_NONE;
  thread->recipe = recipe;
  programThreadSave(thread);
  
  //Signal function directly as there is no activeStep for signalProgramThread()
  (*stepFunc)(STEPSIGNAL_INIT, thread);
  
  //Abort if the brew step is still unset
  if (thread->activeStep == BREWSTEP_NONE)
    return 0;
  return thread;
}

void programThreadSave(struct ProgramThread *thread) {
  int index = thread - programThread;
  eepromSaveProgramThread(index, thread);
}

void programThreadSetStep(struct ProgramThread *thread, byte brewStep) {
  byte lastStep = thread->activeStep;
  thread->activeStep = brewStep;
  programThreadSave(thread);
  if (brewStep == BREWSTEP_NONE)
    eventHandler(EVENT_STEPEXIT, lastStep);
  else
    eventHandler(EVENT_STEPINIT, thread->activeStep);
}

boolean brewStepZoneInUse(byte brewStep) {
  if (brewStep >= BREWSTEP_FILL && brewStep <= BREWSTEP_MASHHOLD && zoneIsActive(ZONE_MASH))
    return 1;  
  else if (brewStep == BREWSTEP_SPARGE && (zoneIsActive(ZONE_MASH) || zoneIsActive(ZONE_BOIL)))
    return 1;  
  return 0;
}

void programThreadSignal(struct ProgramThread *thread, enum StepSignal signal) {
  void (*stepFunc)(enum StepSignal, struct ProgramThread *) = brewStepFunc(thread->activeStep);
  if (stepFunc)
    (*stepFunc)(signal, thread);
}

//Supports BTnic commands against brewsteps with no knowledge of threads
void brewStepSignal(byte brewStep, enum StepSignal signal) {
  for (byte i = 0; i < PROGRAMTHREAD_MAX; i++) {
    if (programThread[i].activeStep == brewStep) {
      void (*stepFunc)(enum StepSignal, struct ProgramThread *) = brewStepFunc(brewStep);
      if (stepFunc)
        (*stepFunc)(signal, programThread + i);
    }
  }
}

void programThreadResetAll() {
  for (byte i = 0; i < PROGRAMTHREAD_MAX; i++)
    programThreadSignal(programThread + i, STEPSIGNAL_ABORT); //Abort any active program threads
}

void brewStepFill(enum StepSignal signal, struct ProgramThread *thread) {
  switch (signal) {
    case STEPSIGNAL_INIT:
		for (byte i = 0; i < NUM_VESSELS; i++)
			vessels[i]->setStage(FILL);
      vessels[VS_HLT]->setTargetVolume(calcSpargeVol(thread->recipe));
      vessels[VS_MASH]->setTargetVolume(calcStrikeVol(thread->recipe));
      if (getProgMLHeatSrc(thread->recipe) == VS_HLT) {
		  vessels[VS_HLT]->setTargetVolume(min(vessels[VS_HLT]->getVolume() + vessels[VS_MASH]->getTargetVolume(), vessels[VS_HLT]->getCapacity()));
		  vessels[VS_MASH]->setTargetVolume(0);
      }
	  
#ifdef AUTO_FILL_START
	  fillController[0]->startAuto();
	  fillController[1]->startAuto();
#else 
	  fillController[0]->stop();
	  fillController[1]->stop();
#endif
      programThreadSetStep(thread, BREWSTEP_FILL);
      break;
    case STEPSIGNAL_UPDATE:
#ifndef VOLUME_MANUAL
		fillController[0]->update();
		fillController[1]->update();
#endif
//Note: because this calls itself recursively and the STEPSIGNAL_ADVANCE case has a delay in it, we need to process the StopFilling command first,
		//otherwise it won't stop until after the vessel is full
#ifdef AUTO_FILL_EXIT
        if (fillController[0]->isAtTarget() && fillController[1]->isAtTarget())
          brewStepFill(STEPSIGNAL_ADVANCE, thread);
      #else
        #endif
      break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
		vessels[VS_HLT]->setTargetVolume(0); 
		vessels[VS_MASH]->setTargetVolume(0);
		fillController[0]->update();
		fillController[1]->update();
      if (signal == STEPSIGNAL_ADVANCE) {
        if (getDelayMins())
          brewStepDelay(STEPSIGNAL_INIT, thread);
        else
          brewStepPreheat(STEPSIGNAL_INIT, thread);
      }
      break;
  }
}

void brewStepDelay(enum StepSignal signal, struct ProgramThread *thread) {
  switch (signal) {
    case STEPSIGNAL_INIT:
      //Load delay minutes from EEPROM if timer is not already populated via Power Loss Recovery
      if (getDelayMins() && !timerValue[TIMER_MASH])
        setTimer(TIMER_MASH, getDelayMins());
        programThreadSetStep(thread, BREWSTEP_DELAY);
      break;
    case STEPSIGNAL_UPDATE:
      if (timerValue[TIMER_MASH] == 0)
        brewStepDelay(STEPSIGNAL_ADVANCE, thread);
      break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
      clearTimer(TIMER_MASH);
      setAlarm(0);
      if (signal == STEPSIGNAL_ADVANCE)
        brewStepPreheat(STEPSIGNAL_INIT, thread);
      break;
  }
}

void brewStepPreheat(enum StepSignal signal, struct ProgramThread *thread) {
  static byte preheatVessel;
  switch (signal) {
    case STEPSIGNAL_INIT:
		for (byte i = 0; i < NUM_VESSELS; i++)
			vessels[i]->setStage(FILL);
      #ifdef MASH_PREHEAT_NOVALVES
        vlvConfig[VLV_MASHHEAT] = 0;
        vlvConfig[VLV_MASHIDLE] = 0;
      #endif
      preheatVessel = getProgMLHeatSrc(thread->recipe);
    
	  byte preheatTemp;
#ifdef MASH_PREHEAT_STRIKE
	  preheatTemp = calcStrikeTemp(stepProgram[BREWSTEP_PREHEAT]);
#elif defined MASH_PREHEAT_STEP1
	  preheatTemp = calcStrikeTemp(stepProgram[BREWSTEP_PREHEAT]);
#else
	  preheatTemp = 0; //Don't preheat
#endif        
	  vessels[VS_HLT]->setSetpoint(getProgHLT(thread->recipe));
	  if (preheatTemp)
		vessels[preheatVessel]->setSetpoint(calcStrikeTemp(thread->recipe)); //This will overwrite the HLT's fixed setpoint if the HLT is the preheat vessel. That is by design.
      
      #if defined USESTEAM
        vessels[VS_STEAM]->setSetpoint(getSteamTgt());
      #endif
      
	//No timer used for preheat
      clearTimer(TIMER_MASH);
      #ifdef MASH_PREHEAT_SENSOR
	  //TODO: make this configurable in new model
        //Overwrite mash temp sensor address from EEPROM using the memory location of the specified sensor (sensor element number * 8 bytes)
        //EEPROMreadBytes(MASH_PREHEAT_SENSOR * 8, tSensor[TS_MASH], 8);
      #endif
      programThreadSetStep(thread, BREWSTEP_PREHEAT);
      break;
    case STEPSIGNAL_UPDATE:
		if (vessels[preheatVessel]->hasReachedTargetTemperature())
		{
			setAlarm(1);
#ifdef AUTO_PREHEAT_EXIT 
			if (vessels[preheatVessel]->isPreheated())
				brewStepPreheat(STEPSIGNAL_ADVANCE, thread);
#endif
		}
    
      #if defined SMART_HERMS_HLT && defined SMART_HERMS_PREHEAT
        smartHERMSHLT();
      #endif
      break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
      clearTimer(TIMER_MASH);
      resetHeatOutput(VS_HLT);
      resetHeatOutput(VS_MASH);
      #ifdef USESTEAM
        resetHeatOutput(VS_STEAM);
      #endif
      #ifdef MASH_PREHEAT_SENSOR
      //Restore mash temp sensor address from EEPROM (address 8)
        EEPROMreadBytes(8, tSensor[TS_MASH], 8);
      #endif
      #ifdef MASH_PREHEAT_NOVALVES
        loadVlvConfigs();
      #endif
      if (signal == STEPSIGNAL_ADVANCE)
        brewStepGrainIn(STEPSIGNAL_INIT, thread);
      break;
  }
}

void brewStepGrainIn(enum StepSignal signal, struct ProgramThread *thread) {
  switch (signal) {
    case STEPSIGNAL_INIT:
		for (byte i = 0; i < NUM_VESSELS; i++)
			vessels[i]->setStage(FILL);

      //Disable HLT and Mash heat output during 'Add Grain' to avoid dry running heat elements and burns from HERMS recirc
      grainInStart = 0;
      resetHeatOutput(VS_HLT);
      resetHeatOutput(VS_MASH);
      #if defined USESTEAM
        vessels[VS_STEAM]->setSetpoint(getSteamTgt());
      #endif
      setAlarm(1);
      bitSet(actProfiles, VLV_ADDGRAIN);
      if(getProgMLHeatSrc(thread->recipe) == VS_HLT) {
        unsigned long spargeVol = calcSpargeVol(thread->recipe);
        unsigned long mashVol = calcStrikeVol(thread->recipe);
        vessels[VS_HLT]->setTargetVolume((min((spargeVol + mashVol), vessels[VS_HLT]->getCapacity()) - mashVol));
        #ifdef VOLUME_MANUAL
          // In manual volume mode show the target mash volume as a guide to the user
          vessels[VS_MASH]->setTargetVolume(mashVol);
        #endif
		  
		  
#ifndef AUTO_ML_XFER
			  flowController[0]->stop();
#else
			  flowController[0]->startAuto();
#endif
	
      }
      programThreadSetStep(thread, BREWSTEP_GRAININ);
      break;
    case STEPSIGNAL_UPDATE:
      #ifdef AUTO_GRAININ_EXIT
        if(flowController[0] && flowController[0]->isAtTarget()) {
          if (!grainInStart)
            grainInStart = millis();
          else if ((millis() - grainInStart) / 1000 > AUTO_GRAININ_EXIT)
            brewStepGrainIn(STEPSIGNAL_ADVANCE, thread);
        } 
      #endif
		 flowController[0]->update();
		break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
		vessels[VS_HLT]->setTargetVolume(0);
		resetHeatOutput(VS_HLT); 
		flowController[0]->update();
		bitClear(actProfiles, VLV_ADDGRAIN);
      
      #ifdef USESTEAM
        resetHeatOutput(VS_STEAM);
      #endif
      if (signal == STEPSIGNAL_ADVANCE) {
        if (calcRefillVolume(thread->recipe))
          brewStepRefill(STEPSIGNAL_INIT, thread);
        else
          brewStepDoughIn(STEPSIGNAL_INIT, thread);
      }
      break;
  }
}

void brewStepRefill(enum StepSignal signal, struct ProgramThread *thread) {
  switch (signal) {
    case STEPSIGNAL_INIT:
      vessels[VS_HLT]->setTargetVolume(calcRefillVolume(thread->recipe));
	  vessels[VS_MASH]->setTargetVolume(0);
#ifdef AUTO_REFILL_START
	  fillController[0]->startAuto();
#else
	  fillController[0]->stop();
#endif
      programThreadSetStep(thread, BREWSTEP_REFILL);
      break;
    case STEPSIGNAL_UPDATE:
      #ifdef AUTO_FILL_EXIT
        if (fillController[0]->isAtTarget())
          brewStepRefill(STEPSIGNAL_ADVANCE, thread);
      #endif
		fillController[0]->update();
      break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
		vessels[VS_HLT]->setTargetVolume(0);
      if (signal == STEPSIGNAL_ADVANCE)
        brewStepDoughIn(STEPSIGNAL_INIT, thread);
      break;
  }
}

void brewStepMashHelper(byte mashStep, enum StepSignal signal, struct ProgramThread *thread) {
  switch (signal) {
    case STEPSIGNAL_INIT:
      vessels[VS_HLT]->setSetpoint(getProgHLT(thread->recipe));
      #ifdef RIMS_MLT_SETPOINT_DELAY
        starttime = millis(); // get current time
        timetoset = starttime + RIMS_DELAY; //note that overflow of the milisecond timer is not covered here 
        steptoset = BREWSTEP_DOUGHIN + mashStep; //step that we need to set the setpoint to after the timer is done. 
        RIMStimeExpired = 0; //reset the boolean so that we know if the timer has expired for this program or not
        autoValve[vesselAV(TS_MASH)] = 1; // turn on the mash recirc valve profile as if the setpoint had been set
      #else
	  vessels[VS_MASH]->setSetpoint(getProgMashTemp(thread->recipe, mashStep));
      #endif
      
      #if defined USESTEAM
      vessels[VS_STEAM]->setSetpoint(getSteamTgt());
      #endif
      
      //Set timer only if empty (for purposes of power loss recovery)
      if (!timerValue[TIMER_MASH]) setTimer(TIMER_MASH, getProgMashMins(thread->recipe, mashStep)); 
      //Leave timer paused until preheated
      timerStatus[TIMER_MASH] = 0;
      programThreadSetStep(thread, BREWSTEP_DOUGHIN + mashStep);
      break;
    case STEPSIGNAL_UPDATE:
      #ifdef SMART_HERMS_HLT
        smartHERMSHLT();
      #endif
      if (vessels[VS_MASH]->hasReachedTargetTemperature()) {
        //Unpause Timer
        if (!timerStatus[TIMER_MASH]) pauseTimer(TIMER_MASH);
      }
      //Exit Condition (and skip unused mash steps)
      if (
          #ifdef RIMS_MLT_SETPOINT_DELAY
            getProgMashTemp(stepProgram[BREWSTEP_DOUGHIN + mashStep], mashStep) == 0 
            || (vessels[VS_MASH]->hasReachedTargetTemperature() && timerValue[TIMER_MASH] == 0)
          #else
            vessels[VS_MASH]->getSetpoint() == 0 
            || (vessels[VS_MASH]->hasReachedTargetTemperature() && timerValue[TIMER_MASH] == 0)
          #endif
         )
        brewStepMashHelper(mashStep, STEPSIGNAL_ADVANCE, thread);
      break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
      clearTimer(TIMER_MASH);
      resetHeatOutput(VS_HLT);
      resetHeatOutput(VS_MASH);
      #ifdef USESTEAM
        resetHeatOutput(VS_STEAM);
      #endif
      if (signal == STEPSIGNAL_ADVANCE) {
        void (*stepFunc)(enum StepSignal, struct ProgramThread *) = brewStepFunc(thread->activeStep + 1);
        if (stepFunc)
          (*stepFunc)(STEPSIGNAL_INIT, thread);
      }
      break;
  }  
}

void brewStepDoughIn(enum StepSignal signal, struct ProgramThread *thread) {
  brewStepMashHelper(MASHSTEP_DOUGHIN, signal, thread);
}

void brewStepAcid(enum StepSignal signal, struct ProgramThread *thread) {
  brewStepMashHelper(MASHSTEP_ACID, signal, thread);
}

void brewStepProtein(enum StepSignal signal, struct ProgramThread *thread) {
  brewStepMashHelper(MASHSTEP_PROTEIN, signal, thread);
}

void brewStepSacch(enum StepSignal signal, struct ProgramThread *thread) {
  brewStepMashHelper(MASHSTEP_SACCH, signal, thread);
}

void brewStepSacch2(enum StepSignal signal, struct ProgramThread *thread) {
  brewStepMashHelper(MASHSTEP_SACCH2, signal, thread);
}

void brewStepMashOut(enum StepSignal signal, struct ProgramThread *thread) {
  brewStepMashHelper(MASHSTEP_MASHOUT, signal, thread);
}

void brewStepMashHold(enum StepSignal signal, struct ProgramThread *thread) {
  switch (signal) {
    case STEPSIGNAL_INIT:
      setAlarm(1);
      //Set HLT to Sparge Temp
      vessels[VS_HLT]->setSetpoint(getProgSparge(thread->recipe));
      //Cycle through steps and use last non-zero step for mash setpoint
      if (!vessels[VS_MASH]->getSetpoint()) {
        byte i = MASHSTEP_MASHOUT;
        while (vessels[VS_MASH]->getSetpoint() == 0 && i >= MASHSTEP_DOUGHIN && i <= MASHSTEP_MASHOUT)
			vessels[VS_MASH]->setSetpoint(getProgMashTemp(thread->recipe, i--));
      }
      #if defined USESTEAM
        vessels[VS_STEAM]->setSetpoint(getSteamTgt());
      #endif
      programThreadSetStep(thread, BREWSTEP_MASHHOLD);
      break;
    case STEPSIGNAL_UPDATE:
      #ifdef AUTO_MASH_HOLD_EXIT 
        if (!zoneIsActive(ZONE_BOIL) && vessels[VS_HLT]->isPreheated())
          brewStepMashHold(STEPSIGNAL_ADVANCE, thread);
      #endif
      break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
      resetHeatOutput(VS_HLT);
      resetHeatOutput(VS_MASH);
      #ifdef USESTEAM
        resetHeatOutput(VS_STEAM);
      #endif      
      if (signal == STEPSIGNAL_ADVANCE)
        brewStepSparge(STEPSIGNAL_INIT, thread);
      break;
  }
}

void autoSpargeStart()
{
#ifdef USEMETRIC
	// value is given in 10ths of a liter per min, so 1 liter/min would be 10, and 10 * 100 = 1000 which is 1 liter/min in flow rate calcs
		flowController[0]->startAuto();
		flowController[0]->setTargetFlowRate(getPumpTgt() * 100);
	
#else
	//value is given in 10ths of a quart per min, so 1 quart/min would be 10, and 10 *25 = 250 which is 1 quart/min in flow rate calcs (1000ths of a gallon/min)
		flowController[0]->startAuto();
		flowController[0]->setTargetFlowRate(getPumpTgt() * 25);
#endif
		flowController[1]->startAuto();
		flowController[1]->matchFlow(flowController[0]);
}

void brewStepSparge(enum StepSignal signal, struct ProgramThread *thread) {
  switch (signal) {
    case STEPSIGNAL_INIT:
		for (byte i = 0; i < NUM_VESSELS; i++)
			vessels[i]->setStage(SPARGE);
      #ifdef HLT_HEAT_SPARGE
        #ifdef HLT_MIN_SPARGE
          if (vessels[VS_HLT]->getVolume() >= HLT_MIN_SPARGE)
        #endif
            vessels[VS_HLT]->setSetpoint(getProgSparge(stepProgram[BREWSTEP_SPARGE]));
      #endif
      
      #ifdef BATCH_SPARGE
        //TODO: implement batch sparge logic!
      #else
          vessels[VS_KETTLE]->setTargetVolume(calcPreboilVol(thread->recipe));
        #ifdef AUTO_SPARGE_START
		  autoSpargeStart();
#else
		    flowController[0]->stop(); 
		    flowController[1]->stop();
		#endif
#endif //BATCH_SPARGE
		    programThreadSetStep(thread, BREWSTEP_SPARGE);
      break;
    case STEPSIGNAL_UPDATE:
      
#ifdef HLT_HEAT_SPARGE
        #ifdef HLT_MIN_SPARGE
          if (vessels[VS_HLT]->getVolume() < HLT_MIN_SPARGE)
            vessels[VS_HLT]->setSetpoint(0);
        #endif
      #endif
      
		  if (flowController[0]) flowController[0]->update();
		  if (flowController[1]) flowController[1]->update();

      #ifdef BATCH_SPARGE
      
      #else
        #ifdef AUTO_SPARGE_EXIT
           if (vessels[VS_KETTLE]->getVolume() >= vessels[VS_KETTLE]->getTargetVolume())
             brewStepSparge(STEPSIGNAL_ADVANCE, thread);
        #endif
      #endif
		   break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
      #ifdef HLT_HEAT_SPARGE
        vessels[VS_HLT]->setSetpoint(0);
      #endif
		vessels[VS_HLT]->setTargetVolume(0);
		vessels[VS_KETTLE]->setTargetVolume(0);

		flowController[0]->update();
		flowController[1]->update();

      if (signal == STEPSIGNAL_ADVANCE)
        brewStepBoil(STEPSIGNAL_INIT, thread);
      break;
  }
}

void brewStepBoil(enum StepSignal signal, struct ProgramThread *thread) {
 	switch (signal) {
    case STEPSIGNAL_INIT:
		for (byte i = 0; i < NUM_VESSELS; i++)
			vessels[i]->setStage(BOIL);
		flowController[0]->setTargetFlowRate(0);
		flowController[1]->setTargetFlowRate(0);
      vessels[VS_KETTLE]->setSetpoint(getBoilTemp());
      
      boilAdds = getProgAdds(thread->recipe);
      
      //Set timer only if empty (for purposes of power loss recovery)
      if (!timerValue[TIMER_BOIL]) {
        //Clean start of Boil
        setTimer(TIMER_BOIL, getProgBoil(thread->recipe));
        triggered = 0;
        setBoilAddsTrig(triggered);
      } else {
        //Assuming power loss recovery
        triggered = getBoilAddsTrig();
      }
      //Leave timer paused until preheated
      timerStatus[TIMER_BOIL] = 0;
      lastHop = 0;
      boilControlState = CONTROLSTATE_AUTO;
      programThreadSetStep(thread, BREWSTEP_BOIL);
      break;
    case STEPSIGNAL_UPDATE:
      #ifdef PREBOIL_ALARM
        if (!(triggered & 32768) && vessels[VS_KETTLE]->getTemperature()  != BAD_TEMP && vessels[VS_KETTLE]->getTemperature() >= PREBOIL_ALARM * 100) {
          setAlarm(1);
          triggered |= 32768; 
          setBoilAddsTrig(triggered);
        }
      #endif
      if (vessels[VS_KETTLE]->hasReachedTargetTemperature()) {
        //Unpause Timer
        if (!timerStatus[TIMER_BOIL])
          pauseTimer(TIMER_BOIL);
      }
      //Turn off hop valve profile after 5s
      if (lastHop > 0 && millis() - lastHop > HOPADD_DELAY) {
        bitClear(actProfiles, VLV_HOPADD);
        lastHop = 0;
      }
      if (vessels[VS_KETTLE]->hasReachedTargetTemperature()) {
        //Boil Addition
        if ((boilAdds ^ triggered) & 1) {
          bitSet(actProfiles, VLV_HOPADD);
          lastHop = millis();
          setAlarm(1); 
          triggered |= 1; 
          setBoilAddsTrig(triggered); 
        }
        //Timed additions (See hoptimes[] array in BrewTroller.pde)
        for (byte i = 0; i < 11; i++) {
          if (((boilAdds ^ triggered) & (1<<(i + 1))) && timerValue[TIMER_BOIL] <= hoptimes[i] * unsigned(60000)) {
            bitSet(actProfiles, VLV_HOPADD);
            lastHop = millis();
            setAlarm(1); 
            triggered |= (1<<(i + 1)); 
            setBoilAddsTrig(triggered);
          }
        }
        
        #ifdef AUTO_BOIL_RECIRC
        if (timerValue[TIMER_BOIL] <= AUTO_BOIL_RECIRC * 60000) bitSet(actProfiles, VLV_BOILRECIRC);
        #endif
      }
      //Exit Condition  
      if(vessels[VS_KETTLE]->hasReachedTargetTemperature() && timerValue[TIMER_BOIL] == 0) {
        //Kill Kettle power at end of timer...
        resetHeatOutput(VS_KETTLE);      
        //...but wait for last hop addition to complete before leaving step
        if(lastHop == 0)
          brewStepBoil(STEPSIGNAL_ADVANCE, thread);
      }
      break;
    case STEPSIGNAL_ABORT:
      programThreadSetStep(thread, BREWSTEP_NONE);
    case STEPSIGNAL_ADVANCE:
      bitClear(actProfiles, VLV_HOPADD);
      #ifdef AUTO_BOIL_RECIRC
        bitClear(actProfiles, VLV_BOILRECIRC);
      #endif
      boilControlState = CONTROLSTATE_OFF;
      resetHeatOutput(VS_KETTLE);
      clearTimer(TIMER_BOIL);
      if (signal == STEPSIGNAL_ADVANCE)
        brewStepChill(STEPSIGNAL_INIT, thread);
      break;
  }
}

void brewStepChill(enum StepSignal signal, struct ProgramThread *thread) {
  switch (signal) {
    case STEPSIGNAL_INIT:
      pitchTemp = getProgPitch(thread->recipe);
      programThreadSetStep(thread, BREWSTEP_CHILL);
      break;
    case STEPSIGNAL_UPDATE:
      if (vessels[VS_KETTLE]->getTemperature() != -1 && vessels[VS_KETTLE]->getTemperature() <= KETTLELID_THRESH) {
        if (!vlvConfigIsActive(VLV_KETTLELID))
          bitSet(actProfiles, VLV_KETTLELID);
      } else {
        if (vlvConfigIsActive(VLV_KETTLELID))
          bitClear(actProfiles, VLV_KETTLELID);
      }
      break;
    case STEPSIGNAL_ABORT:
    case STEPSIGNAL_ADVANCE:
      programThreadSetStep(thread, BREWSTEP_NONE);
      autoValve[AV_CHILL] = 0;
      bitClear(actProfiles, VLV_CHILLBEER);
      bitClear(actProfiles, VLV_CHILLH2O);
      break;
  }
}

#ifdef SMART_HERMS_HLT
void smartHERMSHLT() {
  if (!vessels[VS_MASH]->getSetpoint()  return;
  vessels[VS_HLT]->setSetpoint(vessels[VS_MASH]->getSetpoint() * 2 - vessels[VS_MASH]->getTemperature());
  //Constrain HLT Setpoint to Mash Setpoint + MASH_HEAT_LOSS (minimum) and HLT_MAX_TEMP (Maximum)
  vessels[VS_HLT]->setSetpoint(constrain(vessels[VS_HLT]->getSetpoint(), vessels[VS_MASH]->getSetpoint() + MASH_HEAT_LOSS * 100, HLT_MAX_TEMP * 100);
}
#endif
  
unsigned long calcStrikeVol(byte recipe) {
  unsigned int mashRatio = getProgRatio(recipe);
  unsigned long retValue;
  if (mashRatio) {
    retValue = round(getProgGrain(recipe) * mashRatio / 100.0);

    //Convert qts to gal for US
    #ifndef USEMETRIC
      retValue = round(retValue / 4.0);
    #endif
    retValue += vessels[VS_MASH]->getDeadspace();
  }
  else {
    //No Sparge Logic (Matio Ratio = 0)
    retValue = calcPreboilVol(recipe);
  
    //Add Water Lost in Spent Grain
    retValue += calcGrainLoss(recipe);
    
    //Add Loss from other Vessels
    retValue += (vessels[VS_HLT]->getDeadspace() + vessels[VS_MASH]->getDeadspace());
  }
  return retValue;
}

unsigned long calcSpargeVol(byte recipe) {
  //Determine Preboil Volume Needed (Batch + Evap + Deadspace + Thermo Shrinkage)
  unsigned long retValue = calcPreboilVol(recipe);

  //Add Water Lost in Spent Grain
  retValue += calcGrainLoss(recipe);
  
  //Add Loss from other Vessels
  retValue += (vessels[VS_HLT]->getDeadspace()  + vessels[VS_MASH]->getDeadspace());

  //Subtract Strike Water Volume
  retValue -= calcStrikeVol(recipe);
  return retValue;
}

unsigned long calcRefillVolume(byte recipe) {
  unsigned long returnValue;
  if (getProgMLHeatSrc(recipe) == VS_HLT) {
    #ifdef HLT_MIN_REFILL
      SpargeVol = calcSpargeVol(recipe);
      returnValue = max(SpargeVol, HLT_MIN_REFILL_VOL);
    #else
      returnValue = calcSpargeVol(recipe);
    #endif
  }
  return returnValue;
}

unsigned long calcPreboilVol(byte recipe) {
  // Pre-Boil Volume is the total volume needed in the kettle to ensure you can collect your anticipated batch volume
  // It is (((batch volume + kettle loss) / thermo shrinkage factor ) / evap loss factor )
  //unsigned long retValue = (getProgBatchVol(recipe) / (1.0 - getEvapRate() / 100.0 * getProgBoil(recipe) / 60.0)) + vessels[VS_KETTLE]->getDeadspace(); // old logic 
  #ifdef BOIL_OFF_GALLONS
    unsigned long retValue = (((getProgBatchVol(recipe) + vessels[VS_KETTLE]->getDeadspace()) / VOL_SHRINKAGE) + (((unsigned long)getEvapRate() * EvapRateConversion) * getProgBoil(recipe) / 60.0));
  #else
    unsigned long retValue = (((getProgBatchVol(recipe) + vessels[VS_KETTLE]->getDeadspace()) / VOL_SHRINKAGE) / (1.0 - getEvapRate() / 100.0 * getProgBoil(recipe) / 60.0));
  #endif
  return round(retValue);
}

unsigned long calcGrainLoss(byte recipe) {
  unsigned long retValue;
  retValue = round(getProgGrain(recipe) * GRAIN_VOL_LOSS);
  return retValue;
}

unsigned long calcGrainVolume(byte recipe) {
  return round (getProgGrain(recipe) * GRAIN2VOL);
}

/**
 * Calculates the strike temperature for the mash.
 */
byte calcStrikeTemp(byte recipe) {
  //Metric temps are stored as quantity of 0.5C increments
  float strikeTemp = (float)getFirstStepTemp(recipe) / SETPOINT_DIV;
  float grainTemp = (float)getGrainTemp() / SETPOINT_DIV;
  
  //Imperial units must be converted from gallons to quarts
  #ifdef USEMETRIC
    const uint8_t kMashRatioVolumeFactor = 1;
  #else
    const uint8_t kMashRatioVolumeFactor = 4;
  #endif
  
  //Calculate mash ratio to include logic for no sparge recipes (Using mash ratio of 0 would not work in calcs)
  float mashRatio = (float)calcStrikeVol(recipe) *  kMashRatioVolumeFactor / getProgGrain(recipe);
  
  #ifdef USEMETRIC
    const float kGrainThermoDynamic = 0.41;
  #else
    const float kGrainThermoDynamic = 0.2;
  #endif
  
  //Calculate strike temp using the formula:
  //  Tw = (TDC/r)(T2 - T1) + T2
  //  where:
  //    TDC = Thermodynamic constant (0.2 for Imperial Units and 0.41 for Metric)
  //    r = The ratio of water to grain in quarts per pound or l per kg
  //    T1 = The initial temperature of the mash
  //    T2 = The target temperature of the mash
  //    Tw = The actual temperature of the infusion water
  strikeTemp = (kGrainThermoDynamic / mashRatio) * (strikeTemp - grainTemp) + strikeTemp;

  //Add Config.h value for adjustments if any
  strikeTemp += STRIKE_TEMP_OFFSET;
  
  //Return value in EEPROM format which is 0-255F or 0-255 x 0.5C
  return strikeTemp * SETPOINT_DIV;
}

byte getFirstStepTemp(byte recipe) {
  byte firstStep = 0;
  byte i = MASHSTEP_DOUGHIN;
  while (firstStep == 0 && i <= MASHSTEP_MASHOUT) firstStep = getProgMashTemp(recipe, i++);
  return firstStep;
}
