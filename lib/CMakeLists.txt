# Build Arduino Libraries
set(ARDUINO_SOURCE
        arduino/avr-libc/malloc.c
        arduino/avr-libc/realloc.c
        arduino/CDC.cpp
        arduino/HardwareSerial.cpp
        arduino/HID.cpp
        arduino/IPAddress.cpp
        arduino/main.cpp
        arduino/new.cpp
        arduino/Print.cpp
        arduino/Stream.cpp
        arduino/Tone.cpp
        arduino/USBCore.cpp
        arduino/WInterrupts.c
        arduino/wiring.c
        arduino/wiring_analog.c
        arduino/wiring_digital.c
        arduino/wiring_pulse.c
        arduino/wiring_shift.c
        arduino/WMath.cpp
        arduino/WString.cpp)

set(ARDUINO_HEADERS
        arduino/avr-libc/sectionname.h
        arduino/avr-libc/stdlib_private.h
        arduino/Arduino.h
        arduino/binary.h
        arduino/Client.h
        arduino/WString.h
        arduino/wiring_private.h
        arduino/USBCore.h
        arduino/USBDesc.h
        arduino/WCharacter.h
        arduino/Udp.h
        arduino/USBAPI.h
        arduino/Stream.h
        arduino/Print.h
        arduino/Printable.h
        arduino/Server.h
        arduino/new.h
        arduino/Platform.h
        arduino/IPAddress.h
        arduino/HardwareSerial.h
        arduino/pins_arduino.h)

add_avr_library(arduino ${ARDUINO_SOURCE} ${ARDUINO_HEADERS})

# Build EEPROM Library
set(EEPROM_SOURCES
    EEPROM/EEPROM.h
    EEPROM/EEPROM.cpp)

add_avr_library(EEPROM ${EEPROM_SOURCES})

# Build encoder Library
set(ENCODER_SOURCES
    encoder/encoder.cpp
    encoder/encoder.h)

add_avr_library(encoder ${ENCODER_SOURCES})

# Build fastpin Library
set(FASTPIN_SOURCES
    fastpin/pin.h
    fastpin/pin.cpp)

add_avr_library(fastpin ${FASTPIN_SOURCES})

# Build menu Library
set(MENU_SOURCES
    menu/menu.h
    menu/menu.cpp)

add_avr_library(menu ${MENU_SOURCES})

# Build modbusmaster Library
set(MODBUS_SOURCES
    modbusmaster/ModbusMaster.h
    modbusmaster/ModbusMaster.cpp)

add_avr_library(modbusmaster ${MODBUS_SOURCES})

# Build PID Library
set(PID_SOURCES
    PID_Beta6/PID_Beta6.h
    PID_Beta6/PID_Beta6.cpp)

add_avr_library(pid ${PID_SOURCES})

# Build 2Wire Library
set(TWOWIRE_SOURCES
    Wire/Wire.h
    Wire/Wire.cpp
    Wire/utility/twi.h
    Wire/utility/twi.c)

add_avr_library(twowire ${TWOWIRE_SOURCES})

# Build liquidcrystalfp Library
set(liquidcrystalfp_sources
    liquidcrystalfp/LiquidCrystalFP.h
    liquidcrystalfp/LiquidCrystalFP.cpp)

add_avr_library(liquidcrystalfp ${liquidcrystalfp_sources})