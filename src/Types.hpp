/*
 Copyright (C) 2015 Eric Yanush
 
 This file is part of BrewTroller.
 
 BrewTroller is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 BrewTroller is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with BrewTroller.  If not, see <http://www.gnu.org/licenses/>.
 
 Documentation, Forums and more information available at http://www.brewtroller.com
 */

#ifndef Types_h
#define Types_h

#include <stdint.h>

struct ProgramThread {
    uint8_t activeStep;
    uint8_t recipe;
};


#endif /* Types_h */
