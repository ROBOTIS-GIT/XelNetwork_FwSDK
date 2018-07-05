/*
 * log.c
 *
 *  Created on: 2017. 8. 25.
 *      Author: baram
 */






#include "led.h"

#ifdef _USE_HW_LOG

#include <stdarg.h>
#include <stdbool.h>
#include "hw.h"




//-- Internal Variables
//
static bool    log_enable = false;
static uint8_t log_ch     = _DEF_UART1;


//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//




bool logInit(void)
{

  return true;
}

void logOn(void)
{
  log_enable = true;
}

void logOff()
{
  log_enable = false;
}

void logChangeChannel(uint8_t ch)
{
  log_ch = ch;
}

void logPrintf( const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[255];

  if (log_enable == false)
  {
    return;
  }


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  switch(log_ch)
  {
    case _DEF_UART1:
    case _DEF_UART2:
    case _DEF_UART3:
    case _DEF_UART4:
      uartWrite(log_ch, (uint8_t *)print_buffer, len);
      break;
  }

  return;
}


#endif /* _USE_HW_LOG */
