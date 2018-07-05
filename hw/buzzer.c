/*
 * buzzer.c
 *
 *  Created on: 2017. 4. 7.
 *      Author: baram
 */



#include "buzzer.h"

#ifdef _USE_HW_BUZZER

#include <stdbool.h>
#include "hw.h"
#include "driver/drv_buzzer.h"
#include "driver/drv_timer.h"

//-- Internal Variables
//

// timerx_toggle_count:
//  > 0 - duration specified
//  = 0 - stopped
//  < 0 - infinitely (until stop() method called, or new play() called)

volatile int32_t  tone_toggle_freq;
volatile int32_t  tone_toggle_count;
volatile int32_t  tone_toggle_period;
volatile uint8_t  tone_timer = _HW_DEF_BUZZER_TIMER;
volatile bool     buzzer_enable = false;
volatile bool     buzzer_no_timeout = false;

//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_BUZZER
void buzzerCmdifInit(void);
int  buzzerCmdif(int argc, char **argv);
#endif


//-- External Functions
//


void buzzer_isr( void )
{
  static uint8_t tone_pin_out = 0;


  if (tone_toggle_count > 0 || buzzer_no_timeout == true)
  {
    // toggle the pin
    tone_pin_out ^= 1;

    if (buzzer_enable)
    {
      drvBuzzerSetPin(tone_pin_out);
    }
    else
    {
      drvBuzzerSetPin(0);
    }

    if(tone_toggle_count > 0)
    {
      tone_toggle_count--;
    }
  }
  else
  {
    buzzerOff();
  }
}



bool buzzerInit(void)
{
  drvBuzzerInit();


#ifdef _USE_HW_CMDIF_BUZZER
  buzzerCmdifInit();
#endif

  return true;
}

void buzzerOn(uint32_t freq, uint32_t time)
{

  uint32_t toggle_count = 0;
  uint32_t ocr = 0;


  ocr = (8000000 / freq) / 2 ;


  if (time > 0)
  {
    toggle_count = 2 * freq * time / 1000;
  }
  else
  {
    toggle_count = -1;
  }

  tone_toggle_freq   = freq;
  tone_toggle_count  = toggle_count;
  tone_toggle_period = ocr;

  buzzer_enable = true;

  if (time == 0)
  {
    buzzer_no_timeout = true;
  }
  else
  {
    buzzer_no_timeout = false;
  }

  drvTimerStop(tone_timer);
  drvTimerSetPeriod(tone_timer, ocr);
  drvTimerAttachInterrupt(tone_timer, buzzer_isr);
  drvTimerStart(tone_timer);
}

void buzzerAddPeriod(int32_t period)
{
  drvTimerAddPeriod(tone_timer, period);
}

void buzzerOff(void)
{
  buzzer_enable = false;
  buzzer_no_timeout = false;
  drvTimerStop(tone_timer);
  drvBuzzerSetPin(0);
}

void buzzerStop(void)
{
  buzzer_enable = false;
}

void buzzerStart(void)
{
  buzzer_enable = true;
}



#ifdef _USE_HW_CMDIF_BUZZER
#define NOTE_C4  262
#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_G3  196
#define NOTE_B3  247
#define NOTE_C4  262


int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void buzzerCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("buzzer", buzzerCmdif);
}

int buzzerCmdif(int argc, char **argv)
{
  bool ret = true;
  uint32_t freq;
  uint32_t time;


  if (argc == 2)
  {
    if(strcmp("off", argv[1]) == 0)
    {
      buzzerOff();
    }
    else if(strcmp("demo", argv[1]) == 0)
    {
      for (int thisNote = 0; thisNote < 8; thisNote++)
      {
        int noteDuration = 1000 / noteDurations[thisNote];
        buzzerOn(melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        buzzerOff();
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (argc == 4)
  {
    freq = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    time = (uint32_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);

    if(strcmp("on", argv[1]) == 0 && argc == 4)
    {
      buzzerOn(freq, time);
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
    cmdifPrintf( "buzzer on [freq] [time(ms)] \n");
    cmdifPrintf( "buzzer off \n");
    cmdifPrintf( "buzzer demo \n");
  }

  return 0;
}
#endif /* _USE_HW_CMDIF_BUZZER */

#endif /* _USE_HW_BUZZER */
