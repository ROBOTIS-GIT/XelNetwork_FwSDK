/*
 * timer.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */





#include "timer.h"

#ifdef _USE_HW_TIMER

#include <stdbool.h>
#include "hw.h"
#include "driver/drv_timer.h"




//-- Internal Variables
//




//-- External Variables
//



//-- Internal Functions
//


//-- External Functions
//



bool timerInit(void)
{
  drvTimerInit();

  return true;
}

void timerStop(uint8_t channel)
{
  drvTimerStop(channel);
}

void timerSetPeriod(uint8_t channel, uint32_t period_data)
{
  drvTimerSetPeriod(channel, period_data);
}

void timerAttachInterrupt(uint8_t channel, voidFuncPtr handler)
{
  drvTimerAttachInterrupt(channel, handler);
}

void timerDetachInterrupt(uint8_t channel)
{
  drvTimerDetachInterrupt(channel);
}

void timerStart(uint8_t channel)
{
  drvTimerStart(channel);
}

uint32_t timerGetMicros(void)
{
  return drvTimerGetMicros();
}

#endif /* _USE_HW_TIMER */





