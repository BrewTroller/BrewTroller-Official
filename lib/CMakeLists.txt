# Build Arduino Libraries
set(ARDUINO_SOURCE
        arduino/avr-libc/malloc.c
        arduino/avr-libc/realloc.c
        arduino/CDC.cpp
        arduino/HardwareSerial.cpp
        arduino/HID.cpp
        arduino/IPAddress.cpp
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

# Build EEPROM Library
set(EEPROM_SOURCES
    EEPROM/EEPROM.h
    EEPROM/EEPROM.cpp)

# Build 2Wire Library
set(TWOWIRE_SOURCES
    Wire/Wire.h
    Wire/Wire.cpp
    Wire/utility/twi.h
    Wire/utility/twi.c)

# Build encoder Library
set(ENCODER_SOURCES
    encoder/encoder.cpp
    encoder/encoder.h)

# Build fastpin Library
set(FASTPIN_SOURCES
    fastpin/pin.h
    fastpin/pin.cpp)

# Build menu Library
set(MENU_SOURCES
    menu/menu.h
    menu/menu.cpp)

# Build modbusmaster Library
set(MODBUS_SOURCES
    modbusmaster/ModbusMaster.h
    modbusmaster/ModbusMaster.cpp)

# Build PID Library
set(PID_SOURCES
    PID_Beta6/PID_Beta6.h
    PID_Beta6/PID_Beta6.cpp)

# Build liquidcrystalfp Library
set(liquidcrystalfp_sources
    liquidcrystalfp/LiquidCrystalFP.h
    liquidcrystalfp/LiquidCrystalFP.cpp)

# Build DS2482 Library (OneWire-TwoWire transciever)
set(DS2482_SOURCES
    ds2482/DS2482.h
    ds2482/DS2482.cpp)

# Build OneWire Library (OneWire GPIO)
set(ONEWIRE_SOURCES
    OneWire/OneWire.h
    OneWire/OneWire.cpp)

#disable some compiler warnings for libraries
set_source_files_properties(${ARDUINO_SOURCE} ${ARDUINO_HEADERS}
                            ${EEPROM_SOURCES}
                            ${TWOWIRE_SOURCES}
                            ${ENCODER_SOURCES}
                            ${FASTPIN_SOURCES}
                            ${MENU_SOURCES}
                            ${MODBUS_SOURCES}
                            ${PID_SOURCES}
                            ${liquidcrystalfp_sources}
                            ${DS2482_SOURCES}
                            ${ONEWIRE_SOURCES} PROPERTIES COMPILE_FLAGS "-Wno-sign-compare -Wno-unused-variable -Wno-strict-aliasing -Wno-attributes -Wno-parentheses -Wno-return-type -Wno-maybe-uninitialized")

add_avr_library(BT-libs ${ARDUINO_SOURCE} ${ARDUINO_HEADERS}
                        ${EEPROM_SOURCES}
                        ${TWOWIRE_SOURCES}
                        ${ENCODER_SOURCES}
                        ${FASTPIN_SOURCES}
                        ${MENU_SOURCES}
                        ${MODBUS_SOURCES}
                        ${PID_SOURCES}
                        ${liquidcrystalfp_sources}
                        ${DS2482_SOURCES}
                        ${ONEWIRE_SOURCES})