/*
 * gpio.c
 *
 *  Created on: 2017. 8. 11.
 *      Author: baram
 */



#include "gpio.h"

#ifdef _USE_HW_GPIO

#include <stdbool.h>
#include "hw.h"
#include "driver/drv_gpio.h"


//-- Internal Variables
//

//-- External Variables
//

//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_GPIO
void gpioCmdifInit(void);
int gpioCmdif(int argc, char **argv);
#endif

//-- External Functions
//

bool gpioInit(void)
{
  drvGpioInit();

#ifdef _USE_HW_CMDIF_GPIO
  gpioCmdifInit();
#endif

  return true;
}

void gpioPinMode(uint8_t channel, uint8_t mode)
{
  drvGpioPinMode(channel, mode);
}

void gpioPinWrite(uint8_t channel, uint8_t value)
{
  drvGpioPinWrite(channel, value);
}

int gpioPinRead(uint8_t channel)
{
  return drvGpioPinRead(channel);
}



#ifdef _USE_HW_CMDIF_GPIO
void gpioCmdifInit(void)
{
  cmdifAdd("gpio", gpioCmdif);
}

int gpioCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t print_ch;
  uint8_t channel;
  uint32_t option = 0;

  if (argc < 4)
  {
    ret = false;
  }
  else
  {
    print_ch = (uint8_t) strtoul((const char *) argv[2], (char **) NULL, (int) 0);
    option   = (uint32_t) strtoul((const char *) argv[3], (char **) NULL, (int) 0);
    channel  = print_ch - 1;

    if (strcmp("mode", argv[1]) == 0)
    {
      gpioPinMode(channel, option);
    }
    else if (strcmp("get", argv[1]) == 0)
    {
      while (cmdifRxAvailable() == 0)
      {
        cmdifPrintf("Pin %d : %d, delay : %d\r\n", print_ch, gpioPinRead(channel), option);
        delay(option);
      }
    }
    else if (strcmp("set", argv[1]) == 0)
    {
      gpioPinWrite(channel, option);
    }
    else if (strcmp("test", argv[1]) == 0)
    {
      gpioPinMode(channel, option);

      if(option < 3)  //read GPIO pin only for Input setting
      {
        while (cmdifRxAvailable() == 0)
        {
          cmdifPrintf("[Ch %d] Value : %d\r\n", channel, gpioPinRead(channel));
          delay(1000);
        }
      }
    }
    else
    {
      ret = false;
    }
  }

  if (ret == false)
  {
    cmdifPrintf("\n");
    cmdifPrintf("gpio get  channel[1~%d] [interval (ms)]\n", _HW_DEF_GPIO_CH_MAX);
    cmdifPrintf("gpio set  channel[1~%d] [bit]\n", _HW_DEF_GPIO_CH_MAX);
    cmdifPrintf("gpio test channel[1~%d] [mode]\n", _HW_DEF_GPIO_CH_MAX);
    cmdifPrintf("gpio mode channel[1~%d] [mode]\n", _HW_DEF_GPIO_CH_MAX);
    cmdifPrintf("   *[bit] option  : 0(Low), 1(High)\n");
    cmdifPrintf("   *[mode] option : 0(INPUT),  1(INPUT_PULLUP),  2(INPUT_PULLDOWN)\n");
    cmdifPrintf("                    3(OUTPUT), 4(OUTPUT_PULLUP), 5(OUTPUT_PULLDOWN)\n");
  }
  return 0;
}
#endif /* _USE_HW_CMDIF_GPIO */

#endif /* _USE_HW_GPIO */
