/*
 * encoder.c
 *
 *  Created on: 2017. 4. 11.
 *      Author: D.ggavy
 */


#include "encoder.h"

#ifdef _USE_HW_ENCODER

#include <stdarg.h>
#include <stdbool.h>
#include "driver/drv_encoder.h"
#include "hw.h"



//-- Internal Variables
//
void encoderCmdifInit(void);
int  encoderCmdif(int argc, char **argv);


//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//
bool encoderInit(void)
{
  drvEncoderInit();


#ifdef _USE_HW_CMDIF_ENCODER
  encoderCmdifInit();
#endif

  return true;
}

uint16_t encoderReadValue(uint8_t ch)
{
  return drvEncoderReadValue(ch);
}

uint32_t encoderReadCount(uint8_t ch)
{
  return drvEncoderReadCount(ch);
}

uint32_t encoderReadSpeed(uint8_t ch)
{
  return drvEncoderReadSpeed(ch);
}


#ifdef _USE_HW_CMDIF_ENCODER
void encoderCmdifInit(void)
{
  if(cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("enc", encoderCmdif);
}

int encoderCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t number = LEFT;
  uint8_t option;
  int16_t encValue;
  int16_t t_time;
  uint32_t pre_time;
  int16_t pre_enc_value = 0;
  uint32_t cur_enc_count = 0;
  uint32_t pre_enc_count = 0;


  if (argc < 4)
  {
    ret = false;
  }
  else
  {
    number = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    option = (uint8_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);

    if(strcmp("show", argv[1])==0)
    {
      pre_time = micros();
      while(((LEFT == number)||(RIGHT == number)) && (cmdifRxAvailable() == 0))
      {

        if (micros()-pre_time >= 10000)
        {
          pre_time = micros();

          t_time   = micros();
          encValue = encoderReadValue(number);
          t_time   = micros() - t_time;

          if (option == 0)
          {
            cmdifPrintf("angle Of %d : %d\n", number, encValue);
          }
          else if (option == 1)
          {
            cmdifPrintf("time : %d\n", t_time);
          }
          else if( option == 2)
          {
            cmdifPrintf("%d\t %d\t %d\n", encValue, encValue-pre_enc_value, drvEncoderReadCount(number));
          }
          else if( option == 3)
          {
            cur_enc_count = drvEncoderReadCount(number);

            cmdifPrintf("%d\t %d\t\n", cur_enc_count, cur_enc_count-pre_enc_count);
          }

          pre_enc_value = encValue;
          pre_enc_count = cur_enc_count;
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
    cmdifPrintf( "enc [show] number option \n");
  }

  return 0;
}

#endif /* _USE_HW_CMDIF_ENCODER */

#endif /* _USE_HW_ENCODER */
