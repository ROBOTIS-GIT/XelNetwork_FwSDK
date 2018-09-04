/*
 * uart.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */





#include "uart.h"

#ifdef _USE_HW_UART
#include <stdbool.h>
#include "driver/drv_uart.h"
#include "hw.h"





//-- Internal Variables
//




//-- External Variables
//


//-- Internal Functions
//
static bool uartIsEnable(uint8_t channel);


//-- External Functions
//


//-- Driver Functions
//
extern bool      drvUartInit(void);
extern bool      drvUartOpen(uint8_t channel, uint32_t baud, uint32_t option);
extern bool      drvUartIsEnable(uint8_t channel);
extern uint32_t  drvUartIsAvailable(uint8_t channel);
extern int32_t   drvUartWrite(uint8_t channel, uint8_t *p_data, uint32_t length);
extern uint8_t   drvUartRead(uint8_t channel);
extern uint32_t  drvUartGetTxErrCount(uint8_t channel);
extern uint32_t  drvUartGetTxCount(uint8_t channel);
extern uint32_t  drvUartGetTxRetryCount(uint8_t channel);




bool uartInit(void)
{


  drvUartInit();

  return true;
}

err_code_t uartOpen(uint8_t channel, uint32_t baud)
{
  err_code_t err_code = OK;


  if (channel >= _HW_DEF_UART_CH_MAX)
  {
    return ERR_INVAILD_INDEX;
  }


  drvUartOpen(channel, baud, 0);


  return err_code;
}

err_code_t uartClose(uint8_t channel)
{
  err_code_t err_code = OK;


  if (channel >= _HW_DEF_UART_CH_MAX)
  {
    return ERR_INVAILD_INDEX;
  }


  drvUartClose(channel);


  return err_code;
}

bool uartIsEnable(uint8_t channel)
{
  if (channel >= _HW_DEF_UART_CH_MAX)
  {
    return false;
  }

  return drvUartIsEnable(channel);
}

void uartWaitForEnable(uint8_t channel, uint32_t timeout)
{
  uint32_t t_time;


  t_time = millis();
  while(1)
  {
    if (uartIsEnable(channel) == true)
    {
      break;
    }
    if ((millis()-t_time) >= timeout)
    {
      break;
    }
  }
}

void uartFlush(uint8_t channel)
{
  drvUartFlush(channel);
}

uint32_t uartAvailable(uint8_t channel)
{
  return drvUartIsAvailable(channel);
}

void uartPutch(uint8_t channel, uint8_t ch)
{
  uartWrite(channel, &ch, 1 );
}

uint8_t uartGetch(uint8_t channel)
{
  if (uartIsEnable(channel) == false ) return 0;


  while(1)
  {
    if( uartAvailable(channel) ) break;
  }


  return uartRead(channel);
}

int32_t uartWrite(uint8_t channel, uint8_t *p_data, uint32_t length)
{
  int32_t  ret;
  uint32_t t_time;


  if (uartIsEnable(channel) == false ) return 0;

  t_time = millis();
  while(1)
  {
    ret = drvUartWrite(channel, p_data, length );

    if(ret < 0)
    {
      ret = 0;
      break;
    }
    if(ret == length)
    {
      break;
    }
    if(millis()-t_time > 100)
    {
      ret = 0;
      break;
    }

  }
  return ret;
}

uint8_t uartRead(uint8_t channel)
{
  return drvUartRead(channel);
}

#if 1
int32_t uartPrintf(uint8_t channel, const char *fmt, ...)
{
  static bool busy = false;
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  int32_t len_out;
  static char print_buffer[255];
  static char print_buffer_out[255];
  uint32_t i;


  if (uartIsEnable(channel) == false ) return 0;
  if (busy == true) return 0;

  busy = true;

  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  len_out = 0;
  for (i=0; i<len; i++)
  {
    if (print_buffer[i] == '\n')
    {
      print_buffer_out[len_out++] = '\r';
    }
    print_buffer_out[len_out++] = print_buffer[i];
  }
  ret = uartWrite(channel, (uint8_t *)print_buffer_out, len_out);

  busy = false;

  return ret;
}

#endif

int32_t uartPrint(uint8_t channel, uint8_t *p_str)
{
  int32_t index = 0;

  if (uartIsEnable(channel) == false ) return 0;

  while(1)
  {
    uartPutch(channel, p_str[index]);

    if (p_str[index] == 0)
    {
      break;
    }

    index++;

    if (index > 255)
    {
      break;
    }
  }


  return index;
}

uint32_t uartGetTxErrCount(uint8_t channel)
{
  if (uartIsEnable(channel) == false ) return 0;

  return drvUartGetTxErrCount(channel);
}

uint32_t uartGetTxCount(uint8_t channel)
{
  if (uartIsEnable(channel) == false ) return 0;

  return drvUartGetTxCount(channel);
}

uint32_t uartGetTxRetryCount(uint8_t channel)
{
  if (uartIsEnable(channel) == false ) return 0;

  return drvUartGetTxRetryCount(channel);
}

uint32_t uartGetRxCount(uint8_t channel)
{
  if (uartIsEnable(channel) == false ) return 0;

  return drvUartGetRxCount(channel);
}

uint32_t uartGetBaud(uint8_t channel)
{
  return drvUartGetBaud(channel);
}

#endif /* _USE_HW_UART */
