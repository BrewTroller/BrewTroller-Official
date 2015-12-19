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

#ifdef PID_FLOW_CONTROL 
  #define LAST_HEAT_OUTPUT VS_PUMP // not this is mostly done for code readability as VS_PUMP = VS_STEAM
#else
  #ifdef USESTEAM
    #define LAST_HEAT_OUTPUT VS_STEAM
  #elif defined DIRECT_FIRED_RIMS
    #define LAST_HEAT_OUTPUT VS_STEAM
  #else
    #define LAST_HEAT_OUTPUT VS_KETTLE
  #endif
#endif

#ifdef PWM_BY_TIMER
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
    //set TOP as 1000, which makes the overflow on return to bottom for this mode happen ever 
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
// sense it's called at bottom, and sense this has a fair amount of code in it, it's good to let the 
// compare interrupts interrupt this interrupt (same with the UART and timer0 interrupts)
ISR(TIMER1_OVF_vect, ISR_NOBLOCK )
{
    //count the number of times this has been called 
    timer1_overflow_count++;
    for(byte i = 0; i <= LAST_HEAT_OUTPUT; i++)
    {
        // if PID is enabled, and NOT one of the 8khz PWM outputs then we can use this
        if(PIDEnabled[i])
        {
            //init the cyclestart counter if needed
            if(cycleStart[i] == 0 ) cycleStart[i] = timer1_overflow_count; 
            //if our period just ended, update to when the next period ends
            if((timer1_overflow_count - cycleStart[i]) > PIDOutputCountEquivalent[i][0]) 
                cycleStart[i] += PIDOutputCountEquivalent[i][0];
            //check to see if the pin should be high or low (note when our 16 bit integer wraps we will have 1 period where 
            // the PWM % if cut short, because from the time of wrap until the next period 
            if ((!estop) && PIDOutputCountEquivalent[i][1] >= timer1_overflow_count - cycleStart[i] 
                  && timer1_overflow_count != cycleStart[i]) 
                heatPin[i].set(HIGH); else heatPin[i].set(LOW);
        }
    }
}

#endif //PWM_BY_TIMER


void pinInit() {
  #ifdef ALARM_PIN
    alarmPin.setup(ALARM_PIN, OUTPUT);
  #endif
  
  #ifdef DIRECT_FIRED_RIMS
    heatPin[VS_STEAM].setup(STEAMHEAT_PIN, OUTPUT);
  #endif

  #ifdef USESTEAM
    #ifdef STEAMHEAT_PIN
      heatPin[VS_STEAM].setup(STEAMHEAT_PIN, OUTPUT);
    #endif
  #endif

  #ifdef PID_FLOW_CONTROL
    #ifdef PWMPUMP_PIN
      heatPin[VS_PUMP].setup(PWMPUMP_PIN, OUTPUT);
    #endif
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
  #ifdef PID_FLOW_CONTROL
    PIDCycle = 1; // for PID pump flow the STEAM heat output is set to a fixed 10hz signal with 100 step outputs. 
  #endif
  
  #ifdef PID_FLOW_CONTROL
    #ifdef USEMETRIC
      pid.SetInputLimits(0, 255000); // equivalent of 25.5 LPM (255 * 100)
    #else
      pid.SetInputLimits(0, 6375); // equivalent of 6.375 GPM (255 * 25)
    #endif
    pid.SetOutputLimits(PID_FLOW_MIN, PIDCycle * PIDLIMIT_STEAM);
    pid.SetTunings(pid.GetP_Param(), pid.GetI_Param(), pid.GetD_Param());
    #ifdef PID_CONTROL_MANUAL
      pid.SetMode(MANUAL);
    #else
      pid.SetMode(AUTO);
    #endif
    pid.SetSampleTime(FLOWRATE_READ_INTERVAL);
    #ifdef PID_CONTROL_MANUAL
      nextcompute = millis() + FLOWRATE_READ_INTERVAL;
    #endif
  #else
    #ifdef USEMETRIC
      pid.SetInputLimits(0, 50000000 / steamPSens);
    #else
      pid.SetInputLimits(0, 7250000 / steamPSens);
    #endif
    pid.SetOutputLimits(0, PIDCycle * PIDLIMIT_STEAM);
    pid.SetTunings(pid.GetP_Param(), pid.GetI_Param(), pid.GetD_Param());
    pid.SetMode(PID::AUTO_MODE);
    pid.SetSampleTime(PID_CYCLE_TIME);
  #endif

  #ifdef DEBUG_PID_GAIN
    for (byte vessel = VS_HLT; vessel <= VS_STEAM; vessel++) logDebugPIDGain(vessel);
  #endif
}

void resetOutputs() {
  actProfiles = 0;
  
  for (byte i = VS_HLT; i <= LAST_HEAT_OUTPUT; i++)
	  vessels[i]->setSetpoint(0);

  updateValves();
}

void resetHeatOutput(byte vessel) {
  #ifdef PWM_BY_TIMER
    uint8_t oldSREG;
  #endif
  vessels[vessel]->setSetpoint(0);
  
  #ifdef PWM_BY_TIMER
    // need to disable interrupts so a write into here can finish before an interrupt can come in and read it
    oldSREG = SREG;
    cli();
    //if we are not a 8K output then we can set it to 0, but if we are we need to set it to 1000 to make the duty cycle 0
    PIDOutputCountEquivalent[vessel][1] = 0;
    SREG = oldSREG; // restore interrupts
  #endif
}  

//TODO: Make the below section work with new vessel definitions
#if defined PID_FLOW_CONTROL && defined PID_CONTROL_MANUAL
void processPID_FLOW_CONTROL(byte vessel) {
  if(vessel == VS_PUMP){ //manual control if PID isnt working due to long sample times or other reasons
    millistemp = millis();
    if(millistemp >= nextcompute){
      nextcompute += FLOWRATE_READ_INTERVAL;
      if(setpoint[vessel] == 0) PIDOutput[vessel] = 0;
      else{
        if((long)setpoint[vessel] - flowRate[VS_KETTLE] > 100){
          additioncount[0]++;
          additioncount[1] = 0;
          if(additioncount[0] > 5){    // this is here to break a case where adding 10 causes a change of 100 but lowering 10 causes a change of 100 off the setpoint and we just oscilate. 
            additioncount[0] = 0;
            PIDOutput[vessel] += 5;
          }
          else PIDOutput[vessel] += 10;
        }
        else if((long)setpoint[vessel] - flowRate[VS_KETTLE] < -100){
          additioncount[0]++;
          additioncount[1] = 0;
          if(additioncount[0] > 5){    // this is here to break a case where adding 10 causes a change of 100 but lowering 10 causes a change of 100 off the setpoint and we just oscilate. 
           additioncount[0] = 0;
           PIDOutput[vessel] -= 5;
         }
         else PIDOutput[vessel] -= 10;
        }
        else if((long)setpoint[vessel] - flowRate[VS_KETTLE] > 50){ 
          additioncount[0] = 0;
          additioncount[1]++;
          if(additioncount[0] > 5){    // this is here to break a case where adding 5 causes a change of 50 but lowering 5 causes a change of 50 off the setpoint and we just oscilate. 
            additioncount[1] = 0;
            PIDOutput[vessel] += 1;
          }
          else PIDOutput[vessel] += 5;
        }
        else if((long)setpoint[vessel] - flowRate[VS_KETTLE] < -50){ 
          additioncount[0] = 0;
          additioncount[1]++;
          if(additioncount[0] > 5){    // this is here to break a case where adding 5 causes a change of 50 but lowering 5 causes a change of 50 off the setpoint and we just oscilate. 
            additioncount[1] = 0;
            PIDOutput[vessel] -= 1;
          }
          else PIDOutput[vessel] -= 5;
        }
        else if((long)setpoint[vessel] - flowRate[VS_KETTLE] > 10) PIDOutput[vessel] += 1;
        else if((long)setpoint[vessel] - flowRate[VS_KETTLE] < -10) PIDOutput[vessel] -= 1;
        
        if(PIDOutput[vessel] > pid[vessel].GetOUTMax()) PIDOutput[vessel] = pid[vessel].GetOUTMax();
        else if(PIDOutput[vessel] < pid[vessel].GetOUTMin()) PIDOutput[vessel] = pid[vessel].GetOUTMin();
      }
    }
  }
}
#endif // defined PID_FLOW_CONTROL && defined PID_CONTROL_MANUAL

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
  
  for (int vesselIndex = 0; vesselIndex < HEAT_OUTPUTS_COUNT; vesselIndex++) {
    #ifndef PWM_BY_TIMER
  
      if (cycleStart[vesselIndex] == 0) cycleStart[vesselIndex] = timestamp;
      
    #endif
    
	  vessels[vesselIndex]->updateOutput());
	#ifdef RGBIO8_ENABLE
    if (softSwitchHeat[vesselIndex] == SOFTSWITCH_AUTO) {
      // Auto
    #endif
    
    }
}

#ifdef PVOUT
  void updateValves() {
    setValves(computeValveBits());
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
    #ifdef HLT_MIN_REFILL
      unsigned long HLTStopVol;
    #endif
    //Do Valves
    if (autoValve[AV_FILL]) {
      if (vessels[VS_HLT]->getVolume() < vessels[VS_HLT]->getTargetVolume()) bitSet(actProfiles, VLV_FILLHLT);
        else bitClear(actProfiles, VLV_FILLHLT);
        
      if (vessels[VS_MASH]->getVolume() < vessels[VS_MASH]->getTargetVolume()) bitSet(actProfiles, VLV_FILLMASH);
        else bitClear(actProfiles, VLV_FILLMASH);
    }
    
    //HLT/MASH/KETTLE AV Logic
    for (byte i = VS_HLT; i <= VS_KETTLE; i++) {
      byte vlvHeat = vesselVLVHeat(i);
      byte vlvIdle = vesselVLVIdle(i);
      if (autoValve[vesselAV(i)]) {
		  if (vessels[i]->getOutput()) {
          if (vlvConfigIsActive(vlvIdle)) bitClear(actProfiles, vlvIdle);
          if (!vlvConfigIsActive(vlvHeat)) bitSet(actProfiles, vlvHeat);
        } else {
          if (vlvConfigIsActive(vlvHeat)) bitClear(actProfiles, vlvHeat);
          if (!vlvConfigIsActive(vlvIdle)) bitSet(actProfiles, vlvIdle); 
        }
      }
    }
    
    if (autoValve[AV_SPARGEIN]) {
      if (vessels[VS_HLT]->getVolume() > vessels[VS_HLT]->getTargetVolume()) bitSet(actProfiles, VLV_SPARGEIN);
        else bitClear(actProfiles, VLV_SPARGEIN);
    }
    if (autoValve[AV_SPARGEOUT]) {
      if (vessels[VS_KETTLE]->getVolume() < vessels[VS_KETTLE]->getTargetVolume()) bitSet(actProfiles, VLV_SPARGEOUT);
      else bitClear(actProfiles, VLV_SPARGEOUT);
    }
    if (autoValve[AV_FLYSPARGE]) {
		if (vessels[VS_KETTLE]->getVolume() < vessels[VS_KETTLE]->getTargetVolume()) {
#ifdef SPARGE_IN_PUMP_CONTROL
			long hltVol = vessels[VS_HLT]->getVolume();
			if ((long)vessels[VS_KETTLE]->getVolume() - (long)prevSpargeVol[0] >= SPARGE_IN_HYSTERESIS)
			{
#ifdef HLT_MIN_REFILL
				HLTStopVol = (SpargeVol > HLT_MIN_REFILL_VOL ? getVolLoss(VS_HLT) : (HLT_MIN_REFILL_VOL - SpargeVol));
				if (hltVol > HLTStopVol + 20) //I have no idea where this random 20 came from here... Some sort of safety factor in min vols?
#else
				if (hltVol > vessels[VS_HLT]->getDeadspace() + 20)
#endif
					bitSet(actProfiles, VLV_SPARGEIN);
				prevSpargeVol[0] = hltVol
			}
#ifdef HLT_FLY_SPARGE_STOP
			else if ((long)prevSpargeVol[1] - (long)hltVol >= SPARGE_IN_HYSTERESIS || hltVol < HLT_FLY_SPARGE_STOP_VOLUME + 20)
#else
			else if ((long)prevSpargeVol[1] - (long)hltVol >= SPARGE_IN_HYSTERESIS || hltVol < vessels[VS_HLT]->getDeadspace() + 20)
#endif
          {
             bitClear(actProfiles, VLV_SPARGEIN);
			 prevSpargeVol[1] = vessels[VS_HLT]->getVolume();
          }
        #else
          bitSet(actProfiles, VLV_SPARGEIN);
        #endif
        bitSet(actProfiles, VLV_SPARGEOUT);
      } else {
        bitClear(actProfiles, VLV_SPARGEIN);
        bitClear(actProfiles, VLV_SPARGEOUT);
      }
    }
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
  for (int i = 0; i < PVOUT_COUNT; i++) {
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
  return modeMask;
}

byte getHeatPower (byte vessel) {
	return vessels[vessel]->getOutput();
}

