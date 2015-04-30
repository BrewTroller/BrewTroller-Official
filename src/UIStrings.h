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

#ifndef __BT_UI_STRINGS_H__
#define __BT_UI_STRINGS_H__

#include "Config.h"
#include "BrewTroller.h" 
//TODO: Refactor configuration macros from BrewTroller.h to Config.h, and remove this include
#ifndef NOUI

//TODO: Namespace Strings into Top-Level NS (UI::Strings) and into use-case NSes (::MainMenu)

//Generic Menu strings
extern const char OK[];
extern const char CANCEL[];
extern const char EXIT[];
extern const char ABORT[];
extern const char MENU[];
extern const char SPACE[];
extern const char MISSING_CONFIG[];
extern const char INIT_EEPROM[];
extern const char PLEASE_WAIT[];
extern const char CONTINUE[];
extern const char TARGET[];
extern const char ACTUAL[];
extern const char GREATER_SYM[];
extern const char LESS_SYM[];
extern const char ON[];
extern const char OFF[];
extern const char COLON[];
extern const char TEMPBLANK[];
extern const char VOLBLANK[];
extern const char AUTO_S[];
extern const char MANUAL_S[];
extern const char SET_TIMER[];
extern const char PAUSE_TIMER[];
extern const char START_TIMER[];
extern const char COLON_SPACE[];
extern const char PERC_SYM[];

//Home screen strings
extern const char BUILD[];
extern const char BT_URL[];

//Fill screen strings
extern const char FILL[];
extern const char REFILL[];

//Valve profile name strings
extern const char FILLHLT[];
extern const char FILLMASH[];
extern const char ADDGRAIN[];
extern const char MASHHEAT[];
extern const char MASHIDLE[];
extern const char SPARGEIN[];
extern const char SPARGEOUT[];
extern const char BOILADDS[];
extern const char KETTLELID[];
extern const char CHILLH2O[];
extern const char CHILLBEER[];
extern const char BOILRECIRC[];
extern const char DRAIN[];
extern const char HLTHEAT[];
extern const char HLTIDLE[];
extern const char KETTLEHEAT[];
extern const char KETTLEIDLE[];
extern const char USER1[];
extern const char USER2[];
extern const char USER3[];

//Grain in step strings
extern const char GRAININ[];

//Mash step strings
extern const char DELAY[];
extern const char PREHEAT[];
extern const char DOUGHIN[];
extern const char ACID[];
extern const char PROTEIN[];
extern const char SACCH[];
extern const char SACCH2[];
extern const char MASHOUT[];
extern const char ENDMASH[];
#ifdef DIRECT_FIRED_RIMS
    extern const char BADTEMP[];
    extern const char OCTOTHORPE[];
    extern const char ASTERISK[];
    extern const char DASH[];
#endif

//Sparge step strings
extern const char SPARGE[];
extern const char ALLOFF[];
extern const char FILLBOTH[];
extern const char FLYSPARGE[];

//Boil step strings
extern const char BOIL[];
extern const char BOIL_AUTO[];
extern const char BOIL_MAN[];

//Chill step state strings
extern const char CHILL[];
extern const char BEER[];
extern const char H20[];
extern const char IN[];
extern const char OUT[];
extern const char CHILLNORM[];

//AUX Screen Strings
extern const char AUX_TEMPS[];
extern const char AUX[];
extern const char ONE[];
extern const char TWO[];
#ifndef DIRECT_FIRED_RIMS
    extern const char THREE[];
#endif

//Main Menu Strings
extern const char EDIT_PROGRAM[];
extern const char START_PROGRAM[];
extern const char VALVE_ON[]; //Also used in Boil Menu
extern const char VALVE_OFF[]; // Also used in Boil Menu
extern const char RESET_ALL[];
extern const char SYSTEM_SETUP[];
extern const char DRAIN_ERR1[];
extern const char DRAIN_ERR2[];
extern const char DRAIN_ERR3[];

//Fill Menu Strings
extern const char AUTO_FILL[];
extern const char HLT_TARGET[]; // Also used in Sparge Menu
extern const char MASH_TARGET[];
extern const char HLT_TARGET_VOL[]; // Also used in Sparge Menu
extern const char MASH_TARGET_VOL[];

//Mash Menu Strings
extern const char HLT_SETPOINT[]; // Also used in Edit Prog menu
extern const char MASH_SETPOINT[];
extern const char MASH_TIMER[];

//Sparge Menu Strings
extern const char AUTO_IN[];
extern const char AUTO_OUT[];
extern const char AUTO_FLY[];
extern const char KETTLE_TARGET[];
extern const char KETTLE_TARGET_VOL[];

//Boil Menu Strings
extern const char BOIL_CTRL[];
extern const char BOIL_TEMP[];
extern const char BOIL_POWER[];
extern const char BOIL_TIMER[];

//Program Menu Strings
extern const char PROG_NAME[];
extern const char EDIT_PROG[];
extern const char GRAIN_TEMP[];
extern const char START[];
extern const char DELAY_START[];
extern const char START_FAILED[];
extern const char START_ERR1[];
extern const char START_ERR2[];
extern const char START_ERR3[];
extern const char BATCH_VOL[];
extern const char GRAIN_WT[];
extern const char BOIL_LEN[];
extern const char MINUTES_IND[];
extern const char MASH_RATIO[];
extern const char RATIO_TO_1[];
extern const char NO_SPARGE[];
extern const char HLT_TEMP[];
extern const char SPARGE_TEMP[];
extern const char PITCH_TEMP[];
extern const char MASH_SCHED[];
extern const char HEAT_STRIKE_IN[];
extern const char MASH[];
extern const char UNKWN[];
extern const char PROG_CALCS[];
extern const char BATCH_VOLUME[];
extern const char GRAIN_WEIGHT[];
extern const char RATIO_UNITS[];
extern const char STRIKE_TEMP[];
extern const char STRIKE_VOL[];
extern const char SPARGE_VOL[];
extern const char PREBOIL_VOL[];
extern const char GRAIN_VOL[];
extern const char GRAIN_LOSS[];
//Hop Schedule
extern const char AT_BOIL[];
extern const char MIN_SPACE[];
extern const char MIN_2SPACE[];
extern const char MIN_3SPACE[];
extern const char ZERO[];
//HLT Warning
extern const char HLT_CAP_WARN[];
//Mash Warning
extern const char MASH_CAP_WARN[];
//Boil Warning
extern const char BOIL_CAP_WARN[];
//Program Strings
extern const char FAILED_ADV1[];
extern const char FAILED_ADV2[];

//Vessel Strings
extern const char TITLE_VS_HLT[];
extern const char TITLE_VS_MASH[];
extern const char TITLE_VS_KETTLE[];

//E-Stop Strings
extern const char ESTOP_TRIGGERED[];
extern const char CLEAR_ALARM[];
extern const char CLEAR_ESTOP[];

//Direct Fired RIMS Strings
#ifdef DIRECT_FIRED_RIMS
    extern const char AT[];
    extern const char SET[];
    extern const char RIMS[];
#endif

#ifdef PID_FLOW_CONTROL
    extern const char TITLE_VS_PUMP[];
#elif defined USESTEAM
    extern const char TITLE_VS_STEAM[];
#endif

//Abort Confirm Dialog
extern const char ABORT_OP1[];
extern const char ABORT_OP2[];
extern const char ABORT_OP3[];
extern const char RESET[];

//Confirm Delete Dialog
extern const char DELETE_ITEM[];
extern const char DELETE[];

//Print Timer
extern const char TIMER_SEP[];

//System Setup Menu
#ifndef UI_NO_SETUP
    extern const char TEMP_SENSORS[];
    extern const char OUTPUTS[];
    extern const char VOLS_CAPS[];
    #ifdef PVOUT
        extern const char VALVE_PROFILES[];
        #ifdef PVOUT_TYPE_MODBUS
            extern const char RS485_OUTPUTS[];
        #endif
    #endif
    #ifdef UI_DISPLAY_SETUP
        extern const char DISPLAY_S[];
    #endif
    #ifdef RGBIO8_ENABLE
        #ifdef RGBIO8_SETUP
            extern const char RGB_SETUP[];
        #endif
    #endif
    #ifdef DIGITAL_INPUTS
        extern const char TRIGGERS[];
    #endif
    extern const char RESET_CONFIG[];

    extern const char* const TITLE_VS[];

    extern const char PIDCYCLE[];
    extern const char PIDGAIN[];
    extern const char HYSTERESIS[];

    #ifdef PID_FLOW_CONTROL
        extern const char PUMPFLOW[];
    #elif defined USESTEAM
        extern const char STEAMPRESS[];
        extern const char STEAMSENSOR[];
        extern const char STEAMZERO[];
    #endif

    extern const char CAPACITY[];
    extern const char DEADSPACE[];
    extern const char CALIBRATION[];

#endif //UI_NO_SETUP

extern const char HLTDESC[];
extern const char MASHDESC[];
extern const char SEC[];

#ifdef USEMETRIC
    extern const char VOLUNIT[];
    extern const char WTUNIT[];
    extern const char TUNIT[];
    #ifdef PID_FLOW_CONTROL
        extern const char PUNIT[];
    #else
        extern const char PUNIT[];
    #endif
#else
    extern const char VOLUNIT[];
    extern const char WTUNIT[];
    extern const char TUNIT[];
    #ifdef PID_FLOW_CONTROL
        extern const char PUNIT[];
    #else
        extern const char PUNIT[];
    #endif
#endif //USEMETRIC

#endif //NOUI

#endif