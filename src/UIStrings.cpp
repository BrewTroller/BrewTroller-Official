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
const char MANUAL_S[] PROGMEM = "Manual";
const char SET_TIMER[] PROGMEM = "Set Timer";
const char PAUSE_TIMER[] PROGMEM = "Pause Timer";
const char START_TIMER[] PROGMEM = "Start Timer";
const char COLON_SPACE[] PROGMEM = ": ";


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
#ifdef DIRECT_FIRED_RIMS
    const char BADTEMP[] PROGMEM = "12345678----";
    const char OCTOTHORPE[] PROGMEM = "#";
    const char ASTERISK[] PROGMEM = "*";
    const char DASH[] PROGMEM = "-";
#endif

//Sparge step strings
const char SPARGE[] PROGMEM = "Sparge";
const char ALLOFF[] PROGMEM = "All Off";
const char FILLBOTH[] PROGMEM = "Fill Both";
const char FLYSPARGE[] PROGMEM = "Fly Sparge";

//Boil step strings
const char BOIL[] PROGMEM = "Boil";
const char BOIL_AUTO[] PROGMEM = "  Auto";
const char BOIL_MAN[] PROGMEM = "Manual";

//Chill step state strings
const char CHILL[] PROGMEM = "Chill";
const char BEER[] PROGMEM = "Beer";
const char H20[] PROGMEM = "H20";
const char IN[] PROGMEM = "In";
const char OUT[] PROGMEM = "Out";
const char CHILLNORM[] PROGMEM = "Chill Both";

//AUX Screen Strings
const char AUX_TEMPS[] PROGMEM = "AUX Temps";
const char AUX[] PROGMEM = "AUX";
const char ONE[] = "AUX1";
const char TWO[] = "AUX2";
#ifndef DIRECT_FIRED_RIMS
const char THREE[] = "AUX3";
#endif

//Main Menu Strings
const char EDIT_PROGRAM[] PROGMEM = "Edit Program";
const char START_PROGRAM[] PROGMEM = "Start Program";
const char VALVE_ON[] PROGMEM = ": On";
const char VALVE_OFF[] PROGMEM = ": Off";
const char RESET_ALL[] PROGMEM = "Reset All";
const char SYSTEM_SETUP[] PROGMEM = "System Setup";
const char DRAIN_ERR1[] PROGMEM = "Cannot drain while";
const char DRAIN_ERR2[] PROGMEM = "mash or boil zone";
const char DRAIN_ERR3[] PROGMEM = "is active";

//Fill Menu Strings
const char AUTO_FILL[] PROGMEM = "Auto Fill";
const char HLT_TARGET[] PROGMEM = "HLT Target";
const char MASH_TARGET[]  PROGMEM = "Mash Target";
const char HLT_TARGET_VOL[] PROGMEM = "HLT Target Vol";
const char MASH_TARGET_VOL[] PROGMEM = "Mash Target Vol";

//Mash Menu Strings
const char HLT_SETPOINT[] PROGMEM = "HLT Setpoint";
const char MASH_SETPOINT[] PROGMEM = "Mash Setpoint";
const char MASH_TIMER[] PROGMEM = "Mash Timer";

//Sparge Menu Strings
const char AUTO_IN[] PROGMEM = "Auto In";
const char AUTO_OUT[] PROGMEM = "Auto Out";
const char AUTO_FLY[] PROGMEM = "Auto Fly";
const char KETTLE_TARGET[] PROGMEM = "Kettle Target";
const char KETTLE_TARGET_VOL[] PROGMEM = "Kettle Target Vol";

//Boil Menu Strings
const char BOIL_CTRL[] PROGMEM = "Boil Ctrl: ";
const char BOIL_TEMP[] PROGMEM = "Boil Temp";
const char BOIL_POWER[] PROGMEM = "Boil Power";
const char BOIL_TIMER[] PROGMEM = "Boil Timer";

//Program Menu Strings
const char PROG_NAME[] PROGMEM = "Program Name:";
const char EDIT_PROG[] PROGMEM = "Edit Program";
const char GRAIN_TEMP[] PROGMEM = "Grain Temp";
const char START[] PROGMEM = "Start";
const char DELAY_START[] PROGMEM = "Delay Start";
const char START_FAILED[] PROGMEM = "Program start failed";
const char START_ERR1[] PROGMEM = "Cannot start program";
const char START_ERR2[] PROGMEM = "while mash zone is";
const char START_ERR3[] PROGMEM = "active.";

//Vessel Strings
const char TITLE_VS_HLT[] PROGMEM = "HLT";
const char TITLE_VS_MASH[] PROGMEM = "Mash";
const char TITLE_VS_KETTLE[] PROGMEM = "Kettle";

//E-Stop Strings
const char ESTOP_TRIGGERED[] PROGMEM = "E-Stop Triggered";
const char CLEAR_ALARM[] PROGMEM = ">Clear Alarm";
const char CLEAR_ESTOP[] PROGMEM = " Clear E-Stop";

//Program Strings
const char FAILED_ADV1[] PROGMEM = "Failed to advance";
const char FAILED_ADV2[] PROGMEM = "program.";

//Direct Fired RIMS Strings
#ifdef DIRECT_FIRED_RIMS
    const char AT[] PROGMEM = "At";
    const char SET[] PROGMEM = "Set";
    const char RIMS[] PROGMEM = "RIMS";
#endif

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