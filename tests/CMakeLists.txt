cmake_minimum_required(VERSION 2.8)

set(TEST_TARGET brewtrollerTests)

#build the googlemock/test framework
add_subdirectory(googletest)

#List all test sources
set(BT_TEST_SRCS src/main.cpp
                 src/globals.cpp
				 src/EEPROMTests.cpp)
#List all test target sources
set(BT_TEST_TARGET_SRCS ../src/ConfigManager.cpp)

#Include the BrewTroller headers
include_directories(../src)

#Include the test dummy headers
include_directories(system dummy_avr)
#List all dummy implementations
set(AVR_DUMMY_SRCS dummy_avr/src/eeprom.cpp)

#Include any test headers
include_directories(src)

#Include the googletests/mock paths
include_directories(googletest/googletest/include
					googletest/googlemock/include)

add_executable(${TEST_TARGET} ${BT_TEST_SRCS} ${BT_TEST_TARGET_SRCS} ${AVR_DUMMY_SRCS})
target_link_libraries(${TEST_TARGET} gmock gtest)

#Set the language standard
set_property(TARGET ${TEST_TARGET} PROPERTY CXX_STANDARD 11)