/*
 * encoder.h
 *
 *  Created on: 2017. 4. 11.
 *      Author: D.ggavy
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_ENCODER

bool     encoderInit(void);

uint16_t encoderReadValue(uint8_t ch);
uint32_t encoderReadCount(uint8_t ch);
uint32_t encoderReadSpeed(uint8_t ch);


#endif /* _USE_HW_ENCODER */


#ifdef __cplusplus
}
#endif


#endif /* ENCODER_H_ */
