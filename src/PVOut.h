#ifndef PVOUT_H
  #define PVOUT_H
  #include <pin.h>
  #include "Config.h"
  #include "HardwareProfile.h"
  #include <ModbusMaster.h>
  #include <HardwareSerial.h>
  
  class PVOutGPIO
  {
    private:
    pin* valvePin;
    unsigned long vlvBits;
    byte pinCount;
    
    public:
    PVOutGPIO(byte count);

    ~PVOutGPIO();
  
    void setup(byte pinIndex, byte digitalPin);
  
    void init(void);
    
    void set(unsigned long vlvBits);
    
    unsigned long get();
    
    inline pin* getPin(byte index) {return &valvePin[index]; } //This function should be used with caution! The class could destroy this pin at any time and callers will be left with a dangling pointer.
  };
  
  class PVOutMUX
  {
    private:
    pin muxLatchPin, muxDataPin, muxClockPin, muxEnablePin;
    boolean muxEnableLogic;
    unsigned long vlvBits;
    
    public:
    PVOutMUX(byte latchPin, byte dataPin, byte clockPin, byte enablePin, boolean enableLogic);
    
    void init(void);
    
    void set(unsigned long vlvBits);
    
    unsigned long get();
  };

#ifdef PVOUT_TYPE_MODBUS
  class PVOutMODBUS
  {
    private:
    unsigned long outputsState;
    ModbusMaster slave;
    byte slaveAddr, outputCount, bitOffset;
    unsigned int coilReg;

    public:
    PVOutMODBUS(uint8_t addr, unsigned int coilStart, uint8_t coilCount, uint8_t offset);
    
    void init(void);
    
    void set(unsigned long vlvBits);
    
    unsigned long get();
    byte count();
    unsigned long offset();
    byte detect();
    byte setAddr(byte newAddr);
    byte setIDMode(byte value);
    byte getIDMode();
  };
#endif

#endif //ifndef PVOUT_H
