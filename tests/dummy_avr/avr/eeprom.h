//
//  eeprom.h
//  BrewTroller Test Suite
//
//  Created by Eric Yanush on 2015-12-18.
//  Copyright © 2015 EricYanush. All rights reserved.
//

#ifndef dummy_eeprom_h
#define dummy_eeprom_h

#include <stdint.h>
#include <stddef.h>

//Define the EEMEM attribute to be nothing, so we can compile for non-avr targets
#define EEMEM

/**
  initFakeEEPROM: initializes the fake eeprom for testing eeprom interactions
  - parameter dataStore: a pointer to the object used to back the fake eeprom, interpreted as an array of bytes
  - parameter dataStoreSize: the integer size of the backing dataStore, in bytes.
  - returns: void
 */
void initFakeEEPROM(uint8_t *dataStore, uint16_t dataStoreSize);

/**
 eeprom_read_byte: reads a single byte from the fake eeprom
 - parameter address: the address of the byte to read
 - returns: the byte read from eeprom
 */
uint8_t eeprom_read_byte(const uint8_t *address);

/**
 eeprom_read_block: reads a block from the fake eeprom of abitrary size
 - parameter dest: pointer to memory where data should be read into
 - parameter src: pointer to the source of the memory where data should be read from
 - parameter size: the size of the data to be read
 */
void eeprom_read_block(void* dest, const void* src, size_t size);

/**
 eeprom_update_byte: updates a byte in the fake eeprom
 - parameter dest: pointer to memory where byte should be updated
 - parameter newVal: the new value to update the destination with
 - Note: This always updates the destination, unlike the AVR implementation which only does so on changes, to reduce unnecesary write cycles.
 */
void eeprom_update_byte(uint8_t* dest, uint8_t newValue);


/**
 eeprom_update_block: updates a block in the fake eeprom of arbitrary size
 - parameter src: pointer to the source of the data to use for updating
 - parameter dst: pointer to the destination in fake eeprom to update
 - parameter size: size of the data to update
 - Note: This alwasy updates the destination, unlike the AVR implementaion which only does so on changes, to reduce unnecessary write cycles.
 */
void eeprom_update_block(const void* src, void* dst, size_t size);
#endif /* dummy_eeprom_h */
