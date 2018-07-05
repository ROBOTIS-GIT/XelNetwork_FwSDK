/*
 * uart.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef UART_H_
#define UART_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_UART


bool uartInit(void);

err_code_t  uartOpen(uint8_t channel, uint32_t baud);
err_code_t  uartClose(uint8_t channel);
uint32_t    uartAvailable(uint8_t channel);
void        uartWaitForEnable(uint8_t channel, uint32_t timeout);
void        uartFlush(uint8_t channel);
void        uartPutch(uint8_t channel, uint8_t ch);
uint8_t     uartGetch(uint8_t channel);
int32_t     uartWrite(uint8_t channel, uint8_t *p_data, uint32_t length);
uint8_t     uartRead(uint8_t channel);
int32_t     uartPrintf(uint8_t channel, const char *fmt, ...);
int32_t     uartPrint(uint8_t channel, uint8_t *p_str);

uint32_t    uartGetTxErrCount(uint8_t channel);
uint32_t    uartGetTxCount(uint8_t channel);
uint32_t    uartGetTxRetryCount(uint8_t channel);

uint32_t    uartGetRxCount(uint8_t channel);

#endif /* _USE_HW_UART */


#ifdef __cplusplus
}
#endif

#endif /* UART_H_ */
