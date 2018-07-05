/*
 * led.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef LED_H_
#define LED_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_LED


bool ledInit(void);

void ledOn(uint8_t ch);
void ledOff(uint8_t ch);
void ledToggle(uint8_t ch);

void ledOnAll(void);
void ledOffAll(void);


#endif /* _USE_HW_LED */

#ifdef __cplusplus
}
#endif

#endif /* LED_H_ */
