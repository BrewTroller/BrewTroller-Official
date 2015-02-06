#  BrewTroller 2.x CMake File
#  Using contributions from cmake-avr from Matthias Kleemann
#  github.com/mkleemann/cmake-avr
#

cmake_minimum_required(VERSION 2.8)

project(BrewTroller)


# Set Processor Vars
set(AVR_MCU atmega1284p)
set(MCU_SPEED "16000000UL")

# Set Compiler flags
add_definitions("-DF_CPU=${MCU_SPEED}")
add_definitions("-fpack-struct")
add_definitions("-fshort-enums")
add_definitions("-Wall")
add_definitions("-funsigned-char")
add_definitions("-funsigned-bitfields")
add_definitions("-ffunction-sections")
add_definitions("-fdata-sections")
add_definitions("-std=c++11")
add_definitions("-Os")

include(avr_toolchain.cmake)

include_directories(lib/arduino
                    lib/arduino/avr-libc)

# Source files for BrewTroller
set(SOURCE_FILES
    src/BrewCore.cpp
    src/BrewTroller.cpp
    src/Com.cpp
    src/Com_BTnic.h
    src/Com_BTPD.cpp
    src/Com_RGBIO8.cpp
    src/Com_RGBIO8.h
    src/Com_S0ASC.cpp
    src/Config.h
    src/EEPROM.cpp
    src/Enum.h
    src/Events.cpp
    src/HWProfile.h
    src/Outputs.cpp
    src/PVOut.h
    src/StepLogic.cpp
    src/Temp.cpp
    src/Timer.cpp
    src/UI.cpp
    src/UI_LCD.h
    src/Util.cpp
    src/Volume.cpp)

add_avr_executable(BrewTroller ${SOURCE_FILES})

# Source files for Arduino Libraries
set(ARDUINO_SOURCE
        lib/arduino/avr-libc/malloc.c
        lib/arduino/avr-libc/realloc.c
        lib/arduino/CDC.cpp
        lib/arduino/HardwareSerial.cpp
        lib/arduino/HID.cpp
        lib/arduino/IPAddress.cpp
        lib/arduino/main.cpp
        lib/arduino/new.cpp
        lib/arduino/Print.cpp
        lib/arduino/Stream.cpp
        lib/arduino/Tone.cpp
        lib/arduino/USBCore.cpp
        lib/arduino/WInterrupts.c
        lib/arduino/wiring.c
        lib/arduino/wiring_analog.c
        lib/arduino/wiring_digital.c
        lib/arduino/wiring_pulse.c
        lib/arduino/wiring_shift.c
        lib/arduino/WMath.cpp
        lib/arduino/WString.cpp)

set(ARDUINO_HEADERS
        lib/arduino/avr-libc/sectionname.h
        lib/arduino/avr-libc/stdlib_private.h
        lib/arduino/Arduino.h
        lib/arduino/binary.h
        lib/arduino/Client.h
        lib/arduino/WString.h
        lib/arduino/wiring_private.h
        lib/arduino/USBCore.h
        lib/arduino/USBDesc.h
        lib/arduino/WCharacter.h
        lib/arduino/Udp.h
        lib/arduino/USBAPI.h
        lib/arduino/Stream.h
        lib/arduino/Print.h
        lib/arduino/Printable.h
        lib/arduino/Server.h
        lib/arduino/new.h
        lib/arduino/Platform.h
        lib/arduino/IPAddress.h
        lib/arduino/HardwareSerial.h
        lib/arduino/pins_arduino.h)

add_avr_library(arduino ${ARDUINO_SOURCE} ${ARDUINO_HEADERS})

# Link libraries
target_link_libraries(BrewTroller-${AVR_MCU}.elf arduino-${AVR_MCU})