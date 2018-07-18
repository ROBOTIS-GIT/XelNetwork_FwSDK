/*
 * ctable.c
 *
 *  Created on: 2017. 7. 19.
 *      Author: Baram
 */
#include <stdlib.h>


#include "hw_def.h"

#ifdef _USE_HW_CTABLE
#include "def.h"
#include "hw.h"
#include "ctable.h"








//-- Internal Variables
//
static ctable_t *p_ctable_cmd = NULL;


//-- External Variables
//


//-- Internal Functions
//
#ifdef _USE_HW_CMDIF_CTABLE
void ctableCmdifInit(void);
int ctableCmdif(int argc, char **argv);
#endif


//-- External Functions
//





bool ctableInit(ctable_t *p_ctable, uint8_t *p_mem, uint32_t mem_length, ctable_attribute_t *p_attr)
{
  static bool first_run = false;
  bool ret = true;
  uint16_t i;

  p_ctable->p_mem  = p_mem;
  p_ctable->p_attr = p_attr;
  p_ctable->attr_length = 0;

  i = 0;
  while(1)
  {
    if (p_ctable->p_attr[i].address == 0xFFFF)
    {
      break;
    }
    i++;
  }
  p_ctable->attr_length = i;
  p_ctable->init = true;
  p_ctable->mem_length = mem_length;

  ctableUpdate(p_ctable);

  p_ctable_cmd = p_ctable;

  if (first_run == false)
  {
    first_run = true;
#ifdef _USE_HW_CMDIF_CTABLE
    ctableCmdifInit();
#endif
  }
  return ret;
}

void ctableUpdate(ctable_t *p_ctable)
{
  uint16_t i;
  uint16_t j;
  uint16_t addr;
  //uint8_t  *p_data;
  uint16_t length;
  //uint32_t data;


  if (p_ctable->init != true) return;


  for (i=0; i<p_ctable->attr_length; i++)
  {
    addr   = p_ctable->p_attr[i].address;
    //p_data = (uint8_t *)&data;
    length = p_ctable->p_attr[i].length;


    if (p_ctable->p_attr[i].update & _UPDATE_INIT)
    {
      if ((addr+length) < p_ctable->mem_length)
      {
        if (p_ctable->p_mem != NULL)
        {
          for (j=0; j<p_ctable->p_attr[i].count; j++)
          {
            memcpy(&p_ctable->p_mem[addr + j * length], (uint8_t *)&p_ctable->p_attr[i].init_data, length);
          }
        }
      }
      p_ctable->p_attr[i].update &= ~_UPDATE_INIT;
    }

    if (p_ctable->p_attr[i].update & _UPDATE_RD)
    {
      if ((p_ctable->p_attr[i].attr & _ATTR_VIR) == 0)
      {
        if (p_ctable->p_mem != NULL)
        {
          ctableRead(p_ctable, addr, &p_ctable->p_mem[addr], length);
        }
      }
      p_ctable->p_attr[i].update &= ~_UPDATE_RD;
    }

    if (p_ctable->p_attr[i].update & _UPDATE_WR)
    {
      if ((p_ctable->p_attr[i].attr & _ATTR_VIR) == 0)
      {
        if (p_ctable->p_mem != NULL)
        {
          ctableWrite(p_ctable, addr, &p_ctable->p_mem[addr], length);
        }
      }
      else
      {
        // 가상메모리면 초기값을 Write
        ctableWrite(p_ctable, addr, (uint8_t *)&p_ctable->p_attr[i].init_data, length);
      }
      p_ctable->p_attr[i].update &= ~_UPDATE_WR;
    }
  }
}

uint8_t ctableRead(ctable_t *p_ctable, uint16_t addr, uint8_t *p_data, uint16_t length)
{
  uint8_t ret = DXL_ERR_NONE;
  uint16_t i;
  uint16_t j;
  uint16_t node_addr_start;
  uint16_t node_addr_end;
  uint16_t node_length;
  uint16_t addr_start;
  uint16_t addr_end;
  bool     update;


  if (p_ctable->init != true) return DXL_ERR_NONE;

  addr_start = addr;
  addr_end   = addr + length;

  if (p_ctable->p_mem == NULL)
  {
    memset(&p_data[0], 0x00, length);
  }

  p_ctable->data_type = -1;
  for (i=0; i<p_ctable->attr_length; i++)
  {
    node_addr_start = p_ctable->p_attr[i].address;
    node_addr_end   = node_addr_start + (p_ctable->p_attr[i].length * p_ctable->p_attr[i].count);

    update = false;

    if (node_addr_start >= addr_start && node_addr_end <= addr_end)       // 테이블의 주소 범위가 읽기 주소 범위에 포함되는 경우
    {
      update = true;
    }
    if (addr_start >= node_addr_start && addr_start < node_addr_end)      // 읽기 주소의 시작 주소가 테이블 주소의 범위에 있는 경우
    {
      update = true;
      node_addr_start = addr_start;
    }
    if (addr_end > node_addr_start && addr_end <= node_addr_end)          // 읽기 주소의 마지막 주소가 테이블 주소의 범위에 있는 경우
    {
      update = true;
      node_addr_end = addr_end;
    }

    node_length = node_addr_end - node_addr_start;

    if (update == true)
    {
      p_ctable->data_type = p_ctable->p_attr[i].module;

      if (p_ctable->p_attr[i].attr & _ATTR_EEPROM)
      {
        for (j=node_addr_start; j<node_addr_end; j++)
        {
          if (p_ctable->p_mem != NULL)
          {
            #ifdef _USE_HW_EEPROM
            p_ctable->p_mem[j] = eepromReadByte(j);
            //cmdifPrintf("rd eep addr : %d %d %d %d\n", p_ctable->p_attr[i].address, j, p_ctable->p_mem[node_addr_start], eepromReadByte(j) );
            #endif
          }
        }
      }

      if ((p_ctable->p_attr[i].attr & _ATTR_VIR) > 0)
      {
        memset(&p_data[node_addr_start-addr_start], 0x00, node_length);
      }

      if (p_ctable->p_attr[i].update_func != NULL)
      {
        (*p_ctable->p_attr[i].update_func)((void *)p_ctable, i, _UPDATE_RD, node_addr_start - p_ctable->p_attr[i].address, &p_data[node_addr_start-addr_start], node_length);
        //cmdifPrintf("rd addr : %d %d %d %d\n", p_ctable->p_attr[i].address, node_addr_start-addr_start, node_addr_start - p_ctable->p_attr[i].address, node_length );
      }

      if ((p_ctable->p_attr[i].attr & _ATTR_VIR) == 0)
      {
        if (node_addr_end < p_ctable->mem_length)
        {
          if (p_ctable->p_mem != NULL)
          {
            memcpy(&p_data[node_addr_start-addr_start], &p_ctable->p_mem[node_addr_start], node_length);
          }
        }
      }
    }
  }
  return ret;
}

uint8_t ctableWrite(ctable_t *p_ctable, uint16_t addr, uint8_t *p_data, uint16_t length)
{
  uint8_t ret = DXL_ERR_NONE;
  uint16_t i;
  uint16_t j;
  uint16_t node_addr_start;
  uint16_t node_addr_end;
  uint16_t node_length;
  uint16_t addr_start;
  uint16_t addr_end;
  bool     update;


  if (p_ctable->init != true) return DXL_ERR_NONE;

  addr_start = addr;
  addr_end   = addr + length;

  for (i=0; i<p_ctable->attr_length; i++)
  {
    node_addr_start = p_ctable->p_attr[i].address;
    node_addr_end   = node_addr_start + (p_ctable->p_attr[i].length * p_ctable->p_attr[i].count);

    update = false;

    if (node_addr_start >= addr_start && node_addr_end <= addr_end)
    {
      update = true;
    }
    if (addr_start >= node_addr_start && addr_start < node_addr_end)
    {
      update = true;
      node_addr_start = addr_start;
    }
    if (addr_end > node_addr_start && addr_end <= node_addr_end)
    {
      update = true;
      node_addr_end = addr_end;
    }

    node_length = node_addr_end - node_addr_start;

    // 쓰기의 경우는 시작 주소와 사이즈가 같아야 한다.
    if (node_addr_start != p_ctable->p_attr[i].address || p_ctable->p_attr[i].length != node_length)
    {
      if (p_ctable->p_attr[i].count == 1)
      {
        update = false;
      }
    }

    if (update == true)
    {
      if (p_ctable->p_attr[i].attr & _ATTR_WR)
      {
        if ((p_ctable->p_attr[i].attr & _ATTR_VIR) == 0)
        {
          if (p_ctable->p_mem != NULL)
          {
            memcpy(&p_ctable->p_mem[node_addr_start], &p_data[node_addr_start-addr_start], node_length);
          }
        }

        if (p_ctable->p_attr[i].update_func != NULL)
        {
          (*p_ctable->p_attr[i].update_func)((void *)p_ctable, i, _UPDATE_WR, node_addr_start - p_ctable->p_attr[i].address, &p_data[node_addr_start-addr_start], node_length);
          //cmdifPrintf("wr addr : %d %d %d %d %d\n", p_ctable->p_attr[i].address, node_addr_start-addr_start, node_addr_start - p_ctable->p_attr[i].address, node_length );
        }

        if (p_ctable->p_attr[i].attr & _ATTR_EEPROM)
        {
          for (j=node_addr_start; j<node_addr_end; j++)
          {
            if (p_ctable->p_mem != NULL)
            {
              #ifdef _USE_HW_EEPROM
              eepromWriteByte(j, p_ctable->p_mem[j]);

              //cmdifPrintf("wr eep addr : %d %d %d %d\n", p_ctable->p_attr[i].address, j, p_ctable->p_mem[j], node_length );
              #endif
            }
          }
        }
      }
      else
      {
        ret = DXL_ERR_ACCESS;
      }
    }
  }

  return ret;
}

void ctableRemoveReset(ctable_t *p_ctable, uint16_t addr)
{
  uint32_t i;

  for(i=0; i<p_ctable_cmd->attr_length; i++)
  {
    if ((p_ctable_cmd->p_attr[i].attr & _ATTR_WR))
    {
      if (p_ctable_cmd->p_attr[i].address == addr)
      {
        p_ctable_cmd->p_attr[i].update |= _UPDATE_NOT_RESET;
        break;
      }
    }
  }
}

void ctableReset(ctable_t *p_ctable)
{
  uint32_t i;

  for(i=0; i<p_ctable_cmd->attr_length; i++)
  {
    if ((p_ctable_cmd->p_attr[i].attr & _ATTR_WR) && (p_ctable_cmd->p_attr[i].update & _UPDATE_NOT_RESET) == 0)
    {
      p_ctable_cmd->p_attr[i].update &= ~_UPDATE_NOT_RESET;

      if (p_ctable_cmd->p_attr[i].count == 1)
      {
        ctableWrite(p_ctable_cmd, p_ctable_cmd->p_attr[i].address, (uint8_t *)&p_ctable_cmd->p_attr[i].init_data, p_ctable_cmd->p_attr[i].length);
      }
      else
      {
        if (p_ctable->p_attr[i].update_func != NULL)
        {
          (*p_ctable->p_attr[i].update_func)((void *)p_ctable, i, _UPDATE_RESET, p_ctable->p_attr[i].address, NULL, 0);
        }
      }
    }
  }
}

#ifdef _USE_HW_CMDIF_CTABLE

void ctableCmdifInit(void)
{
  if (cmdifIsInit() == false)
  {
    cmdifInit();
  }
  cmdifAdd("ctable", ctableCmdif);
}

int ctableCmdif(int argc, char **argv)
{
  bool ret = true;
  uint32_t i;


  if (p_ctable_cmd == NULL)
  {
    return 0;
  }


  if (argc == 2 && strcmp("list", argv[1]) == 0)
  {
    for(i=0; i<p_ctable_cmd->attr_length; i++)
    {
      cmdifPrintf("%03d addr:%04d len:%03d cnt:%03d ",
                  i,
                  p_ctable_cmd->p_attr[i].address,
                  p_ctable_cmd->p_attr[i].length,
                  p_ctable_cmd->p_attr[i].count
                  );
      if (p_ctable_cmd->p_attr[i].attr & _ATTR_VIR)
      {
        cmdifPrintf("VIR ");
      }
      if (p_ctable_cmd->p_attr[i].attr & _ATTR_EEPROM)
      {
        cmdifPrintf("EEP ");
      }
      if (p_ctable_cmd->p_attr[i].attr & _ATTR_CONST)
      {
        cmdifPrintf("CON ");
      }

      if ((p_ctable_cmd->p_attr[i].attr & _ATTR_RD) && (p_ctable_cmd->p_attr[i].attr & _ATTR_WR))
      {
        cmdifPrintf("RW ");
      }
      else if (p_ctable_cmd->p_attr[i].attr & _ATTR_RD)
      {
        cmdifPrintf("RD ");
      }
      else if (p_ctable_cmd->p_attr[i].attr & _ATTR_WR)
      {
        cmdifPrintf("WR ");
      }
      else
      {
        cmdifPrintf("   ");
      }

      cmdifPrintf("Init:%d\t ", p_ctable_cmd->p_attr[i].init_data);

      if (p_ctable_cmd->p_attr[i].length <= 4 && p_ctable_cmd->p_attr[i].count == 1)
      {
        data_t data;

        data.u32Data = 0;
        ctableRead(p_ctable_cmd, p_ctable_cmd->p_attr[i].address, data.u8Data, p_ctable_cmd->p_attr[i].length);

        cmdifPrintf("Data: 0x%08X\t ", data.u32Data);
        cmdifPrintf("%d\t ", data.u32Data);
        cmdifPrintf("%d\t ", data.s32Data);
      }
      cmdifPrintf("\n");
    }
  }
  else if (argc == 2 && strcmp("reset", argv[1]) == 0)
  {
    for(i=0; i<p_ctable_cmd->attr_length; i++)
    {
      if (p_ctable_cmd->p_attr[i].attr & _ATTR_WR)
      {
        if (p_ctable_cmd->p_attr[i].count == 1)
        {
          ctableWrite(p_ctable_cmd, p_ctable_cmd->p_attr[i].address, (uint8_t *)&p_ctable_cmd->p_attr[i].init_data, p_ctable_cmd->p_attr[i].length);
          cmdifPrintf("%03d addr:%04d len:%03d cnt:%03d \n",
                      i,
                      p_ctable_cmd->p_attr[i].address,
                      p_ctable_cmd->p_attr[i].length,
                      p_ctable_cmd->p_attr[i].count
                      );
        }
      }
    }
    cmdifPrintf("Reset Done\n");
  }
  else if (argc == 4 && strcmp("read", argv[1]) == 0)
  {
    uint16_t index;
    uint16_t cnt_index;

    index     = (uint16_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    cnt_index = (uint16_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);

    if (index >= p_ctable_cmd->attr_length)
    {
      cmdifPrintf("out of range\n");
      return 0;
    }

    while(cmdifRxAvailable() == 0)
    {
      data_t data;

      data.u32Data = 0;
      ctableRead(p_ctable_cmd, p_ctable_cmd->p_attr[index].address + p_ctable_cmd->p_attr[index].length * cnt_index, data.u8Data, p_ctable_cmd->p_attr[index].length);

      cmdifPrintf("0x%08X\t ", data.u32Data);
      cmdifPrintf("%d\t ", data.u32Data);
      cmdifPrintf("%d\n", data.s32Data);

      delay(100);
    }
  }
  else if (argc == 5 && strcmp("write", argv[1]) == 0)
  {
    uint16_t index;
    uint16_t cnt_index;
    data_t data;

    index        = (uint16_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    cnt_index    = (uint16_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);
    data.s32Data = (int32_t)  strtoul((const char * ) argv[4], (char **)NULL, (int) 0);

    if (index >= p_ctable_cmd->attr_length)
    {
      cmdifPrintf("out of range\n");
      return 0;
    }

    cmdifPrintf("Write : addr %d, data %d\n", p_ctable_cmd->p_attr[index].address, data.s32Data);
    ctableWrite(p_ctable_cmd, p_ctable_cmd->p_attr[index].address + p_ctable_cmd->p_attr[index].length * cnt_index, data.u8Data, p_ctable_cmd->p_attr[index].length);

    cmdifPrintf("Read :\n");
    data.u32Data = 0;
    ctableRead(p_ctable_cmd, p_ctable_cmd->p_attr[index].address + p_ctable_cmd->p_attr[index].length * cnt_index, data.u8Data, p_ctable_cmd->p_attr[index].length);

    cmdifPrintf("0x%08X\t ", data.u32Data);
    cmdifPrintf("%d\t ", data.u32Data);
    cmdifPrintf("%d\n ", data.s32Data);
  }
  else
  {
    ret = false;
  }

  if (ret == false)
  {
    cmdifPrintf( "ctable list\n");
    cmdifPrintf( "ctable reset\n");
    cmdifPrintf( "ctable read  index cnt_index\n");
    cmdifPrintf( "ctable write index cnt_index data\n");
  }

  return 0;
}
#endif

#endif


