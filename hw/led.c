/*
 * led.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */





#include "hw_def.h"

#ifdef _USE_HW_LED
#include <stdbool.h>
#include "led.h"
#include "driver/drv_led.h"
#include "hw.h"




//-- Internal Variables
//
#ifdef _USE_HW_CMDIF_LED
void ledCmdifInit(void);
int  ledCmdif(int argc, char **argv);
#endif


//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//




bool ledInit(void)
{
  uint8_t i;


  drvLedInit();


  for (i=0; i<_HW_DEF_LED_CH_MAX; i++)
  {
    ledOff(i);
  }

#ifdef _USE_HW_CMDIF_LED
  ledCmdifInit();
#endif

  return true;
}

void ledOn(uint8_t ch)
{
  drvLedSetState(ch, true);
}

void ledOff(uint8_t ch)
{
  drvLedSetState(ch, false);
}

void ledToggle(uint8_t ch)
{
  bool led_state;


  led_state = drvLedGetState(ch);

  drvLedSetState(ch, !led_state);
}

void ledOnAll(void)
{
  uint16_t i;


  for (i=0; i<_HW_DEF_LED_CH_MAX; i++)
  {
    ledOn(i);
  }
}

void ledOffAll(void)
{
  uint16_t i;


  for (i=0; i<_HW_DEF_LED_CH_MAX; i++)
  {
    ledOff(i);
  }
}





#ifdef _USE_HW_CMDIF_LED
void ledCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("led", ledCmdif);
}

int ledCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t print_ch;
  uint8_t ch;


  if (argc < 3)
  {
    ret = false;
  }
  else
  {
    print_ch = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    ch = print_ch - 1;

    if(strcmp("on", argv[1]) == 0)
    {
      ledOn(ch);
    }
    else if(strcmp("off", argv[1])==0)
    {
      ledOff(ch);
    }
    else if(strcmp("toggle", argv[1])==0)
    {
      ledToggle(ch);
    }
    else if(strcmp("demo", argv[1])==0)
    {
      while(cmdifRxAvailable() == 0)
      {
        ledToggle(ch);
        delay(200);
        cmdifPrintf("led toggle %d\n", print_ch);
      }
    }
    else
    {
      ret = false;
    }
  }


  if (ret == false)
  {
    cmdifPrintf( "led [cmd] channel[1~5] ...\n");
    cmdifPrintf( "\t on/off/toggle/demo \n");
  }

  return 0;
}
#endif /* _USE_HW_CMDIF_LED */

#endif /* _USE_HW_LED */
