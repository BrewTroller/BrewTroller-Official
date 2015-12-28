/*
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
#include <pin.h>
#include "HardwareProfile.h"
#include "Enum.h"
#include "BrewTroller.h"
#include "EEPROM.h"
#include "Outputs.h"
#include "Events.h"
#include "Vessel.h"

  #ifdef USESTEAM
    #define LAST_HEAT_OUTPUT VS_STEAM
  #elif defined DIRECT_FIRED_RIMS
    #define LAST_HEAT_OUTPUT VS_STEAM
  #else
    #define LAST_HEAT_OUTPUT VS_KETTLE
  #endif

#ifdef USEPWM
// note there are some assumptions here, we assume that the COM1A1, COM1B1, COM1A0, and COM1B0 
// bits are all 0 (as they should be on power up)
void pwmInit( void )
{
    // set timer 1 prescale factor to 0
    sbi(TCCR1B, CS10);
    cbi(TCCR1B, CS12);
    cbi(TCCR1B, CS11);

    //clear timer 1 out of 8 bit phase correct PWM mode from sanguino init
    cbi(TCCR1A, WGM10);
    //set timer 1 into 16 bit phase and frequency correct PWM mode with ICR1 as TOP
    sbi(TCCR1B, WGM13);
    //set TOP as 1000, which makes the overflow on return to bottom for this mode happen every 
    // 125uS given a 16mhz input clock, aka 8khz PWM frequency, the overflow ISR will handle 
    // the PWM outputs that are slower than 8khz, and the OCR1A/B ISR will handle the 8khz PWM outputs
    ICR1 = 1000; 

    //enable timer 1 overflow interrupt (in this mode overflow happens when the timer counds down to BOTTOM
    // after counting UP from BOTTOM to TOP. 
    sbi(TIMSK1, TOIE1);

}

//note that the code in any SIGNAL function is an ISR, and the code needs to kept short and fast
// it is important to avoid divides by non power of 2 numbers, remainder (mod) calculations, wait loops,
// or calls to functions that have wait loops. It's also not a good idea to write into any global that may be 
// used else where in the code inside here without interrupt protecting all accesses to that variable in 
// non ISR code, or making sure that if we do write to it in the ISR, we dont write/read to it in non ISR code
// (for example, below the heatPin objects are not written to if PIDEnable[i] = 1;
//
// Also the below ISR is set to nonblock so that interrupts are enabled as we enter the function
// this is done to make sure that we can run low counts in the compare registers, for example, 
// a count of 1 could cause an interrupts 1 processor clock cycle after this interrupt is called 
// sinse it's called at bottom, and sinse this has a fair amount of code in it, it's good to let the 
// compare interrupts interrupt this interrupt (same with the UART and timer0 interrupts)
ISR(TIMER1_OVF_vect, ISR_NOBLOCK )
{
#if defined  PWM_HLT  || defined PWM_MASH || defined PWM_KETTLE || defined PWM_STEAM || defined PWM_PUMP1 || defined PWM_PUMP2
    //count the number of times this has been called 
    timer1_overflow_count++;
	for (byte i = 0; i <= LAST_HEAT_OUTPUT + NUM_PWM_PUMPS; i++)
	{
		if (cycleStartPump[i] == 0) cycleStart[i] = timer1_overflow_count;
		//if our period just ended, update to when the next period ends
	}

	byte cachedOutput;
#if defined PWM_HLT  || defined PWM_MASH || defined PWM_KETTLE || defined PWM_STEAM
	for (byte i = 0; i <= LAST_HEAT_OUTPUT; i++)
	{
		cachedOutput = vessels[i]->getOutput();
		if ((timer1_overflow_count - cycleStart[i]) > cachedOutput)
			cycleStart[cycleStartIndex] += cachedOutput;

        vessels[i]->updatePWMOutput(cachedOutput >= timer1_overflow_count - cycleStart[i])
			&& timer1_overflow_count != cycleStart[i])));
    }
#endif
#if defined PWM_PUMP1 || defined PWM_PUMP2
	for (byte i = 0; i <= NUM_PWM_PUMPS; i++)
	{
		cachedOutput = flowController[i]->getOutput();
		if ((timer1_overflow_count - cycleStart[i+LAST_HEAT_OUTPUT]) > cachedOutput)
			cycleStart[i] += cachedOutput;

		flowController[i]->updatePWMOutput(cachedOutput >= timer1_overflow_count - cycleStart[i+LAST_HEAT_OUTPUT])
			&& timer1_overflow_count != cycleStart[i+LAST_HEAT_OUTPUT])));
	}
#endif
}

#endif //PWM_BY_TIMER


void pinInit() {
  #ifdef ALARM_PIN
    alarmPin.setup(ALARM_PIN, OUTPUT);
  #endif
  
 
  #ifdef HEARTBEAT
    hbPin.setup(HEARTBEAT_PIN, OUTPUT);
  #endif
  
  #ifdef DIGITAL_INPUTS
    #if DIGIN_COUNT > 0
      digInPin[0].setup(DIGIN1_PIN, INPUT);
    #endif
    #if DIGIN_COUNT > 1
      digInPin[1].setup(DIGIN2_PIN, INPUT);
    #endif
    #if DIGIN_COUNT > 2
      digInPin[2].setup(DIGIN3_PIN, INPUT);
    #endif
    #if DIGIN_COUNT > 3
      digInPin[3].setup(DIGIN4_PIN, INPUT);
    #endif
    #if DIGIN_COUNT > 4
      digInPin[4].setup(DIGIN5_PIN, INPUT);
    #endif
    #if DIGIN_COUNT > 5
      digInPin[5].setup(DIGIN6_PIN, INPUT);
    #endif
  #endif
}

void pidInit() {
  //note that the PIDCycle for the 8khz outputs is set to 10 because the TOP of the counter/timer is set to 1000
  // this means that after it is multiplied by the PIDLIMIT it will be the proper value to give you the desired % output
  // it also makes the % calculations work properly in the log, UI, and other area's. 
  
  
  #ifdef DEBUG_PID_GAIN
    for (byte vessel = VS_HLT; vessel <= NUM_VESSELS; vessel++) logDebugPIDGain(vessel);
  #endi
}

void resetOutputs() {
  actProfiles = 0;
  
  for (byte i = VS_HLT; i <= LAST_HEAT_OUTPUT; i++)
	  vessels[i]->setSetpoint(0);
#if defined PID_PUMP1 || defined PID_PUMP2
  flowController[0]->setTargetOutput(0);
  flowController[1]->setTargetOutput(0);
#endif
  updateValves();
}

void resetHeatOutput(byte vessel) {
  vessels[vessel]->setSetpoint(0);
  updateValves();
}  

/*
 * Things get a little complex here, so let's break it down.
 * processHeatOutputs() is called by the main brewCore() loop.
 *    Processes the RIMS_MLT_SETPOINT_DELAY, which waits a number of
 *      milliseconds before enabling the setpoint for RIMS.
 *    Calls boilController() which manages the auto boil process. If
 *      auto boil is active it may set PIDOutput[VS_KETTLE]
 *    Then it loops through all of the vessels (using HEAT_OUTPUTS_COUNT
 *      as a maximum) and calls their output update function.
 *      Skips various vessels depending on certain conditions.
 */       
void processHeatOutputs() {
  //Process Heat Outputs
  #ifdef RIMS_MLT_SETPOINT_DELAY
    if(timetoset <= millis() && timetoset != 0){
      RIMStimeExpired = 1;
      timetoset = 0;
      vessels[VS_MASH]->setSetpoint(getProgMashTemp(stepProgram[steptoset], steptoset - 5));
    }
  #endif
	unsigned int timestamp = millis();
  for (int vesselIndex = 0; vesselIndex < HEAT_OUTPUTS_COUNT; vesselIndex++) {
    #ifdef USEPWM  
	  if (cycleStart[vesselIndex] == 0 && !vessel[vesselIndex]->isPWM()) cycleStart[vesselIndex] = timestamp;
	#else
	   if (cycleStart[vesselIndex] == 0) cycleStart[vesselIndex] = timestamp;
    #endif
    
	  vessels[vesselIndex]->updateOutput();
    }
}


  void updateValves() {
#ifdef PVOUT
	  setValves(computeValveBits());
#endif
  }
  
  void setValves(unsigned long vlvBits) {
    if (vlvBits != Valves.get()) {
      Valves.set(vlvBits);
      //Mirror outputs to Modbus
      #ifdef PVOUT_TYPE_MODBUS
        if (ValvesMB[0])
          ValvesMB[0]->set(vlvBits >> (ValvesMB[0]->offset()));
        
        if (ValvesMB[1])
          ValvesMB[1]->set(vlvBits >> (ValvesMB[1]->offset()));
      #endif
    }
  }

  void processAutoValve() {
    if (autoValve[AV_CHILL]) {
      //Needs work
      /*
      //If Pumping beer
      if (vlvConfigIsActive(VLV_CHILLBEER)) {
        //Cut beer if exceeds pitch + 1
        if (temp[TS_BEEROUT] > pitchTemp + 1.0) bitClear(actProfiles, VLV_CHILLBEER);
      } else {
        //Enable beer if chiller H2O output is below pitch
        //ADD MIN DELAY!
        if (temp[TS_H2OOUT] < pitchTemp - 1.0) bitSet(actProfiles, VLV_CHILLBEER);
      }
      
      //If chiller water is running
      if (vlvConfigIsActive(VLV_CHILLH2O)) {
        //Cut H2O if beer below pitch - 1
        if (temp[TS_BEEROUT] < pitchTemp - 1.0) bitClear(actProfiles, VLV_CHILLH2O);
      } else {
        //Enable H2O if chiller H2O output is at pitch
        //ADD MIN DELAY!
        if (temp[TS_H2OOUT] >= pitchTemp) bitSet(actProfiles, VLV_CHILLH2O);
      }
      */
    }
  }
#endif //#ifdef PVOUT
  
unsigned long computeValveBits() {
  if (estop) return 0;
  unsigned long vlvBits = 0;
  
  //Force certain valve profiles off based on trigger pins if enabled
  if (TriggerPin[TRIGGER_SPARGEMAX] != NULL) if(TriggerPin[TRIGGER_SPARGEMAX]->get()) bitClear(actProfiles, VLV_SPARGEIN);
  if (TriggerPin[TRIGGER_HLTMIN] != NULL) if(!TriggerPin[TRIGGER_HLTMIN]->get()) bitClear(actProfiles, VLV_HLTHEAT);
  if (TriggerPin[TRIGGER_MASHMIN] != NULL) if(!TriggerPin[TRIGGER_MASHMIN]->get()) bitClear(actProfiles, VLV_MASHHEAT);
  if (TriggerPin[TRIGGER_KETTLEMIN] != NULL) if(!TriggerPin[TRIGGER_KETTLEMIN]->get()) bitClear(actProfiles, VLV_KETTLEHEAT);
  
  for (byte i = 0; i < NUM_VLVCFGS; i++) {
    if (bitRead(actProfiles, i)) {
      vlvBits |= vlvConfig[i];
    }
  }
  #ifdef RGBIO8_ENABLE
  // Build the softswitch masks
  // Any bits set to 1 on offMask will force the corresponding valve off.
  unsigned long offMask = 0;
  // Any bits set to 1 on onMask will force the corresponding valve on.
  unsigned long onMask = 0;
  //Start at NUM_PID_PUMPS so we don't affect the PID-controlled pumps
  for (int i = NUM_PID_PUMPS; i < PVOUT_COUNT; i++) {
    if (softSwitchPv[i] == SOFTSWITCH_OFF) {
      offMask |= (1 << i);
    }
    else if (softSwitchPv[i] == SOFTSWITCH_ON) {
      onMask |= (1 << i);
    }
  }
  // Apply the masks to the pre-computed valve bits.
  offMask = ~offMask;
  vlvBits &= offMask;
  vlvBits |= onMask;
  #endif
  return vlvBits;
}

boolean vlvConfigIsActive(byte profile) {
  //An empty valve profile cannot be active
  if (!vlvConfig[profile]) return 0;
  return bitRead(actProfiles, profile);
}

//Map AutoValve Profiles to Vessels
byte vesselAV(byte vessel) {
  if (vessel == VS_HLT) return AV_HLT;
  else if (vessel == VS_MASH) return AV_MASH;
  else if (vessel == VS_KETTLE) return AV_KETTLE;
}

byte vesselVLVHeat(byte vessel) {
  if (vessel == VS_HLT) return VLV_HLTHEAT;
  else if (vessel == VS_MASH) return VLV_MASHHEAT;
  else if (vessel == VS_KETTLE) return VLV_KETTLEHEAT;
}

byte vesselVLVIdle(byte vessel) {
  if (vessel == VS_HLT) return VLV_HLTIDLE;
  else if (vessel == VS_MASH) return VLV_MASHIDLE;
  else if (vessel == VS_KETTLE) return VLV_KETTLEIDLE;
}

pin * vesselMinTrigger(byte vessel) {
  if (vessel == VS_HLT) return TriggerPin[TRIGGER_HLTMIN];
  else if (vessel == VS_MASH) return TriggerPin[TRIGGER_MASHMIN];
  else if (vessel == VS_KETTLE) return TriggerPin[TRIGGER_KETTLEMIN];
  else return NULL;
}

byte autoValveBitmask(void) {
  byte modeMask = 0;
  for (byte i = AV_FILL; i < NUM_AV; i++)
    if (autoValve[i]) modeMask |= 1<<i;
  if (flowController[0]->getSwitch() == SOFTSWITCH_AUTO) modeMask |= 1 << AV_SPARGE_IN;
  if (flowController[1]->getSwitch() == SOFTSWITCH_AUTO) modeMask |= 1 << AV_SPARGE_OUT;
  if (flowController[0]->getSwitch() == SOFTSWITCH_AUTO && flowController[1]->getSwitch() == SOFTSWITCH_AUTO) modeMask |= 1 << AV_FLYSPARGE;
  if (fillController[0]->getSwitch() == SOFTSWITCH_AUTO || fillController[1]->getSwitch() == SOFTSWITCH_AUTO) modeMask |= 1 << AV_FILL;
  if (vessels[VS_HLT]->getSwitch() == SOFTSWITCH_AUTO) modeMask |= 1 << AV_HLT;
  if (vessels[VS_MASH]->getSwitch() == SOFTSWITCH_AUTO) modeMask |= 1 << AV_MASH;
  if (vessels[VS_KETTLE]->getSwitch() == SOFTSWITCH_AUTO) modeMask |= 1 << AV_KETTLE;
  return modeMask;
}

byte getHeatPower (byte vessel) {
	return vessels[vessel]->getOutput();
}

