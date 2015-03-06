/*
   Copyright (C) 2015 BrewTroller

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
Documentation, Forums and more information available at http://www.brewtroller.com
*/

#include <avr/pgmspace.h>
#include "UIStrings.h"

#ifndef NOUI

//Generic Menu strings
const char OK[] PROGMEM = "Ok";
const char CANCEL[] PROGMEM = "Cancel";
const char EXIT[] PROGMEM = "Exit";
const char ABORT[] PROGMEM = "Abort";
const char MENU[] PROGMEM = "Menu";
const char SPACE[] PROGMEM = " ";
const char MISSING_CONFIG[] PROGMEM = "Missing Config";
const char INIT_EEPROM[] PROGMEM = "Initialize EEPROM";
const char PLEASE_WAIT[] PROGMEM = "Please Wait...";
const char CONTINUE[] PROGMEM = "Continue";
const char TARGET[] PROGMEM = "Target";
const char ACTUAL[] PROGMEM = "Actual";
const char GREATER_SYM[] PROGMEM = ">";
const char LESS_SYM[] PROGMEM = "<";
const char COLON[] PROGMEM = ":";
const char ON[] PROGMEM = "On";
const char OFF[] PROGMEM = "Off";
const char TEMPBLANK[] PROGMEM = "----";
const char VOLBLANK[] PROGMEM = "--.---";
const char AUTO_S[] PROGMEM = "Auto";


//Home screen strings
const char BUILD[] PROGMEM = "Build";
const char BT_URL[] PROGMEM = "www.brewtroller.com";

//Fill screen strings
const char FILL[] PROGMEM = "Fill";
const char REFILL[] PROGMEM = "Refill";

//Valve profile name strings
const char FILLHLT[] PROGMEM = "Fill HLT";
const char FILLMASH[] PROGMEM = "Fill Mash";
const char ADDGRAIN[] PROGMEM = "Add Grain";
const char MASHHEAT[] PROGMEM = "Mash Heat";
const char MASHIDLE[] PROGMEM = "Mash Idle";
const char SPARGEIN[] PROGMEM = "Sparge In";
const char SPARGEOUT[] PROGMEM = "Sparge Out";
const char BOILADDS[] PROGMEM = "Boil Additions";
const char KETTLELID[] PROGMEM = "Kettle Lid";
const char CHILLH2O[] PROGMEM = "Chill H2O";
const char CHILLBEER[] PROGMEM = "Chill Beer";
const char BOILRECIRC[] PROGMEM = "Boil Recirc";
const char DRAIN[] PROGMEM = "Drain";
const char HLTHEAT[] PROGMEM = "HLT Heat";
const char HLTIDLE[] PROGMEM = "HLT Idle";
const char KETTLEHEAT[] PROGMEM = "Kettle Heat";
const char KETTLEIDLE[] PROGMEM = "Kettle Idle";
const char USER1[] PROGMEM = "User Valve 1";
const char USER2[] PROGMEM = "User Valve 2";
const char USER3[] PROGMEM = "User Valve 3";

//Grain in step strings
// TODO: this needs to be clarified, what is the difference between Grain in and Dough In?
const char GRAININ[] PROGMEM = "Grain In";

//Mash step strings
const char DELAY[] PROGMEM = "Delay";
const char PREHEAT[] PROGMEM = "Preheat";
const char DOUGHIN[] PROGMEM = "Dough In";
const char ACID[] PROGMEM = "Acid";
const char PROTEIN[] PROGMEM = "Protein";
const char SACCH[] PROGMEM = "Sacch";
const char SACCH2[] PROGMEM = "Sacch2";
const char MASHOUT[] PROGMEM = "Mash Out";
const char ENDMASH[] PROGMEM = "End Mash";

//Sparge step strings
const char SPARGE[] PROGMEM = "Sparge";
const char ALLOFF[] PROGMEM = "All Off";
const char FILLBOTH[] PROGMEM = "Fill Both";
const char FLYSPARGE[] PROGMEM = "Fly Sparge";

//Boil step strings
const char BOIL[] PROGMEM = "Boil";

//Chill step state strings
const char CHILL[] PROGMEM = "Chill";
const char BEER[] PROGMEM = "Beer";
const char H20[] PROGMEM = "H20";
const char IN[] PROGMEM = "In";
const char OUT[] PROGMEM = "Out";
const char CHILLNORM[] PROGMEM = "Chill Both";

//Vessel Strings
const char TITLE_VS_HLT[] PROGMEM = "HLT";
const char TITLE_VS_MASH[] PROGMEM = "Mash";
const char TITLE_VS_KETTLE[] PROGMEM = "Kettle";

#ifdef PID_FLOW_CONTROL
    const char TITLE_VS_PUMP[] PROGMEM = "Pump";
#elif defined USESTEAM
    const char TITLE_VS_STEAM[] PROGMEM = "Steam";
#endif

#ifndef UI_NO_SETUP

    const char* const TITLE_VS[] PROGMEM = {
        TITLE_VS_HLT,
        TITLE_VS_MASH,
        TITLE_VS_KETTLE

        #ifdef PID_FLOW_CONTROL
      , TITLE_VS_PUMP
        #elif defined USESTEAM
      , TITLE_VS_STEAM
        #endif
    };

    const char PIDCYCLE[] PROGMEM = " PID Cycle";
    const char PIDGAIN[] PROGMEM = " PID Gain";
    const char HYSTERESIS[] PROGMEM = " Hysteresis";

    #ifdef PID_FLOW_CONTROL
        const char PUMPFLOW[] PROGMEM = "Pump Flow Rate";
    #elif defined USESTEAM
        const char STEAMPRESS[] PROGMEM = "Steam Target";
        const char STEAMSENSOR[] PROGMEM = "Steam Sensor Sens";
        const char STEAMZERO[] PROGMEM = "Steam Zero Calib";
    #endif

    const char CAPACITY[] PROGMEM = " Capacity";
    const char DEADSPACE[] PROGMEM = " Dead Space";
    const char CALIBRATION[] PROGMEM = " Calibration";

#endif //UI_NO_SETUP

const char HLTDESC[] PROGMEM = "Hot Liquor Tank";
const char MASHDESC[] PROGMEM = "Mash Tun";
const char SEC[] PROGMEM = "s";

#ifdef USEMETRIC
    const char VOLUNIT[] PROGMEM = "l";
    const char WTUNIT[] PROGMEM = "kg";
    const char TUNIT[] PROGMEM = "C";
    #ifdef PID_FLOW_CONTROL
        const char PUNIT[] PROGMEM = "0.1*l/m";
    #else
        const char PUNIT[] PROGMEM = "kPa";
    #endif
#else
    const char VOLUNIT[] PROGMEM = "gal";
    const char WTUNIT[] PROGMEM = "lb";
    const char TUNIT[] PROGMEM = "F";
    #ifdef PID_FLOW_CONTROL
        const char PUNIT[] PROGMEM = "0.1*q/m";
    #else
        const char PUNIT[] PROGMEM = "psi";
    #endif
#endif //USEMETRIC

#endif