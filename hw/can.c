/*
 * can.c
 *
 *  Created on: 2017. 12. 5.
 *      Author: opus
 */

#include "can.h"

#ifdef _USE_HW_CAN

#include "hw.h"


#ifdef _USE_HW_CMDIF_CAN
void canCmdifInit(void);
int  canCmdif(int argc, char **argv);
#endif



void canInit(void)
{
  drvCanInit();

#ifdef _USE_HW_CMDIF_CAN
  canCmdifInit();
#endif
}


bool canOpen(uint8_t channel, uint32_t baudrate, uint8_t format)
{
  return drvCanOpen(channel, baudrate, format);
}

void canClose(uint8_t channel)
{
  drvCanClose(channel);
}

bool canConfigFilter(uint8_t filter_num, uint32_t id, uint32_t mask)
{
  return drvCanConfigFilter(filter_num, id, mask);
}

uint32_t canWrite(uint8_t channel, uint32_t id, uint8_t *p_data, uint32_t length)
{
  return drvCanWrite(channel, id, p_data, length);
}

uint8_t canRead(uint8_t channel)
{
  return drvCanRead(channel);
}

uint32_t canAvailable(uint8_t channel)
{
  return drvCanAvailable(channel);
}

uint32_t canWriteMsg(uint8_t channel, can_msg_t *p_msg)
{
  drv_can_msg_t msg;
  msg.id = p_msg->id;
  msg.length = p_msg->length;
  memcpy(msg.data, p_msg->data, 8);

  return drvCanWriteMsg(channel, &msg);
}

bool canReadMsg(uint8_t channel, can_msg_t *p_msg)
{
  bool ret = false;
  drv_can_msg_t *rx_msg;

  rx_msg = drvCanReadMsg(channel);

  if(rx_msg != NULL)
  {
    p_msg->id = rx_msg->id;
    p_msg->length = rx_msg->length;

    memcpy(p_msg->data, rx_msg->data, p_msg->length);

    ret = true;
  }

  return ret;
}

uint32_t canAvailableMsg(uint8_t channel)
{
  return drvCanAvailableMsg(channel);
}

uint8_t canGetErrCount(uint8_t channel)
{
  return drvCanGetErrCount(channel);
}

uint32_t canGetError(uint8_t channel)
{
  return drvCanGetError(channel);
}

uint32_t canGetState(uint8_t channel)
{
  return drvCanGetState(channel);
}

void canAttachRxInterrupt(uint8_t channel, void (*handler)(can_msg_t *arg))
{
  drvCanAttachRxInterrupt(channel, (void(*)(void *arg)) handler);
}

void canDetachRxInterrupt(uint8_t channel)
{
  drvCanDetachRxInterrupt(channel);
}


#ifdef _USE_HW_CMDIF_CAN
void canRxHandlerTemplate(can_msg_t *arg)
{
  cmdifPrintf("ID : 0x%X, Length : %d, Data : ", arg->id, arg->length);

  for(uint32_t i = 0; i < arg->length; i++)
  {
    cmdifPrintf("%02X ", arg->data[i]);
  }
  cmdifPrintf("\r\n");
}

void canCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("can", canCmdif);
}

int canCmdif(int argc, char **argv)
{
  bool ret = true;

  if((argc > 5)&&(!strcmp(argv[1], "write")))
  {
    uint8_t channel = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0) - 1;
    uint32_t id     = (uint32_t) strtoul((const char * ) argv[3], (char **)NULL, 16);
    uint32_t opt    = (uint32_t) strtoul((const char * ) argv[4], (char **)NULL, (int) 0);
    uint32_t tx_len = argc - 5;
    uint8_t tx_data[tx_len];
    uint32_t i, t_time;

    for(i = 0; i < tx_len; i++)
    {
      tx_data[i] = (uint8_t) strtoul((const char * ) argv[5 + i], (char **)NULL, 16);
    }

    cmdifPrintf("Id : 0x%X, Write bytes : %d \r\n", id, tx_len);

    switch(opt)
    {
      case 1 :
        t_time = millis();
        while(cmdifRxAvailable() == 0)
        {
          if((millis() - t_time) >= 500)
          {
            t_time = millis();
            canWrite(channel, id, tx_data, tx_len);
          }
        }
        break;

      case 0 :
      default :
        if((tx_len = canWrite(channel, id, tx_data, tx_len)) == 0)
        {
          ret = false;
        }
        break;
    }
  }
  else if((argc == 4)&&(!strcmp(argv[1], "read")))
  {
    uint8_t channel = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0) - 1;
    uint8_t opt     = (uint8_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);

    if(opt == 2)
    {
      canAttachRxInterrupt(channel, canRxHandlerTemplate);
      while(cmdifRxAvailable() == 0);
      canDetachRxInterrupt(channel);
    }
    else if(opt == 0)
    {
      uint32_t rx_len;
      uint32_t i;

      while(cmdifRxAvailable() == 0)
      {
        if((rx_len = canAvailable(channel)) > 0)
        {
          for(i = 0; i < rx_len; i++)
          {
            cmdifPrintf("%02X ", canRead(channel));
          }
          cmdifPrintf("\r\n");
        }
      }
    }
    else
    {
      can_msg_t can_msg;

      while(cmdifRxAvailable() == 0)
      {
        if(canAvailableMsg(channel) > 0)
        {
          canReadMsg(channel, (void *)&can_msg);
          canRxHandlerTemplate(&can_msg);
        }
      }
    }
  }
  else if((argc == 5)&&(!strcmp(argv[1], "open")))
  {
    uint8_t channel   = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0) - 1;
    uint32_t baudrate = strtoul((const char * ) argv[3], (char **)NULL, (int) 0);
    uint32_t format   = strtoul((const char * ) argv[4], (char **)NULL, (int) 0);

    if(canOpen(channel, baudrate, format) == false)
    {
      ret = false;
    }
  }
  else if((argc == 5)&&(!strcmp(argv[1], "filter")))
  {
    uint32_t num  = strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    uint32_t id   = strtoul((const char * ) argv[3], (char **)NULL, 16);
    uint32_t mask = strtoul((const char * ) argv[4], (char **)NULL, 16);

    canConfigFilter(num, id, mask);
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf( "can write  channel[1~2] id[hex] opt[0:once|1:loop] data(1byte) data .. .. ...\n");
    cmdifPrintf( "can read   channel[1~2] opt[0:byteData|1:packet|2:isr] ...\n");
    cmdifPrintf( "can open   channel[1~2] baudrate[0~3] format[0:STD,1:EXT] ...\n");
    cmdifPrintf( "                    0 : 125K, 1 : 250K, 2 : 500K, 3 : 1M \n");
    cmdifPrintf( "can filter number[0~27] id[hex] mask[hex] ...\n");
  }

  return 0;
}

#endif /* _USE_HW_CMDIF_PWM */



#endif /* _USE_HW_CAN */
