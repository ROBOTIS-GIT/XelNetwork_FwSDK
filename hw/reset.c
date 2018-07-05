/*
 *  reset.c
 *
 *  Created on: 2017. 6. 9.
 *      Author: Baram
 */

#include "reset.h"

#ifdef _USE_HW_RESET
#include "hw.h"
#include "driver/drv_reset.h"



//-- Internal Variables
//


//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_RESET
void resetCmdifInit(void);
int  resetCmdif(int argc, char **argv);
#endif

//-- External Functions
//


bool resetInit()
{
  bool ret;


  ret = drvResetInit();


#ifdef _USE_HW_CMDIF_RESET
  resetCmdifInit();
#endif

  return ret;
}

uint8_t resetGetStatus(void)
{
  return drvResetGetStatus();
}

void resetRunWdgReset(void)
{
  drvResetRunWdgReset();
}

void resetRunSoftReset(void)
{
  drvResetRunSoftReset();
}

void resetClearFlag(void)
{
  drvResetClearFlag();
}

#ifdef _USE_HW_CMDIF_RESET
void resetCmdifInit(void)
{
  cmdifAdd("reset", resetCmdif);
}

int resetCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t reset_ret;


  if (argc == 2)
  {
    if(strcmp("info", argv[1]) == 0)
    {
      reset_ret = resetGetStatus();

      switch(reset_ret)
      {
        case _DEF_RESET_POWER:
          cmdifPrintf( "_DEF_RESET_POWER\n");
          break;

        case _DEF_RESET_PIN:
          cmdifPrintf( "_DEF_RESET_PIN\n");
          break;

        case _DEF_RESET_WDG:
          cmdifPrintf( "_DEF_RESET_WDG\n");
          break;

        case _DEF_RESET_SOFT:
          cmdifPrintf( "_DEF_RESET_SOFT\n");
          break;
      }
    }
    else if(strcmp("wdg_reset", argv[1]) == 0)
    {
      resetRunWdgReset();
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
    cmdifPrintf( "reset info\n");
    cmdifPrintf( "reset wdg_reset\n");
  }

  return 0;
}
#endif /* _USE_HW_CMDIF_RESET */


#endif /* _USE_HW_RESET */
