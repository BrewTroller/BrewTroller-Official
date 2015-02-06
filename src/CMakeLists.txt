# Source files for BrewTroller
set(SOURCE_FILES
    BrewCore.cpp
    BrewTroller.cpp
    Com.cpp
    Com_BTnic.h
    Com_BTPD.cpp
    Com_RGBIO8.cpp
    Com_RGBIO8.h
    Com_S0ASC.cpp
    Config.h
    EEPROM.cpp
    Enum.h
    Events.cpp
    HWProfile.h
    Outputs.cpp
    PVOut.h
    StepLogic.cpp
    Temp.cpp
    Timer.cpp
    UI.cpp
    UI_LCD.h
    Util.cpp
    Volume.cpp)

add_avr_executable(BrewTroller ${SOURCE_FILES})

# Link libraries
target_link_libraries(BrewTroller-${AVR_MCU}.elf arduino-${AVR_MCU})