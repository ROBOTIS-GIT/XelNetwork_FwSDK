/*
 * spi.c
 *
 *  Created on: 2017. 3. 11.
 *      Author: Baram
 */

/*

 SPI3
  SCK  PB3  AF6
  MISO PB4  AF6 DMA2 Channel1 Request3
  MOSI PB5  AF6 DMA2 Channel2 Request3

 */




#include "spi.h"

#ifdef _USE_HW_SPI

#include <stdbool.h>
#include "hw.h"
#include "driver/drv_spi.h"




bool spiInit(void)
{
  return drvSpiInit();
}

void spiBegin(uint8_t spi_ch)
{
  drvSpiBegin(spi_ch);
}

void spiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder)
{
  drvSpiSetBitOrder(spi_ch, bitOrder);
}

void spiSetClockDivider(uint8_t spi_ch, uint8_t clockDiv)
{
  drvSpiSetClockDivider(spi_ch, clockDiv);
}

void spiSetDataMode(uint8_t spi_ch, uint8_t dataMode)
{
  drvSpiSetDataMode(spi_ch, dataMode);
}

bool spiWrite(uint8_t spi_ch, uint8_t *p_data, uint32_t length)
{
  return drvSpiWrite(spi_ch, p_data, length);
}

bool spiWrite8(uint8_t spi_ch, uint8_t data)
{
  return drvSpiWrite8(spi_ch, data);
}

bool spiWrite16(uint8_t spi_ch, uint16_t data)
{
  return drvSpiWrite16(spi_ch, data);
}

bool spiRead(uint8_t spi_ch, uint8_t *p_data, uint32_t length)
{
  return drvSpiRead(spi_ch, p_data, length);
}

bool spiRead8(uint8_t spi_ch, uint8_t *p_data)
{
  return drvSpiRead8(spi_ch, p_data);
}

bool spiRead16(uint8_t spi_ch, uint16_t *p_data)
{
  return drvSpiRead16(spi_ch, p_data);
}

bool spiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length)
{
  return drvSpiTransfer(spi_ch, p_tx_data, p_rx_data, length);
}

uint8_t spiTransfer8(uint8_t spi_ch, uint8_t data)
{
  return drvSpiTransfer8(spi_ch, data);
}

uint16_t spiTransfer16(uint8_t spi_ch, uint16_t data)
{
  return drvSpiTransfer16(spi_ch, data);
}

void spiSetTimeout(uint8_t spi_ch, uint32_t timeout)
{
  drvSpiSetTimeout(spi_ch, timeout);
}

uint32_t spiGetTimeout(uint8_t spi_ch)
{
  return drvSpiGetTimeout(spi_ch);
}

void spiSetCS(uint8_t ch, uint8_t state)
{
  drvSpiSetCS(ch, state);
}

#endif /* _USE_HW_SPI */
