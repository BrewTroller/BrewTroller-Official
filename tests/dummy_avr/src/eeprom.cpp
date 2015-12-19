//
//  eeprom.cpp
//  BrewTroller
//
//  Created by Eric Yanush on 2015-12-18.
//  Copyright © 2015 EricYanush. All rights reserved.
//

#include <stdint.h>
#include <string.h>
#include "../avr/eeprom.h"

//Define a pointer and size for the data store that we will use to fake the eeprom interface
static uint8_t *dataPointer = 0;
static uint16_t dataSize = 0;

void initFakeEEPROM(uint8_t *dataStore, uint16_t dataStoreSize) {
    dataPointer = dataStore;
    dataSize = dataStoreSize;
}

uint8_t eeprom_read_byte(const uint8_t *address) {
    return *address;
}

void eeprom_read_block(void* dest, const void* src, size_t size) {
    memcpy(dest, src, size);
}

void eeprom_update_byte(uint8_t* dest, uint8_t newVal) {
    memcpy(dest, &newVal, 1);
}

void eeprom_update_block(const void* src, void* dst, size_t size) {
    memcpy(dst, src, size);
}


