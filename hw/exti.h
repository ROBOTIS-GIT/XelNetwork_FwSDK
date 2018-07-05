/*
 * exti.h
 *
 *  Created on: 2017. 11. 13.
 *      Author: opus
 */

#ifndef EXTI_H_
#define EXTI_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_EXTI

void extiInit(void);
bool extiAttachInterrupt(uint8_t ch, uint32_t mode, void (*func)(void *), void *arg);
void extiDetachInterrupt(uint8_t ch);

#endif

#ifdef __cplusplus
 }
#endif

#endif /* EXTI_H_ */
