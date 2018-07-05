/*
 * dxlport.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef DXLPORT_H_
#define DXLPORT_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_DXLPORT


bool dxlportInit(void);


bool dxlportOpen(uint8_t ch, uint32_t baud);
bool dxlportClose(uint8_t ch);
bool dxlportFlush(uint8_t ch);
void dxlportTxEnable(uint8_t ch);
void dxlportTxDisable(uint8_t ch);
void dxlportSetTxDoneISR(uint8_t ch, void (*p_txDoneISR)(void));
bool dxlportIsTxDone(uint8_t ch);

uint32_t dxlportAvailable(uint8_t ch);
uint8_t  dxlportRead(uint8_t ch);
uint32_t dxlportWrite(uint8_t ch, uint8_t *p_data, uint32_t length);
uint32_t dxlportWritePoll(uint8_t ch, uint8_t *p_data, uint32_t length, uint32_t timeout);
int32_t  dxlportPrintf(uint8_t ch, const char *fmt, ...);

#endif /* _USE_HW_DXLPORT */

#ifdef __cplusplus
}
#endif

#endif /* DXLPORT_H_ */
