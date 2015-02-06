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
#add_definitions("-Wall")
add_definitions("-funsigned-char")
add_definitions("-funsigned-bitfields")
add_definitions("-ffunction-sections")
add_definitions("-fdata-sections")
add_definitions("-std=c++11")
add_definitions("-Os")

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
                    lib/liquidcrystalfp)

add_subdirectory(src)
add_subdirectory(lib)
