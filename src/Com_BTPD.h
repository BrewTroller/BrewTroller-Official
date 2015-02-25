#ifndef COM_BTPD_H
#define COM_BTPD_H


#ifdef BTPD_SUPPORT

#define BTPD_HLT_TEMP 0x20 // BTPD_HLT_TEMP: Displays HLT temp and setpoint on specified channel
#define BTPD_MASH_TEMP 0x22 // BTPD_MASH_TEMP: Displays Mash temp and setpoint on specified channel
#define BTPD_KETTLE_TEMP 0x23    // BTPD_KETTLE_TEMP: Displays Kettle temp and setpoint on specified channel
//#define BTPD_KETTLE_TEMPTIME 0x23    // BTPD_KETTLE_TEMP: Displays Kettle temp and boil timer on specified channel
//#define BTPD_H2O_TEMPS 0x24 // BTPD_H2O_TEMPS: Displays H2O In and H2O Out temps on specified channels
#define BTPD_FERM_TEMP 0x24 // BTPD_FERM_TEMP: Displays Beer Out temp and Pitch temp on specified channel
#define BTPD_TIMERS 0x25 // BTPD_FERM_TEMP: Displays Beer Out temp and Pitch temp on specified channel
//#define BTPD_HLT_VOL 0x26 // BTPD_HLT_VOL: Displays current and target HLT volume
//#define BTPD_MASH_VOL 0x27 // BTPD_MASH_VOL: Displays current and target Mash volume
//#define BTPD_KETTLE_VOL 0x28 // BTPD_KETTLE_VOL: Displays current and target Kettle volume
//#define BTPD_STEAM_PRESS 0x29 // BTPD_STEAM_PRESS: Displays current and target Steam pressure
#define BTPD_AUX1_TEMP 0x2a

void updateBTPD();

void sendVsTemp(byte chan, byte sensor, byte vessel);

void sendVsVol(byte chan, byte vessel);

void sendStringBTPD(byte chan, const char *string);

void sendFloat1BTPD(byte chan, float line);

void sendFloat2BTPD(byte chan, float line);

void sendFloatsBTPD(byte chan, float line1, float line2);

#ifdef BTPD_TIMERS
/*
  Converts brewtroller timers to byte values, and chooses appropriate set to send to BTPD
  sends HH:MM for times greater than 1 hour, and MM:SS for times under.
  If timeris paused, send high byte value to post "--:--"
  Timer1 fills the top row
  Timer2 fills the bottom row
*/
void sendVsTime(byte chan, byte timer1, byte timer2);

/*
  BTPD requires ASCII to enable colons;
  colon in first four characters enables top colon,
  colon after enables bottom colon. sent in middle for clarity.
  Format for BTPD is:
  AA:BB
  CC:DD
  values are checked for range, and if leading zero is required. (to maintian two digits)
  AA / CC value above 99 posts --:--
  BB / DD value above 59 posts --:--
*/
void SendTimeBTPD(byte chan, byte AA, byte BB, byte CC, byte DD);

#endif //BTPD_TIMERS
#endif //BTPD_SUPPORT


#endif