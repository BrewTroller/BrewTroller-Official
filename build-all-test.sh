#!/bin/bash

EXIT_CODE=0

# Color codes
RED='\033[0;31m'
BLUE='\033[0;36m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Setup working directory
BUILD_DIR="build"

if [ -d $BUILD_DIR ]; then
    rm -rf $BUILD_DIR
fi

mkdir -p $BUILD_DIR 
cd $BUILD_DIR

# Between build clean up function
function build_cleanup
{
    echo -e "Cleaning build area"
    rm -rf *
}

function error_building_handler
{
    echo -e "${RED}Error building for $1 ${NC}"
    EXIT_CODE=1
}

# generic board builder function
function build_for_board 
{
    echo -e "${BLUE}Test building for $1 ${NC}"
    cmake -Dboard=$1 ../
    CURR_CMAKE_STATUS=$?

    make
    CURR_BUILD_STATUS=$?

    if [ $CURR_CMAKE_STATUS -ne 0 -o $CURR_BUILD_STATUS -ne 0 ]; then
        error_building_handler $1
    else
        echo -e "${GREEN}Build for $1 OK ${NC}"
    fi
}

board_list=("BT_3X"\
            "BT_4P"\
            "OT_BX1_HERMS"\        
            "OT_BX1_HERMSI2CLCD"\  
            "OT_BX1_RIMSDF"\       
            "OT_BX1_RIMSDFI2CLCD"\ 
            "OT_BX1_SINGLEVESSEL"\ 
            "OT_BX1_STEAMPWM"\     
            "OT_BX1_STEAMPWMI2CLCD"\
            "OT_DX1_HERMS"\        
            "OT_DX1_RIMSDF"\       
            "OT_DX1_SINGLEVESSEL"\ 
            "OT_DX1_STEAMPWM"\     
            "OT_EX1_HERMS"\        
            "OT_EX1_RIMSDF"\       
            "OT_EX1_SINGLEVESSEL"\ 
            "OT_EX1_STEAMPWM"\
            "BT_PHOENIX_HERMS"\
            "BT_PHOENIX_RIMS"\
            "BT_PHOENIX_SINGLE_VESSEL"\
            "BT_PHOENIX_STEAM_PWM")     

for board in "${board_list[@]}"
do
    build_for_board $board
done

exit $EXIT_CODE
