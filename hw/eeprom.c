/*
 *  eeprom.c
 *
 *  Created on: 2017. 3. 16.
 *      Author: Baram
 */

#include "eeprom.h"

#ifdef _USE_HW_EEPROM

#include "hw.h"
#include "driver/drv_eeprom.h"



//-- Internal Variables
//


//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_EEPROM
void eepromCmdifInit(void);
int  eepromCmdif(int argc, char **argv);
#endif


//-- External Functions
//





bool eepromInit()
{
  bool ret;


  ret = drvEepromInit();


#ifdef _USE_HW_CMDIF_EEPROM
  eepromCmdifInit();
#endif

  return ret;
}



uint8_t eepromReadByte(uint32_t addr)
{
  return drvEepromReadByte(addr);
}

bool eepromWriteByte(uint32_t index, uint8_t data_in)
{
  return drvEepromWriteByte(index, data_in);
}

uint32_t eepromGetLength(void)
{
  return drvEepromGetLength();
}

bool eepromFormat(void)
{
  return drvEepromFormat();
}




#ifdef _USE_HW_CMDIF_EEPROM
void eepromCmdifInit(void)
{
  cmdifAdd("eeprom", eepromCmdif);
}

int eepromCmdif(int argc, char **argv)
{
  bool ret = true;
  uint32_t i;
  uint32_t addr;
  uint32_t length;
  uint8_t  data;
  uint32_t pre_time;
  bool eep_ret;


  if (argc == 2)
  {
    if(strcmp("info", argv[1]) == 0)
    {
      cmdifPrintf("eeprom init   : %d\n", eepromInit());
      cmdifPrintf("eeprom length : %d bytes\n", eepromGetLength());
    }
    else if(strcmp("format", argv[1]) == 0)
    {
      if (eepromFormat() == true)
      {
        cmdifPrintf("format OK\n");
      }
      else
      {
        cmdifPrintf("format Fail\n");
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (argc == 4)
  {
    if(strcmp("read", argv[1]) == 0)
    {
      addr   = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
      length = (uint32_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);

      if (length > eepromGetLength())
      {
        cmdifPrintf( "length error\n");
        return 0;
      }
      for (i=0; i<length; i++)
      {
        data = eepromReadByte(addr+i);
        cmdifPrintf( "addr : %d\t 0x%02X\n", addr+i, data);
      }
    }
    else if(strcmp("write", argv[1]) == 0)
    {
      addr = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
      data = (uint8_t)  strtoul((const char * ) argv[3], (char **)NULL, (int) 0);

      pre_time = micros();
      eep_ret = eepromWriteByte(addr, data);

      cmdifPrintf( "addr : %d\t 0x%02X %dus\n", addr, data, micros()-pre_time);
      if (eep_ret)
      {
        cmdifPrintf("OK\n");
      }
      else
      {
        cmdifPrintf("FAIL\n");
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
    cmdifPrintf( "eeprom info\n");
    cmdifPrintf( "eeprom format\n");
    cmdifPrintf( "eeprom read  [addr] [length]\n");
    cmdifPrintf( "eeprom write [addr] [data]\n");
  }

  return 0;
}
#endif /* _USE_HW_CMDIF_EEPROM */


#endif /* _USE_HW_EEPROM */
