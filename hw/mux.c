/*
 * mux.c
 *
 *  Created on: 2018. 2. 27.
 *      Author: D.ggavy
 */



#include <stdarg.h>
#include <stdbool.h>

#include "mux.h"

#ifdef _USE_HW_MUX
#include "driver/drv_mux.h"
#include "hw.h"

#define MUX_MAX_CH   _HW_DEF_MUX_CH_MAX


//-- Internal Variables
#ifdef _USE_HW_CMDIF_MUX
void muxCmdifInit(void);
int  muxCmdif(int argc, char **argv);
#endif


//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//


void muxInit(void)
{
	uint8_t i;

	drvMuxInit();

	for(i=0; i<MUX_MAX_CH; i++)
	{
		muxEnable(i);
	}

#ifdef _USE_HW_CMDIF_MUX
	muxCmdifInit();
#endif
}

void muxEnable(uint8_t ch)
{
	drvMuxEnableId(ch, true);
}

void muxSetChannel(uint8_t id, uint8_t ch)
{
	if(muxScanChannel(ch))
	{
		drvMuxSetChannel(id,ch);
		//delay(10);
	}
}

uint8_t muxGetChannel(uint8_t id)
{
	uint8_t ret;

	ret = drvMUxGetChannel(id);

	if(ret<8) return ret;
	else			return 255;
}

bool muxScanChannel(uint8_t ch)
{
	uint8_t i;
	uint8_t ret_ch;

	for(i=0;i<MUX_MAX_CH;i++)
	{
		ret_ch = muxGetChannel(i);
		if(ret_ch == ch) return false;
	}
	return true;
}

#ifdef _USE_HW_CMDIF_MUX
void muxCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("mux", muxCmdif);
}

int muxCmdif(int argc, char **argv)
{
  bool ret = true;
  uint8_t ch;
  uint8_t ret_ch;


  if (argc < 3)
  {
    ret = false;
  }
  else
  {
    ch = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);

    if(strcmp("on", argv[1]) == 0)
    {
    	muxSetChannel(0,ch);
    }
    else if(strcmp("get", argv[1]) == 0)
		{
    	ret_ch = muxGetChannel(0);
    	cmdifPrintf( "%d \n", ret_ch);
		}
    else
    {
      ret = false;
    }
  }


  if (ret == false)
  {
    cmdifPrintf( "MUX [cmd] number ...\n");
    cmdifPrintf( "\t ON/GET \n");
  }

  return 0;
}
#endif
#endif
