/*
 * dac.h
 *
 *  Created on: 2017. 5. 25.
 *      Author: baram
 */

#ifndef DAC_H_
#define DAC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_DAC

void dacInit(void);
void dacClose(void);
bool dacSetup(uint32_t hz);
bool dacStart(uint8_t dac_ch);
void dacStop(uint8_t dac_ch);
void dacWrite(uint8_t dac_ch, uint16_t data);
uint32_t dacAvailable(uint8_t dac_ch);

#endif /* _USE_HW_DAC */

#ifdef __cplusplus
}
#endif



#endif /* DAC_H_ */
