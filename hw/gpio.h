/*
 * gpio.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef GPIO_H_
#define GPIO_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_GPIO

bool gpioInit(void);
void gpioPinMode(uint8_t channel, uint8_t mode);
void gpioPinWrite(uint8_t channel, uint8_t value);
int gpioPinRead(uint8_t channel);

#endif /* _USE_HW_GPIO */

#ifdef __cplusplus
}
#endif



#endif /* GPIO_H_ */
