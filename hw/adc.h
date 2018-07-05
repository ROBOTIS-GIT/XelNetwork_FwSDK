/*
 * adc.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef ADC_H_
#define ADC_H_



#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_ADC

#if 1
bool     adcInit(void);
bool     adcSetup(uint8_t ch);

uint16_t adcRead(uint8_t channel);
uint16_t adcRead8(uint8_t channel);
uint16_t adcRead10(uint8_t channel);
uint16_t adcRead12(uint8_t channel);
uint16_t adcRead16(uint8_t channel);
uint16_t adcReadVoltage(uint8_t channel);
uint16_t adcConvVoltage(uint8_t channel, uint32_t adc_value);

uint8_t  adcGetRes(uint8_t channel);
#else

typedef struct
{
    void      (*init)(void);
    uint16_t  (*read)(uint8_t channel);
    uint16_t  (*read8)(uint8_t channel);
    uint16_t  (*read10)(uint8_t channel);
    uint16_t  (*read12)(uint8_t channel);
    uint16_t  (*read16)(uint8_t channel);

    uint16_t  (*readVoltage)(uint8_t channel);

    uint16_t  (*convertVoltage)(uint8_t channel, uint32_t adc_value);

    uint8_t   (*getResolution)(uint8_t channel);
};
#endif



#endif /* _USE_HW_ADC */

#ifdef __cplusplus
}
#endif


#endif /* ADC_H_ */
