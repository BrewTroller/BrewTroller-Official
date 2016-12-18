#  BrewTroller 2.x CMake File
#  Using contributions from cmake-avr from Matthias Kleemann
#  github.com/mkleemann/cmake-avr
#

cmake_minimum_required(VERSION 2.8)

# Set Processor Vars
set(AVR_MCU atmega1284p CACHE STRING "Hardware targets, available options:
          atmega1284p
          atmega644p")
set(MCU_SPEED "16000000UL")

# Toolchain file needs to be included before any compiler flags on some systems
include(avr_toolchain.cmake)

# Older versions of CMake (< 3) require the toolchain file to be included before the project definition
project(BrewTroller)

# Set Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcall-prologues -fno-inline-small-functions -fno-split-wide-types -fshort-enums -Wall -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -Os")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcall-prologues -fno-inline-small-functions -fno-split-wide-types -fshort-enums -Wall -Wno-switch -Wno-return-type -Wno-maybe-uninitialized -Wno-sequence-point -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -std=c++11 -fno-exceptions -Os")

add_definitions("-DF_CPU=${MCU_SPEED}")

include_directories(lib/arduino
                    lib/arduino/avr-libc
                    lib/EEPROM
                    lib/encoder
                    lib/fastpin
                    lib/menu
                    lib/modbusmaster
                    lib/PID_Beta6
                    lib/Wire
                    lib/liquidcrystalfp
                    lib/ds2482
                    lib/OneWire)

add_subdirectory(lib)
add_subdirectory(src)

if(EXISTS "rgbio.json")
    message(STATUS "Auto-generating RGBIO config")
    execute_process(COMMAND python json2mako.py -j rgbio.json -t templates/rgbio.template -o src/Com_RGBIO8_Config.h
                    ERROR_VARIABLE rgb_stderr
                    RESULT_VARIABLE rgb_result_var
                    ERROR_STRIP_TRAILING_WHITESPACE)
    if (NOT "${rgb_result_var}" STREQUAL "0")
        message(FATAL_ERROR "${rgb_stderr}")
    else()
        add_definitions("-DGENERATED_RGBIO_CONFIG")
    endif()
endif()
