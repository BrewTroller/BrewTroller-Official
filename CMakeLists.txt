#  BrewTroller 2.x CMake File
#  Using contributions from cmake-avr from Matthias Kleemann
#  github.com/mkleemann/cmake-avr
#

cmake_minimum_required(VERSION 2.8)

project(BrewTroller)

# Configurable BrewTroller options
set(board  "OT_DX1_HERMS" CACHE STRING "BrewTroller Hardware Board Target, available options:
           BT_10_21               --> 1.0 - 2.1
           BT_22_24               --> 2.2 - 2.4
           BT_3X                  --> 3.X
           BT_4P                  --> 4.X Pro
           OT_BX1_HERMS           --> BX1 HERMS
           OT_BX1_HERMSI2CLCD     --> BX1 HERMS + I2CLCD
           OT_BX1_RIMSDF          --> BX1 RIMS / Direct Fired
           OT_BX1_RIMSDFI2CLCD    --> BX1 RIMS / Direct Fired + I2CLCD
           OT_BX1_SINGLEVESSEL    --> BX1 Single Vessel
           OT_BX1_STEAMPWM        --> BX1 Steam or PWM Pump
           OT_BX1_STEAMPWMI2CLCD  --> BX1 Steam or PWM Pump + I2CLCD
           OT_DX1_HERMS           --> DX1 HERMS
           OT_DX1_RIMSDF          --> DX1 RIMS / Direct Fired
           OT_DX1_SINGLEVESSEL    --> DX1 Single Vessel
           OT_DX1_STEAMPWM        --> DX1 Steam or PWM Pump
           OT_EX1_HERMS           --> EX1 HERMS
           OT_EX1_RIMSDF          --> EX1 RIMS / Direct Fired
           OT_EX1_SINGLEVESSEL    --> EX1 Single Vessel
           OT_EX1_STEAMPWM        --> EX1 Steam or PWM Pump") # Default to DX1 Herms build

add_definitions("-D${board}")
message(STATUS "Building BrewTroller for   " ${board})

# Set Processor Vars
set(AVR_MCU atmega1284p CACHE STRING "Hardware targets, available options:
          atmega1284p
          atmega644p")
set(MCU_SPEED "16000000UL")

# Set Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcall-prologues -fno-inline-small-functions -fno-split-wide-types -fshort-enums -Wall -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -Os")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcall-prologues -fno-inline-small-functions -fno-split-wide-types -fshort-enums -Wall -Wno-switch -Wno-return-type -Wno-maybe-uninitialized -Wno-sequence-point -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -std=c++11 -Os")

add_definitions("-DF_CPU=${MCU_SPEED}")

include(avr_toolchain.cmake)

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