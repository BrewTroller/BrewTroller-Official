#ifndef BREWTROLLER_H
#define BREWTROLLER_H


#include <Arduino.h>
#include <PID_Beta6.h>
#include <pin.h>
#include "Config.h"
#include "HWProfile.h"
#include "UI_LCD.h"
#include "PVOut.h"

#define BUILD 3

#warning Fix ME! move global scope variables into class


//**********************************************************************************
// Globals
//**********************************************************************************

//Volume Sensor Pin Array
#ifdef HLT_AS_KETTLE
  byte vSensor[3] = { HLTVOL_APIN, MASHVOL_APIN, HLTVOL_APIN};
#elif defined KETTLE_AS_MASH
  byte vSensor[3] = { HLTVOL_APIN, KETTLEVOL_APIN, KETTLEVOL_APIN};
#elif defined SINGLE_VESSEL_SUPPORT
  byte vSensor[3] = { HLTVOL_APIN, HLTVOL_APIN, HLTVOL_APIN};
#else
byte vSensor[3] = { HLTVOL_APIN, MASHVOL_APIN, KETTLEVOL_APIN};
#endif

#ifdef SPARGE_IN_PUMP_CONTROL
unsigned long prevSpargeVol[2] = {0,0};
#endif

#ifdef HLT_MIN_REFILL
unsigned long SpargeVol = 0;
#endif

#ifdef FLOWRATE_CALCS
//Flowrate in thousandths of gal/l per minute
long flowRate[3] = {0,0,0};
#endif

//Create the appropriate 'Valves' object for the hardware configuration (GPIO, MUX, MODBUS)
#if defined PVOUT_TYPE_GPIO
#define PVOUT
PVOutGPIO Valves(PVOUT_COUNT);

#elif defined PVOUT_TYPE_MUX
  #define PVOUT
  PVOutMUX Valves(
    MUX_LATCH_PIN,
    MUX_DATA_PIN,
    MUX_CLOCK_PIN,
    MUX_ENABLE_PIN,
    MUX_ENABLE_LOGIC
  );
#endif

#ifdef PVOUT_TYPE_MODBUS
PVOutMODBUS *ValvesMB[PVOUT_MODBUS_MAXBOARDS];
#endif

//Brew Step Logic Globals
boolean preheated[4];
ControlState boilControlState = CONTROLSTATE_OFF;

//Bit 1 = Boil; Bit 2-11 (See Below); Bit 12 = End of Boil; Bit 13-15 (Open); Bit 16 = Preboil (If Compile Option Enabled)
unsigned int hoptimes[11] = { 105, 90, 75, 60, 45, 30, 20, 15, 10, 5, 0 };
byte pitchTemp;

//PWM by timer globals
#ifdef PWM_BY_TIMER
unsigned int timer1_overflow_count = 0;
unsigned int PIDOutputCountEquivalent[4][2] = {{0,0},{0,0},{0,0},{0,0}};
#endif

//Shared buffers
char buf[20];

//Log Globals
boolean logData = LOG_INITSTATUS;

const char BT[] PROGMEM = "BrewTroller";
const char BTVER[] PROGMEM = "2.7";

//Log Strings
const char LOGCMD[] PROGMEM = "CMD";
const char LOGDEBUG[] PROGMEM = "DEBUG";
const char LOGSYS[] PROGMEM = "SYS";
const char LOGCFG[] PROGMEM = "CFG";
const char LOGDATA[] PROGMEM = "DATA";

//Output Globals
double PIDInput[4], PIDOutput[4], setpoint[4];
//Output Globals
#ifdef PID_FEED_FORWARD
double FFBias;
#endif
#ifdef PWM_BY_TIMER
unsigned int cycleStart[4] = {0,0,0,0};
#else
unsigned long cycleStart[4] = {0,0,0,0};
#endif

pin * TriggerPin[5] = { NULL, NULL, NULL, NULL, NULL };
boolean estop = 0;

//Valve Variables
unsigned long vlvConfig[NUM_VLVCFGS], actProfiles;
boolean autoValve[NUM_AV];

//8-byte Temperature Sensor Address x9 Sensors
byte tSensor[9][8];
int temp[9];

#ifdef DIGITAL_INPUTS
pin digInPin[DIGIN_COUNT];
#endif

//PID Params
byte PIDCycle[4], hysteresis[4];
boolean heatStatus[4], PIDEnabled[4];
unsigned int steamPSens, steamZero;

byte pidLimits[4] = { PIDLIMIT_HLT, PIDLIMIT_MASH, PIDLIMIT_KETTLE, PIDLIMIT_STEAM };

PID pid[4] = {
        PID(&PIDInput[VS_HLT], &PIDOutput[VS_HLT], &setpoint[VS_HLT], 3, 4, 1),

#ifdef PID_FEED_FORWARD
            PID(&PIDInput[VS_MASH], &PIDOutput[VS_MASH], &setpoint[VS_MASH], &FFBias, 3, 4, 1),
        #else
        PID(&PIDInput[VS_MASH], &PIDOutput[VS_MASH], &setpoint[VS_MASH], 3, 4, 1),
#endif

        PID(&PIDInput[VS_KETTLE], &PIDOutput[VS_KETTLE], &setpoint[VS_KETTLE], 3, 4, 1),

#ifdef PID_FLOW_CONTROL
            PID(&PIDInput[VS_PUMP], &PIDOutput[VS_PUMP], &setpoint[VS_PUMP], 3, 4, 1)
        #else
        PID(&PIDInput[VS_STEAM], &PIDOutput[VS_STEAM], &setpoint[VS_STEAM], 3, 4, 1)
#endif
};
#if defined PID_FLOW_CONTROL && defined PID_CONTROL_MANUAL
  unsigned long nextcompute;
  byte additioncount[2];
#endif

#ifdef RIMS_MLT_SETPOINT_DELAY
  byte steptoset = 0;
  byte RIMStimeExpired = 0;
  unsigned long starttime = 0;
  unsigned long timetoset = 0;
#endif

//Steam Pressure in thousandths
unsigned long steamPressure;
byte boilPwr;

//Volume in (thousandths of gal/l)
unsigned long tgtVol[3], volAvg[3], calibVols[3][10];
unsigned int calibVals[3][10];

//Timer Globals
unsigned long timerValue[2], lastTime[2];
boolean timerStatus[2], alarmStatus;

//Heat Output Pin Array
pin heatPin[4], alarmPin;

struct ProgramThread {
    byte activeStep;
    byte recipe;
};

#ifdef HEARTBEAT
pin hbPin;
#endif

//Create the appropriate 'LCD' object for the hardware configuration (4-Bit GPIO, I2C)
#if defined UI_LCD_4BIT
  #include <LiquidCrystalFP.h>

  #ifndef UI_DISPLAY_SETUP
    LCD4Bit LCD(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_DATA4_PIN, LCD_DATA5_PIN, LCD_DATA6_PIN, LCD_DATA7_PIN);
  #else
    LCD4Bit LCD(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_DATA4_PIN, LCD_DATA5_PIN, LCD_DATA6_PIN, LCD_DATA7_PIN, LCD_BRIGHT_PIN, LCD_CONTRAST_PIN);
  #endif

#elif defined UI_LCD_I2C
LCDI2C LCD(UI_LCD_I2CADDR);
#endif

#ifdef USEMETRIC
    #define SETPOINT_MULT 50
    #define SETPOINT_DIV 2
#else
#define SETPOINT_MULT 100
#define SETPOINT_DIV 1
#endif

#endif