#ifndef BREWTROLLER_H
#define BREWTROLLER_H

#define BUILDNUM 3

#include <Arduino.h>
#include <pin.h>
#include <PID_Beta6.h>

#include "HardwareProfile.h"
#include "PVOut.h"
#include "UI_LCD.h"
#include "Vessel.h"
#include "FlowController.h"

const extern void(* softReset) (void);

//**********************************************************************************
// Compile Time Logic
//**********************************************************************************

#if defined USEPWM_HLT || defined USEPWM_MASH || defined USEPWM_KETTLE || defined USEPWM_PUMP
#define USEPWM
#endif

#ifdef USEMETRIC
  #define SETPOINT_MULT 50
  #define SETPOINT_DIV 2
#else
#define SETPOINT_MULT 100
#define SETPOINT_DIV 1
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

struct ProgramThread {
    byte activeStep;
    byte recipe;
};

//**********************************************************************************
// Globals
//**********************************************************************************
//Heat Output Pin Array
extern pin heatPin[4], alarmPin;

#ifdef DIGITAL_INPUTS
extern pin digInPin[DIGIN_COUNT];
#endif

extern pin * TriggerPin[5];
extern boolean estop;

#ifdef HEARTBEAT
extern pin hbPin;
#endif

//Volume Sensor Pin Array
extern byte vSensor[3];

//8-byte Temperature Sensor Address x9 Sensors
extern byte tSensor[9][8];
extern int temp[9];

#ifdef HLT_MIN_REFILL
extern unsigned long SpargeVol;
#endif

//Create the appropriate 'LCD' object for the hardware configuration (4-Bit GPIO, I2C)
#if defined UI_LCD_4BIT
  #include <LiquidCrystalFP.h>

  #ifndef UI_DISPLAY_SETUP
    extern LCD4Bit LCD;
  #else
    extern LCD4Bit LCD;
  #endif

#elif defined UI_LCD_I2C
extern LCDI2C LCD;
#endif



//Valve Variables
extern unsigned long vlvConfig[NUM_VLVCFGS], actProfiles;
extern boolean autoValve[NUM_AV];

//Create the appropriate 'Valves' object for the hardware configuration (GPIO, MUX, MODBUS)
#if defined PVOUT_TYPE_GPIO
#define PVOUT
extern PVOutGPIO Valves;

#elif defined PVOUT_TYPE_MUX
  #define PVOUT
  extern PVOutMUX Valves;
#endif

#ifdef PVOUT_TYPE_MODBUS
extern PVOutMODBUS *ValvesMB[PVOUT_MODBUS_MAXBOARDS];
#endif

//Shared buffers
extern char buf[20];

//Output Globals
extern Vessel* vessels[3];
extern FlowController* flowController[2]; //Used to move between tuns
extern FlowController* fillController; //Used to fill from outside

#ifdef USEPWM
extern unsigned int cycleStart[6];
#else
extern unsigned long cycleStart[6];
#endif

//Steam Pressure in thousandths
extern byte boilPwr;

#ifdef RIMS_MLT_SETPOINT_DELAY
  extern byte steptoset;
  extern byte RIMStimeExpired;
  extern unsigned long starttime;
  extern unsigned long timetoset;
#endif

//Timer Globals
extern unsigned long timerValue[2], lastTime[2];
extern boolean timerStatus[2], alarmStatus;

//Log Globals
extern boolean logData;

//Brew Step Logic Globals

extern ControlState boilControlState;

//Bit 1 = Boil; Bit 2-11 (See Below); Bit 12 = End of Boil; Bit 13-15 (Open); Bit 16 = Preboil (If Compile Option Enabled)
extern unsigned int hoptimes[11];
extern byte pitchTemp;

//Log Strings
extern const char LOGCMD[];
extern const char LOGDEBUG[];
extern const char LOGSYS[];
extern const char LOGCFG[];
extern const char LOGDATA[];

//PWM by timer globals
#ifdef USEPWM
extern unsigned int timer1_overflow_count = 0;
#endif

// set what the PID cycle time should be based on how fast the temp sensors will respond
#if TS_ONEWIRE_RES == 12
#define PID_CYCLE_TIME 750
#elif TS_ONEWIRE_RES == 11
#define PID_CYCLE_TIME 375
#elif TS_ONEWIRE_RES == 10
#define PID_CYCLE_TIME 188
#elif TS_ONEWIRE_RES == 9
#define PID_CYCLE_TIME 94
#else
// should not be this value, fail the compile
#ERROR
#endif

constexpr bool MASH_AVG_AUX1 =
#ifdef MASH_AVG_A1
true;
#else
false;
#endif

constexpr bool MASH_AVG_AUX2 =
#ifdef MASH_AVG_A2
true;
#else
false;
#endif

constexpr bool MASH_AVG_AUX3 =
#ifdef MASH_AVG_A3
true;
#else
false;
#endif


#endif