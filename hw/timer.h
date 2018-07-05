/*
 * timer.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef TIMER_H_
#define TIMER_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_TIMER


bool timerInit(void);


void timerStop(uint8_t channel);
void timerSetPeriod(uint8_t channel, uint32_t period_data);
void timerAttachInterrupt(uint8_t channel, voidFuncPtr handler);
void timerDetachInterrupt(uint8_t channel);
void timerStart(uint8_t channel);

uint32_t timerGetMicros(void);

#endif /* _USE_HW_TIMER */


#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */
