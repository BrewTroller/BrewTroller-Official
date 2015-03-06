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
#ifndef NOUI

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
extern const char AUTO_S[]; //This is named with _S postfix to avoid naming collision
                            // In the future all of these strings should be namespaced to prevent collisions

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

//Sparge step strings
extern const char SPARGE[];
extern const char ALLOFF[];
extern const char FILLBOTH[];
extern const char FLYSPARGE[];

//Boil step strings
extern const char BOIL[];

//Chill step state strings
extern const char CHILL[];
extern const char BEER[];
extern const char H20[];
extern const char IN[];
extern const char OUT[];
extern const char CHILLNORM[];

//Vessel Strings
extern const char TITLE_VS_HLT[];
extern const char TITLE_VS_MASH[];
extern const char TITLE_VS_KETTLE[];

#ifdef PID_FLOW_CONTROL
    extern const char TITLE_VS_PUMP[];
#elif defined USESTEAM
    extern const char TITLE_VS_STEAM[];
#endif

#ifndef UI_NO_SETUP

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