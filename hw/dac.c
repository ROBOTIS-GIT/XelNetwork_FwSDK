/*
 * dac.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */



#include "dac.h"

#ifdef _USE_HW_DAC

#include <stdbool.h>
#include "driver/drv_dac.h"
#include "hw.h"

//-- Internal Variables
//

//-- External Variables
//

//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_DAC
void dacCmdifInit(void);
int dacCmdif(int argc, char **argv);
#endif

//-- External Functions
//

void dacInit()
{
  drvDacDeInit();
  drvDacInit();

#ifdef _USE_HW_CMDIF_DAC
  dacCmdifInit();
#endif
}

void dacClose(void)
{
  drvDacDeInit();
}

bool dacSetup(uint32_t hz)
{
  if (!drvDacSetup(hz))
    return false;

  return true;
}

bool dacStart(uint8_t dac_ch)
{
  if (!drvDacStart(dac_ch))
    return false;

  return true;
}

void dacStop(uint8_t dac_ch)
{
  drvDacStop(dac_ch);
}

void dacWrite(uint8_t dac_ch, uint16_t data)
{
  drvDacWrite(dac_ch, data);
}

uint32_t dacAvailable(uint8_t dac_ch)
{
  return drvDacAvailable(dac_ch);
}

#ifdef _USE_HW_CMDIF_DAC
void dacCmdifInit(void)
{
  cmdifAdd("dac", dacCmdif);
}

int dacCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t print_ch;
  uint8_t ch;
  uint32_t hz;
  uint16_t idx = 0;
  uint8_t sw = 0;

  if (argc == 4)
  {
    if (strcmp("demo", argv[1]) == 0)
    {
      print_ch = (uint8_t) strtoul((const char *) argv[2], (char **) NULL, (int) 0);
      hz       = (uint32_t) strtoul((const char *) argv[3], (char **) NULL, (int) 0);
      ch = print_ch - 1;

      if (!dacSetup(hz))
        return 0;

      cmdifPrintf("dacSetup : %d channel %d hz\n", print_ch, hz);

      if (!dacStart(ch))
        return 0;

      while (cmdifRxAvailable() == 0)
      {
        uint32_t length = dacAvailable(ch);

        for (uint32_t i = 0; i < length; i++)
        {
          if (sw == 0 && idx == 4095)
            sw = 1;
          else if (sw == 1 && idx == 0)
            sw = 0;

          if (sw == 0)
            dacWrite(ch, idx++);
          else
            dacWrite(ch, idx--);
        }
      }
      dacStop(ch);
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf("dac demo channel[1~2] hz\n");
  }

  return 0;
}
#endif /* _USE_HW_CMDIF_DAC */

#endif /* _USE_HW_DAC */
