//
// Created by Eric Yanush on 2017-11-27.
//

#ifndef BREWTROLLER_RGBIO_MAP_HPP
#define BREWTROLLER_RGBIO_MAP_HPP

#include "Config.h"
#include "@COM_RGBIO_HEADER@"

const RGBIO8_output_assignment rgbioOutputAssignments[RGBIO8_NUM_BOARDS][8] = @RGBIO8_OUTPUT_MAP@;
const RGBIO8_input_assignment rgbioInputAssignments[RGBIO8_NUM_BOARDS][8] = @RGBIO8_INPUT_MAP@;

#endif //BREWTROLLER_RGBIO_MAP_HPP
