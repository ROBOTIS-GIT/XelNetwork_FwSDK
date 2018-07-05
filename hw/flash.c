/*
 *  flash.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */


#include "flash.h"

#ifdef _USE_HW_FLASH

#include "driver/drv_flash.h"
#include "hw.h"





//-- Internal Variables
//
#ifdef _USE_HW_CMDIF_FLASH
void flashCmdifInit(void);
int  flashCmdif(int argc, char **argv);
#endif

//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//











bool flashInit(void)
{


  drvFlashInit();


#ifdef _USE_HW_CMDIF_FLASH
  flashCmdifInit();
#endif


  return true;
}

err_code_t flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  err_code_t err_code = OK;


  err_code = drvFlashWrite(addr, p_data, length);

  return err_code;
}


err_code_t flashRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  err_code_t err_code = OK;


  err_code = drvFlashRead(addr, p_data, length);

  return err_code;
}

err_code_t flashErase(uint32_t addr, uint32_t length)
{
  err_code_t err_code = OK;


  err_code = drvFlashErase(addr, length);

  return err_code;
}





#ifdef _USE_HW_CMDIF_FLASH
void flashCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("flash", flashCmdif);
}

int flashCmdif(int argc, char **argv)
{
  bool ret = true;
  uint32_t addr;
  uint32_t length;
  err_code_t err_code;
  uint32_t time_pre;
  uint32_t time_process;
  uint32_t data;


  if (argc == 4)
  {
    addr   = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    length = (uint32_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);


    if(strcmp("erase", argv[1]) == 0)
    {
      time_pre = micros();
      err_code = flashErase(addr, length);
      time_process = micros()-time_pre;
      cmdifPrintf("flashErase : %d, %dms\n", err_code, time_process/1000);
    }
    else if(strcmp("write", argv[1]) == 0)
    {
      if (addr%4 != 0)
      {
        cmdifPrintf("addr not aligned 4bytes \n");
      }
      else
      {
        data = length;
        time_pre = micros();
        err_code = flashWrite(addr, (uint8_t *)&data, 4);
        time_process = micros()-time_pre;
        cmdifPrintf("flashWrite : %d, %dus\n", err_code, time_process);
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
    cmdifPrintf( "flash erase addr length \n");
    cmdifPrintf( "flash write addr data   \n");
  }

  return 0;
}

#endif /* _USE_HW_CMDIF_FLASH */

#endif /* _USE_HW_FLASH */
