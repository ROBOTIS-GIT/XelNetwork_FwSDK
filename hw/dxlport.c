/*
 * dxlport.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */



#include "dxlport.h"

#ifdef _USE_HW_DXLPORT
#include <stdarg.h>
#include <stdbool.h>
#include "driver/drv_dxlport.h"
#include "hw.h"
#include "uart.h"







//-- Internal Variables
//



//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//




bool dxlportInit(void)
{
  drvDxlportInit();

  return true;
}

bool dxlportOpen(uint8_t ch, uint32_t baud)
{
  bool ret = true;


  ret = drvDxlportOpen(ch, baud);

  return ret;
}

bool dxlportClose(uint8_t ch)
{
  bool ret = true;


  ret = drvDxlportClose(ch);

  return ret;
}

bool dxlportFlush(uint8_t ch)
{
  bool ret = true;


  ret = drvDxlportFlush(ch);

  return ret;
}

void dxlportTxEnable(uint8_t ch)
{

  drvDxlportTxEnable(ch);
}

void dxlportTxDisable(uint8_t ch)
{

  drvDxlportTxDisable(ch);
}

void dxlportSetTxDoneISR(uint8_t ch, void (*p_txDoneISR)(void))
{
  drvDxlportSetTxDoneISR(ch, p_txDoneISR);
}

uint32_t dxlportAvailable(uint8_t ch)
{
  uint32_t ret;


  ret = drvDxlportAvailable(ch);

  return ret;
}

uint8_t dxlportRead(uint8_t ch)
{
  uint8_t ret;


  ret = drvDxlportRead(ch);

  return ret;
}

uint32_t dxlportWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret;


  ret = drvDxlportWrite(ch, p_data, length);

  return ret;
}

uint32_t dxlportWritePoll(uint8_t ch, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  uint32_t ret;
  uint32_t pre_time;

  ret = drvDxlportWrite(ch, p_data, length);

  pre_time = millis();

  while(dxlportIsTxDone(ch) == false)
  {
    if (millis()-pre_time >= timeout)
    {
      ret = 0;
      break;
    }
  }
  return ret;
}

uint32_t dxlportGetBaud(uint8_t ch)
{
  return drvDxlportGetBaud(ch);
}

bool dxlportIsTxDone(uint8_t ch)
{
  return drvDxlportIsTxDone(ch);
}

int32_t dxlportPrintf(uint8_t ch, const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[255];


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  ret = drvDxlportWrite(ch, (uint8_t *)print_buffer, len);

  return ret;
}


#endif /* _USE_HW_DXLPORT */
