cmake_minimum_required(VERSION 2.8)

set(TEST_TARGET brewtrollerTests)

#build the googlemock/test framework
add_subdirectory(googletest)

#List all test sources
set(BT_TEST_SRCS src/main.cpp
				 src/EEPROMTests.cpp)

#Set the language standard
set(CMAKE_CXX_FLAGS "-std=c++14")

#Include the BrewTroller headers
include_directories(../src)

#Include the googletests/mock paths
include_directories(googletest/googletest/include
					googletest/googlemock/include)

add_executable(${TEST_TARGET} ${BT_TEST_SRCS})
target_link_libraries(${TEST_TARGET} gmock gtest)