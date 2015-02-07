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

/*
Compiled on Arduino-0022 (http://arduino.cc/en/Main/Software)
  With Sanguino Software "Sanguino-0018r2_1_4.zip" (http://code.google.com/p/sanguino/downloads/list)

  Using the following libraries:
    PID  v0.6 (Beta 6) (http://www.arduino.cc/playground/Code/PIDLibrary)
    OneWire 2.0 (http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip)
    Encoder by CodeRage ()
    FastPin and modified LiquidCrystal with FastPin by CodeRage (http://www.brewtroller.com/forum/showthread.php?t=626)
*/



//*****************************************************************************************************************************
// BEGIN CODE
//*****************************************************************************************************************************

#include <avr/pgmspace.h>
#include <PID_Beta6.h>
#include <pin.h>
#include "HWProfile.h"
#include "Config.h"
#include "PVOut.h"
#include "UI_LCD.h"
#include "BrewCore.h"
#include "EEPROM.h"
#include "Events.h"
#include "UI.h"
#include "Com.h"
#include "Outputs.h"
#include "Temp.h"
#include "StepLogic.h"

void(* softReset) (void) = 0;

//**********************************************************************************
// Compile Time Logic
//**********************************************************************************

//Enable Mash Avergaing Logic if any Mash_AVG_AUXx options were enabled
#if defined MASH_AVG_AUX1 || defined MASH_AVG_AUX2 || defined MASH_AVG_AUX3
  #define MASH_AVG
#endif

#ifndef STRIKE_TEMP_OFFSET
  #define STRIKE_TEMP_OFFSET 0
#endif

#if COM_SERIAL0 == BTNIC || defined BTNIC_EMBEDDED
  #define BTNIC_PROTOCOL
#endif

#if defined BTPD_SUPPORT || defined UI_LCD_I2C || defined TS_ONEWIRE_I2C || defined BTNIC_EMBEDDED || defined RGBIO8_ENABLE
  #define USE_I2C
#endif

#ifdef BOIL_OFF_GALLONS
  #ifdef USEMETRIC
    #define EvapRateConversion 1000
  #else
    #define EvapRateConversion 100
  #endif
#endif

#ifdef USE_I2C
  #include <Wire.h>
#endif

//**********************************************************************************
// Setup
//**********************************************************************************

void setup() {

  #ifdef ADC_REF
	analogReference(ADC_REF);
  #endif

  #ifdef USE_I2C
    Wire.begin(BT_I2C_ADDR);
  #endif
  
  //Log initialization (Log.pde)
  comInit();

  //Pin initialization (Outputs.pde)
  pinInit();


#ifdef PVOUT
  #if defined PVOUT_TYPE_GPIO
    #if PVOUT_COUNT >= 1
      Valves.setup(0, VALVE1_PIN);
    #endif
    #if PVOUT_COUNT >= 2
      Valves.setup(1, VALVE2_PIN);
    #endif
    #if PVOUT_COUNT >= 3
      Valves.setup(2, VALVE3_PIN);
    #endif
    #if PVOUT_COUNT >= 4
      Valves.setup(3, VALVE4_PIN);
    #endif
    #if PVOUT_COUNT >= 5
      Valves.setup(4, VALVE5_PIN);
    #endif
    #if PVOUT_COUNT >= 6
      Valves.setup(5, VALVE6_PIN);
    #endif
    #if PVOUT_COUNT >= 7
      Valves.setup(6, VALVE7_PIN);
    #endif
    #if PVOUT_COUNT >= 8
      Valves.setup(7, VALVE8_PIN);
    #endif
    #if PVOUT_COUNT >= 9
      Valves.setup(8, VALVE9_PIN);
    #endif
    #if PVOUT_COUNT >= 10
      Valves.setup(9, VALVEA_PIN);
    #endif
    #if PVOUT_COUNT >= 11
      Valves.setup(10, VALVEB_PIN);
    #endif
    #if PVOUT_COUNT >= 12
      Valves.setup(11, VALVEC_PIN);
    #endif
    #if PVOUT_COUNT >= 13
      Valves.setup(12, VALVED_PIN);
    #endif
    #if PVOUT_COUNT >= 14
      Valves.setup(13, VALVEE_PIN);
    #endif
    #if PVOUT_COUNT >= 15
      Valves.setup(14, VALVEF_PIN);
    #endif
    #if PVOUT_COUNT >= 16
      Valves.setup(15, VALVEG_PIN);
    #endif
  #endif
  Valves.init();
#endif

  tempInit();
  
  //Check for cfgVersion variable and update EEPROM if necessary (EEPROM.pde)
  checkConfig();

  
  //Load global variable values stored in EEPROM (EEPROM.pde)
  loadSetup();
  
  #ifdef DIGITAL_INPUTS
    //Digital Input Interrupt Setup
    triggerSetup();
  #endif
  
  //PID Initialization (Outputs.pde)
  pidInit();
  
  #ifdef PWM_BY_TIMER
    pwmInit();
  #endif

  //User Interface Initialization (UI.pde)
  //Moving this to last of setup() to allow time for I2CLCD to initialize
  #ifndef NOUI
    uiInit();
  #endif
  
  //Init of program threads will call event handler to set active screen and must be called after uiInit()
  programThreadsInit();
}


//**********************************************************************************
// Loop
//**********************************************************************************

void loop() {
  //User Interface Processing (UI.pde)
  #ifndef NOUI
    uiCore();
  #endif
  
  //Core BrewTroller process code (BrewCore.pde)
  brewCore();
}

