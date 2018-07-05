/*
 * pwm.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */



#include "pwm.h"

#ifdef _USE_HW_PWM
#include <stdarg.h>
#include <stdbool.h>
#include "driver/drv_pwm.h"
#include "hw.h"

//-- Internal Variables
//

#ifdef _USE_HW_CMDIF_PWM
void pwmCmdifInit(void);
int  pwmCmdif(int argc, char **argv);
#endif

//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//




bool pwmInit(uint32_t freq)
{
  bool err;

  err = drvPwmInit(freq);

  if(err == true)
  {
#ifdef _USE_HW_CMDIF_PWM
  pwmCmdifInit();
#endif
  }

  return err;
}

bool pwmSetup(uint8_t channel)
{
  if(drvPwmSetup(channel) == false)
  {
    return false;
  }

  return true;
}

void pwmSetDuty(uint8_t channel, uint8_t duty)
{
  drvPwmSetDuty(channel, duty);
}

void pwmSetPulse(uint8_t channel, uint32_t res, uint32_t pulse)
{
  drvPwmSetPulse(channel, res, pulse);
}

uint8_t  pwmGetDuty(uint8_t channel)
{
  return drvPwmGetDuty(channel);
}

uint32_t pwmGetPulse(uint8_t channel)
{
  return drvPwmGetPulse(channel);
}

#ifdef _USE_HW_CMDIF_PWM
void pwmCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("pwm", pwmCmdif);
}

int pwmCmdif(int argc, char **argv)
{
  bool ret = true;

  if (argc == 5)
  {
    char    *cmd     = argv[1];
    uint8_t  channel = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0) - 1;
    char    *type    = argv[3];
    uint32_t value   = strtoul((const char * ) argv[4], (char **)NULL, (int) 0);

    if(strcmp(cmd, "write") == 0)
    {
      if(strcmp(type, "duty") == 0)
      {
        pwmSetDuty(channel, value);
      }
      else if(strcmp(type, "pulse") == 0)
      {
        pwmSetPulse(channel, 8, value);
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
  }
  else if((argc == 3)&&(strcmp(argv[1], "read") == 0))
  {
    uint8_t  channel = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0) - 1;

    while(cmdifRxAvailable() == 0)
    {
      cmdifPrintf("pwm %d : %d\%(duty), %d(pulse)\n", channel, pwmGetDuty(channel), pwmGetPulse(channel));
      delay(500);
    }
  }
  else if((argc == 4)&&(strcmp(argv[1], "setup") == 0))
  {
    uint8_t  channel = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0) - 1;
    uint32_t freq    = (uint32_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0) - 1;

    ret = pwmInit(freq);
    if(ret == true)
    {
      ret = pwmSetup(channel);
    }
  }
  else if((argc == 2)&&(strcmp(argv[1], "demo") == 0))
  {
    uint8_t i, pulse = 0;

    while(cmdifRxAvailable() == 0)
    {
      for(i = 0; i < _HW_DEF_PWM_CH_MAX; i++)
      {
        pwmSetPulse(i, 8, pulse++);
      }
      delay(20);
    }
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf( "pwm write channel[1~%d] duty  value[0 ~ 100] ...\n", _HW_DEF_PWM_CH_MAX);
    cmdifPrintf( "pwm write channel[1~%d] pulse value[0 ~ 255] ...\n", _HW_DEF_PWM_CH_MAX);
    cmdifPrintf( "pwm read channel[1~%d] ...\n", _HW_DEF_PWM_CH_MAX);
    cmdifPrintf( "pwm setup channel[1~%d] frequency ...\n", _HW_DEF_PWM_CH_MAX);
  }

  return 0;
}

#endif /* _USE_HW_CMDIF_PWM */

#endif /* _USE_HW_PWM */
