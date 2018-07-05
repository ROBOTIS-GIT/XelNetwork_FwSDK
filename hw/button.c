/*
 * button.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#include "button.h"

#ifdef _USE_HW_BUTTON

#include <stdbool.h>
#include "hw.h"
#include "driver/drv_button.h"


typedef struct
{
  bool        pressed;
  bool        pressed_event;
  uint16_t    pressed_cnt;
  uint32_t    pressed_start_time;
  uint32_t    pressed_end_time;

  bool        released;
  bool        released_event;
  uint32_t    released_start_time;
  uint32_t    released_end_time;

} button_t;





//-- Internal Variables
//
static button_t button_tbl[_HW_DEF_BUTTON_CH_MAX];


//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_BUTTON
void buttonCmdifInit(void);
int  buttonCmdif(int argc, char **argv);
#endif


//-- External Functions
//




void button_isr(void *arg)
{
  uint8_t i;


  for (i=0; i<_HW_DEF_BUTTON_CH_MAX; i++)
  {

    if (buttonIsPressed(i))
    {
      if (button_tbl[i].pressed == false)
      {
        button_tbl[i].pressed_start_time = millis();
      }

      button_tbl[i].pressed       = true;
      button_tbl[i].pressed_event = true;
      button_tbl[i].pressed_cnt++;

      button_tbl[i].pressed_end_time = millis();

      button_tbl[i].released = false;
    }
    else
    {
      if (button_tbl[i].pressed == true)
      {
        button_tbl[i].released_event = true;
        button_tbl[i].released_start_time = millis();
      }

      button_tbl[i].pressed  = false;
      button_tbl[i].released = true;

      button_tbl[i].released_end_time = millis();
    }
  }
}




bool buttonInit(void)
{
  swtimer_handle_t h_button_timer;
  uint32_t i;


  drvButtonInit();


  for (i=0; i<_HW_DEF_BUTTON_CH_MAX; i++)
  {
    button_tbl[i].pressed_cnt    = 0;
    button_tbl[i].pressed        = 0;
    button_tbl[i].released       = 0;
    button_tbl[i].released_event = 0;
  }


  h_button_timer = swtimerGetHandle();
  swtimerSet(h_button_timer, 1, LOOP_TIME, button_isr, NULL );
  swtimerStart(h_button_timer);



#ifdef _USE_HW_CMDIF_BUTTON
  buttonCmdifInit();
#endif


  return true;
}


bool buttonIsPressed(uint8_t ch)
{
  bool button_pressed;


  if (buttonGetState(ch) > 0)
  {
    button_pressed = true;
  }
  else
  {
    button_pressed = false;
  }


  return button_pressed;
}

uint8_t buttonGetState(uint8_t ch)
{
  return drvButtonGetState(ch);
}

uint32_t buttonGetData(void)
{
  uint32_t ret = 0;
  uint32_t i;


  for(i=0; i<_HW_DEF_BUTTON_CH_MAX; i++)
  {
    ret |= (buttonIsPressed(i)<<i);
  }

  return ret;
}

bool buttonClick(uint8_t ch)
{
  uint8_t ret;
  uint8_t cnt = 0;

  while(buttonIsPressed(ch))
  {
    delay(50);

    if(buttonIsPressed(ch))
    {
      cnt = 1;
      ret = true;
    }
  }

  if(!cnt) ret = false;

  return ret;
}

bool buttonGetPressed(uint8_t ch)
{
  bool ret;


  if (ch >= _HW_DEF_BUTTON_CH_MAX) return false;

  ret = button_tbl[ch].pressed;

  return ret;
}

bool buttonGetPressedEvent(uint8_t ch)
{
  bool ret;


  if (ch >= _HW_DEF_BUTTON_CH_MAX) return false;

  ret = button_tbl[ch].pressed_event;

  button_tbl[ch].pressed_event = 0;

  return ret;
}

uint32_t buttonGetPressedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= _HW_DEF_BUTTON_CH_MAX) return 0;


  ret = button_tbl[ch].pressed_end_time - button_tbl[ch].pressed_start_time;

  return ret;
}


bool buttonGetReleased(uint8_t ch)
{
  bool ret;


  if (ch >= _HW_DEF_BUTTON_CH_MAX) return false;

  ret = button_tbl[ch].released;

  return ret;
}

bool buttonGetReleasedEvent(uint8_t ch)
{
  bool ret;


  if (ch >= _HW_DEF_BUTTON_CH_MAX) return false;

  ret = button_tbl[ch].released_event;

  button_tbl[ch].released_event = 0;

  return ret;
}

uint32_t buttonGetReleasedTime(uint8_t ch)
{
  volatile uint32_t ret;


  if (ch >= _HW_DEF_BUTTON_CH_MAX) return 0;


  ret = button_tbl[ch].released_end_time - button_tbl[ch].released_start_time;

  return ret;
}



#ifdef _USE_HW_CMDIF_BUTTON
void buttonCmdifInit(void)
{
  cmdifAdd("button", buttonCmdif);
}

int buttonCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t ch;
  uint32_t cnt = 0;
  uint32_t i;


  if (argc == 2)
  {
    if(strcmp("show", argv[1]) == 0)
    {
      while(cmdifRxAvailable() == 0)
      {
        for (i=0; i<_HW_DEF_BUTTON_CH_MAX; i++)
        {
          cmdifPrintf("%d", buttonGetPressed(i));
        }
        cmdifPrintf("\r");
        delay(50);
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (argc == 3)
  {
    ch = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0) - 1;

    if(strcmp("led", argv[1]) == 0)
    {
      while(cmdifRxAvailable() == 0)
      {
        if(buttonGetPressed(ch))
        {
          if(cnt)
          {
            cnt = 0;
            ledOn(0);

          }
          else
          {
            cnt = 1;
            ledOff(0);

          }
        }
      }
    }
    else if(strcmp("cnt", argv[1])==0)
    {
      while(cmdifRxAvailable() == 0)
      {
        if(buttonClick(ch))
        {
          cnt++;
        }

        cmdifPrintf("cnt is :  %d\n", cnt);
      }
    }
    else if(strcmp("time", argv[1])==0)
    {
      while(cmdifRxAvailable() == 0)
      {
        if(buttonGetPressed(ch))
        {
          cmdifPrintf("Time :  %d ms\n", buttonGetPressedTime(ch));
        }
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
    cmdifPrintf( "button [cmd] channel(1~5) ...\n");
    cmdifPrintf( "\t show/led/cnt/time \n");
  }

  return 0;
}
#endif /* _USE_HW_CMDIF_BUTTON */

#endif /* _USE_HW_BUTTON */
