#!/bin/bash

#Boards
BOARDS=("BT_3X" "BT_4P" "OT_BX1_HERMS" "OT_BX1_HERMSI2CLCD" "OT_BX1_RIMSDF"    \
        "OT_BX1_RIMSDFI2CLCD" "OT_BX1_SINGLEVESSEL" "OT_BX1_STEAMPWM"          \
        "OT_BX1_STEAMPWMI2CLCD" "OT_DX1_HERMS" "OT_DX1_RIMSDF"                 \
        "OT_DX1_SINGLEVESSEL" "OT_DX1_STEAMPWM" "OT_EX1_HERMS" "OT_EX1_RIMSDF" \
        "OT_EX1_SINGLEVESSEL" "OT_EX1_STEAMPWM")

BUILDS_SUCCESS=0

# Setup working directory
BUILD_DIR="build-all-test"
mkdir -p build-all-test

cd $BUILD_DIR

#Do a build
for b in "${BOARDS[@]}"; do
    echo "Test building for $b"
    cmake_err=$(cmake -Dboard=$b ../ 2>&1 | grep error)
    if [[ $cmake_err ]]; then
        echo -e "\033[0;31mErrors setting up cmake: \033[0m"
        echo $cmake_err
        BUILDS_SUCCESS=1
    else
        make_err=$(make  2>&1 | grep error)
        if [[ $make_err ]]; then
            echo -e "\033[0;31mBuild errors: \033[0m"
            echo $make_err
            BUILDS_SUCCESS=1
        else
            echo -e "\033[0;32mSucessfully built for: $b\033[0m"
        fi
    fi
    rm -rf *
done

cd ../
rm -r $BUILD_DIR

exit BUILDS_SUCCESS