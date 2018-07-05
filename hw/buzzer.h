/*
 * buzzer.h
 *
 *  Created on: 2017. 4. 7.
 *      Author: baram
 */

#ifndef BUZZER_H_
#define BUZZER_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_BUZZER


bool buzzerInit(void);

void buzzerOn(uint32_t freq, uint32_t time);
void buzzerOff(void);

void buzzerAddPeriod(int32_t period);

void buzzerStop(void);
void buzzerStart(void);

#endif /* _USE_HW_BUZZER */


#ifdef __cplusplus
}
#endif

#endif /* BUZZER_H_ */
