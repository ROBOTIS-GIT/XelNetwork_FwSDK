/*
 * exti.c
 *
 *  Created on: 2017. 11. 13.
 *      Author: opus
 */

#include "exti.h"

#ifdef _USE_HW_EXTI

#include "driver/drv_exti.h"
#include "hw.h"

#ifdef _USE_HW_CMDIF_EXTI
void extiCmdifInit(void);
int extiCmdif(int argc, char **argv);
void extiCmdifCallbackFunc(void *arg);
#endif

void extiInit(void)
{
  drvExtiInit();

#ifdef _USE_HW_CMDIF_EXTI
  extiCmdifInit();
#endif
}

bool extiAttachInterrupt(uint8_t ch, uint32_t mode, void (*func)(void *), void *arg)
{
  if (!drvExtiAttachInterrupt(ch, mode, func, arg))
    return false;

  return true;
}

void extiDetachInterrupt(uint8_t ch)
{
  drvExtiDetachInterrupt(ch);
}

#ifdef _USE_HW_CMDIF_EXTI

void extiCmdifInit(void)
{
  cmdifAdd("exti", extiCmdif);
}

int extiCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t print_ch;
  uint8_t ch;
  uint8_t all_ch[_HW_DEF_USER_EXTI_MAX] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

  if (argc == 3)
  {
    if (strcmp("get", argv[1]) == 0)
    {
      print_ch = (uint8_t) strtoul((const char *) argv[2], (char **) NULL, (int) 0);
      ch       = print_ch - 1;

      extiAttachInterrupt(ch, _DEF_EXTI_RISING, extiCmdifCallbackFunc, &print_ch);

      while (cmdifRxAvailable() == 0)
      {

      }
      extiDetachInterrupt(ch);
    }
    else if (!strcmp("demo", argv[1]) && !strcmp("all", argv[2]))
    {
      for(ch = 0; ch < _HW_DEF_USER_EXTI_MAX; ch++)
      {
        extiAttachInterrupt(ch, _DEF_EXTI_RISING, extiCmdifCallbackFunc, &all_ch[ch]);
      }

      while (cmdifRxAvailable() == 0)
      {

      }

      for(ch = 0; ch < _HW_DEF_USER_EXTI_MAX; ch++)
      {
        extiDetachInterrupt(ch);
      }
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
    cmdifPrintf("exti get  channel[1~16]\n");
    cmdifPrintf("exti demo all\n");
  }
  return 0;
}

void extiCmdifCallbackFunc(void *arg)
{
  cmdifPrintf("%d EXTI Detected! \n", *(uint8_t*)arg);
}

#endif /* _USE_HW_CMDIF_EXTI */

#endif /* _USE_HW_EXTI */
