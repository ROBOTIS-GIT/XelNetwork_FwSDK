/*
 *  eeprom.h
 *
 *  Created on: 2017. 3.17.
 *      Author: Baram
 */

#ifndef EEPROM_H_
#define EEPROM_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_EEPROM



bool eepromInit();

uint8_t  eepromReadByte(uint32_t addr);
bool     eepromWriteByte(uint32_t index, uint8_t data_in);
uint32_t eepromGetLength(void);
bool     eepromFormat(void);


#endif /* _USE_HW_EEPROM */

#ifdef __cplusplus
}
#endif


#endif /* EEPROM_H_ */

