/*
 * button.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef BUTTON_H_
#define BUTTON_H_



#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_BUTTON


bool buttonInit(void);

bool     buttonIsPressed(uint8_t ch);
uint8_t  buttonGetState(uint8_t ch);
uint32_t buttonGetData(void);


bool     buttonGetPressed(uint8_t ch);
bool     buttonGetPressedEvent(uint8_t ch);
uint32_t buttonGetPressedTime(uint8_t ch);


bool     buttonGetReleased(uint8_t ch);
bool     buttonGetReleasedEvent(uint8_t ch);
uint32_t buttonGetReleasedTime(uint8_t ch);

#endif  /* _USE_HW_BUTTON */


#ifdef __cplusplus
}
#endif



#endif /* BUTTON_H_ */
