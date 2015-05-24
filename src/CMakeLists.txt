# Source files for BrewTroller
set(SOURCE_FILES
        BrewCore.cpp
        BrewCore.h
        BrewTroller.cpp
        BrewTroller.h
        Com.cpp
        Com.h
        Com_BTnic.h
        Com_BTPD.cpp
        Com_BTPD.h
        Com_RGBIO8.cpp
        Com_RGBIO8.h
        Com_S0ASC.cpp
        Config.h
        EEPROM.cpp
        EEPROM.h
        Enum.h
        Events.cpp
        Events.h
        HardwareProfile.h
        Outputs.cpp
        Outputs.h
        PVOut.h
        PVOut.cpp
        StepLogic.cpp
        StepLogic.h
        Temp.cpp
        Temp.h
        Timer.cpp
        Timer.h
        UI.cpp
        UI.h
        UI_LCD.h
        UIStrings.h
        UIStrings.cpp
        Util.cpp
        Util.h
        Volume.cpp
        Volume.h)

add_avr_executable(BrewTroller-${board} ${SOURCE_FILES})

# Link libraries
target_link_libraries(BrewTroller-${board}.elf BT-libs)