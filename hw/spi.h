/*
 * spi.h
 *
 *  Created on: 2017. 10. 19.
 *      Author: Baram
 */

#ifndef SPI_H_
#define SPI_H_



#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_SPI


bool      spiInit(void);

void      spiBegin(uint8_t spi_ch);
void      spiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder);
void      spiSetClockDivider(uint8_t spi_ch, uint8_t clockDiv);
void      spiSetDataMode(uint8_t spi_ch, uint8_t dataMode);

bool      spiWrite(uint8_t spi_ch, uint8_t *p_data, uint32_t length);
bool      spiWrite8(uint8_t spi_ch, uint8_t data);
bool      spiWrite16(uint8_t spi_ch, uint16_t data);

bool      spiRead(uint8_t spi_ch, uint8_t *p_data, uint32_t length);
bool      spiRead8(uint8_t spi_ch, uint8_t *p_data);
bool      spiRead16(uint8_t spi_ch, uint16_t *p_data);

bool      spiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length);
uint8_t   spiTransfer8(uint8_t spi_ch, uint8_t data);
uint16_t  spiTransfer16(uint8_t spi_ch, uint16_t data);

void      spiSetTimeout(uint8_t ch, uint32_t timeout);
uint32_t  spiGetTimeout(uint8_t ch);

void      spiSetCS(uint8_t ch, uint8_t state);

#endif /* _USE_HW_SPI */


#ifdef __cplusplus
 }
#endif


#endif
