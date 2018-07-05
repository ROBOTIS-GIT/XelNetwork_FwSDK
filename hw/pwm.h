/*
 * pwm.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef PWM_H_
#define PWM_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_PWM


bool pwmInit(uint32_t freq);
bool pwmSetup(uint8_t channel);

void pwmSetDuty(uint8_t channel, uint8_t duty);
void pwmSetPulse(uint8_t channel, uint32_t res, uint32_t pulse);
uint8_t  pwmGetDuty(uint8_t channel);
uint32_t pwmGetPulse(uint8_t channel);


#endif /* _USE_HW_PWM */

#ifdef __cplusplus
}
#endif

#endif /* PWM_H_ */
