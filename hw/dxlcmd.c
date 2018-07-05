/*
 * dxlcmd.c
 *
 *  Created on: 2017. 4. 11.
 *      Author: Baram
 */



#include "dxlcmd.h"

#ifdef _USE_HW_DXLCMD

#include <stdlib.h>
#include "error_code.h"
#include "hw.h"




//-- Internal Variables
//



//-- External Variables
//


//-- Internal Functions
//
void dxlcmdCmdifInit(void);
int  dxlcmdCmdif(int argc, char **argv);




//-- External Functions
//




bool dxlcmdInit(void)
{
  bool ret = true;


#ifdef _USE_HW_CMDIF_DXLCMD
  dxlcmdCmdifInit();
#endif

  return ret;
}

dxl_error_t dxlcmdReset(dxl_t *p_packet, uint8_t id, uint8_t option, dxlcmd_resp_reset_t *p_resp, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;
  uint8_t tx_param[1];


  //if (id == DXL_GLOBAL_ID)
  //{
  //  return DXL_RET_ERROR_NOT_GLOBALID;
  //}

  if (dxlIsOpen(p_packet) == true)
  {
    tx_param[0] = option;

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_RESET, tx_param, 1);
    p_packet->tx_time = micros() - pre_time_us;

    if (id == DXL_GLOBAL_ID)
    {
      return dxl_ret;
    }

    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;

        p_resp->id    = p_packet->rx.id;
        p_resp->error = p_packet->rx.error;

        dxl_ret = DXL_RET_RX_RESP;
        break;
      }
      else if (dxl_ret != DXL_RET_EMPTY)
      {
        break;
      }


      if (millis()-pre_time_ms >= timeout)
      {
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdReboot(dxl_t *p_packet, uint8_t id, dxlcmd_resp_reboot_t *p_resp, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;


  //if (id == DXL_GLOBAL_ID)
  //{
  //  return DXL_RET_ERROR_NOT_GLOBALID;
  //}

  if (dxlIsOpen(p_packet) == true)
  {
    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_REBOOT, NULL, 0);
    p_packet->tx_time = micros() - pre_time_us;


    if (id == DXL_GLOBAL_ID)
    {
      return dxl_ret;
    }

    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;

        p_resp->id    = p_packet->rx.id;
        p_resp->error = p_packet->rx.error;

        dxl_ret = DXL_RET_RX_RESP;
        break;
      }
      else if (dxl_ret != DXL_RET_EMPTY)
      {
        break;
      }


      if (millis()-pre_time_ms >= timeout)
      {
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdPing(dxl_t *p_packet, uint8_t id, dxlcmd_resp_ping_t *p_resp, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_ms;
  uint32_t pre_time_us;
  uint32_t mem_addr;
  uint8_t  *p_mem = (uint8_t *)p_resp->mem;


  p_resp->id_count = 0;
  p_resp->p_node[0] = (dxlcmd_ping_node_t *)&p_mem[0];


  if (dxlIsOpen(p_packet) == true)
  {
    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_PING, NULL, 0);
    p_packet->tx_time = micros() - pre_time_us;


    mem_addr = 0;
    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        p_packet->rx_time = micros() - pre_time_us;
        pre_time_ms       = millis();

        p_resp->p_node[p_resp->id_count]->id = p_packet->rx.id;
        p_resp->p_node[p_resp->id_count]->model_number     = p_packet->rx.p_param[0]<<0;
        p_resp->p_node[p_resp->id_count]->model_number    |= p_packet->rx.p_param[1]<<8;
        p_resp->p_node[p_resp->id_count]->firmware_version = p_packet->rx.p_param[2];

        p_resp->id_count++;

        //-- 주소를 4바이트로 정렬( 구조체를 직접 타입변환하여 사용하기 위해서 )
        //
        mem_addr += sizeof(dxlcmd_ping_node_t);
        if (mem_addr%4)
        {
          mem_addr += 4 - (mem_addr%4);
        }

        p_resp->p_node[p_resp->id_count] = (dxlcmd_ping_node_t *)&p_mem[mem_addr];

        if (id != DXL_GLOBAL_ID)
        {
          dxl_ret = DXL_RET_RX_RESP;
          break;
        }
      }

      if (millis()-pre_time_ms >= timeout)
      {
        if (p_resp->id_count > 0)
        {
          dxl_ret = DXL_RET_RX_RESP;
        }
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}


dxl_error_t dxlcmdRead(dxl_t *p_packet, uint8_t id, uint16_t addr, uint16_t length, dxlcmd_resp_read_t *p_resp, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;

  uint8_t tx_param[4];
  uint32_t mem_addr;
  uint8_t  *p_mem = (uint8_t *)p_resp->mem;
  uint32_t i;


  p_resp->id_count = 0;
  p_resp->p_node[0] = (dxlcmd_read_node_t *)&p_mem[0];


  if (id == DXL_GLOBAL_ID)
  {
    return DXL_RET_ERROR_NOT_GLOBALID;
  }

  if (dxlIsOpen(p_packet) == true)
  {
    if (p_packet->packet_ver == DXL_PACKET_VER_1_0 )
    {
      tx_param[0] = addr;
      tx_param[1] = length;
    }
    else
    {
      tx_param[0] = addr >> 0;
      tx_param[1] = addr >> 8;
      tx_param[2] = length >> 0;
      tx_param[3] = length >> 8;
    }

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_READ, tx_param, 4);
    p_packet->tx_time = micros() - pre_time_us;

    mem_addr = 0;
    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;

        mem_addr += sizeof(dxlcmd_read_node_t);
        p_resp->p_node[p_resp->id_count]->p_data = &p_mem[mem_addr];

        p_resp->p_node[p_resp->id_count]->id     = p_packet->rx.id;
        p_resp->p_node[p_resp->id_count]->error  = p_packet->rx.error;
        p_resp->p_node[p_resp->id_count]->length = p_packet->rx.param_length;

        if ((mem_addr+p_packet->rx.param_length) > DXLCMD_MAX_BUFFER)
        {
          break;
        }

        for (i=0; i<p_packet->rx.param_length; i++)
        {
          p_resp->p_node[p_resp->id_count]->p_data[i] = p_packet->rx.p_param[i];
        }

        p_resp->id_count++;
        dxl_ret = DXL_RET_RX_RESP;
        break;
      }
      else if (dxl_ret != DXL_RET_EMPTY)
      {
        break;
      }
      else
      {
#ifdef _USE_HW_RTOS
        osThreadYield();
#endif
      }


      if (millis()-pre_time_ms >= timeout)
      {
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdWrite(dxl_t *p_packet, uint8_t id, uint8_t *p_data, uint16_t addr, uint16_t length, dxlcmd_resp_write_t *p_resp, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;
  uint8_t  tx_param[2 + DXLCMD_MAX_NODE * DXLCMD_MAX_NODE_BUFFER_SIZE];
  uint16_t tx_length;
  uint32_t i;

  /*
  if (id == DXL_GLOBAL_ID)
  {
    return DXL_RET_ERROR_NOT_GLOBALID;
  }
  */

  if (dxlIsOpen(p_packet) == true)
  {
    if (length > DXLCMD_MAX_NODE * DXLCMD_MAX_NODE_BUFFER_SIZE)
    {
      return DXL_RET_ERROR_LENGTH;
    }

    if (p_packet->packet_ver == DXL_PACKET_VER_1_0 )
    {
      tx_param[0] = addr;

      for (i=0; i<length; i++)
      {
        tx_param[1 + i] = p_data[i];
      }

      tx_length = 1 + length;
    }
    else
    {
      tx_param[0] = addr >> 0;
      tx_param[1] = addr >> 8;

      for (i=0; i<length; i++)
      {
        tx_param[2 + i] = p_data[i];
      }

      tx_length = 2 + length;
    }

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_WRITE, tx_param, tx_length);
    p_packet->tx_time = micros() - pre_time_us;


    if (id == DXL_GLOBAL_ID)
    {
      return dxl_ret;
    }

    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;

        p_resp->id    = p_packet->rx.id;
        p_resp->error = p_packet->rx.error;

        dxl_ret = DXL_RET_RX_RESP;
        break;
      }
      else if (dxl_ret != DXL_RET_EMPTY)
      {
        break;
      }


      if (millis()-pre_time_ms >= timeout)
      {
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdWriteNoResp(dxl_t *p_packet, uint8_t id, uint8_t *p_data, uint16_t addr, uint16_t length)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint8_t  tx_param[2 + DXLCMD_MAX_NODE * DXLCMD_MAX_NODE_BUFFER_SIZE];
  uint16_t tx_length;
  uint32_t i;


  if (dxlIsOpen(p_packet) == true)
  {
    if (length > DXLCMD_MAX_NODE * DXLCMD_MAX_NODE_BUFFER_SIZE)
    {
      return DXL_RET_ERROR_LENGTH;
    }

    if (p_packet->packet_ver == DXL_PACKET_VER_1_0 )
    {
      tx_param[0] = addr;

      for (i=0; i<length; i++)
      {
        tx_param[1 + i] = p_data[i];
      }

      tx_length = 1 + length;
    }
    else
    {
      tx_param[0] = addr >> 0;
      tx_param[1] = addr >> 8;

      for (i=0; i<length; i++)
      {
        tx_param[2 + i] = p_data[i];
      }

      tx_length = 2 + length;
    }

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_WRITE, tx_param, tx_length);
    p_packet->tx_time = micros() - pre_time_us;
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdSyncRead(dxl_t *p_packet, dxlcmd_param_sync_read_t *p_param, dxlcmd_resp_read_t *p_resp, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;

  uint8_t tx_param[4 + DXLCMD_MAX_NODE];
  uint32_t mem_addr;
  uint8_t  *p_mem = (uint8_t *)p_resp->mem;
  uint32_t i;


  p_resp->id_count = 0;
  p_resp->p_node[0] = (dxlcmd_read_node_t *)&p_mem[0];


  if (dxlIsOpen(p_packet) == true)
  {
    tx_param[0] = p_param->addr >> 0;
    tx_param[1] = p_param->addr >> 8;
    tx_param[2] = p_param->length >> 0;
    tx_param[3] = p_param->length >> 8;

    for( i=0; i<p_param->id_count; i++)
    {
      tx_param[4+i] = p_param->id_tbl[i];
    }

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, DXL_GLOBAL_ID, INST_SYNC_READ, tx_param, 4 + p_param->id_count);
    p_packet->tx_time = micros() - pre_time_us;

    mem_addr = 0;
    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;

        //-- 주소를 4바이트로 정렬( 구조체를 직접 타입변환하여 사용하기 위해서 )
        //
        mem_addr += sizeof(dxlcmd_read_node_t);
        if (mem_addr%4)
        {
          mem_addr += 4 - (mem_addr%4);
        }
        p_resp->p_node[p_resp->id_count]->p_data = &p_mem[mem_addr];

        p_resp->p_node[p_resp->id_count]->id     = p_packet->rx.id;
        p_resp->p_node[p_resp->id_count]->error  = p_packet->rx.error;
        p_resp->p_node[p_resp->id_count]->length = p_packet->rx.param_length;

        for (i=0; i<p_packet->rx.param_length; i++)
        {
          p_resp->p_node[p_resp->id_count]->p_data[i] = p_packet->rx.p_param[i];
        }

        p_resp->id_count++;

        mem_addr += p_packet->rx.param_length;
        p_resp->p_node[p_resp->id_count] = (dxlcmd_read_node_t *)&p_mem[mem_addr];

        if (p_resp->id_count >= p_param->id_count)
        {
          dxl_ret = DXL_RET_RX_RESP;
          break;
        }
      }
      else
      {
#ifdef _USE_HW_RTOS
        osThreadYield();
#endif
      }

      if (millis()-pre_time_ms >= timeout)
      {
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdBulkRead(dxl_t *p_packet, dxlcmd_param_bulk_read_t *p_param, dxlcmd_resp_read_t *p_resp, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;

  uint8_t tx_param[DXLCMD_MAX_NODE * 5];
  uint32_t mem_addr;
  uint8_t *p_mem = (uint8_t *)p_resp->mem;
  uint32_t i;
  uint16_t tx_length;

  p_resp->id_count = 0;
  p_resp->p_node[0] = (dxlcmd_read_node_t *)&p_mem[0];


  if (dxlIsOpen(p_packet) == true)
  {
    tx_length = 0;
    for( i=0; i<p_param->id_count; i++)
    {
      tx_param[tx_length+0] = p_param->id_tbl[i];
      tx_param[tx_length+1] = p_param->addr[i] >> 0;
      tx_param[tx_length+2] = p_param->addr[i] >> 8;
      tx_param[tx_length+3] = p_param->length[i] >> 0;
      tx_param[tx_length+4] = p_param->length[i] >> 8;
      tx_length += 5;
    }

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, DXL_GLOBAL_ID, INST_BULK_READ, tx_param, tx_length);
    p_packet->tx_time = micros() - pre_time_us;

    mem_addr = 0;
    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;

        mem_addr += sizeof(dxlcmd_read_node_t);
        p_resp->p_node[p_resp->id_count]->p_data = &p_mem[mem_addr];

        p_resp->p_node[p_resp->id_count]->id     = p_packet->rx.id;
        p_resp->p_node[p_resp->id_count]->error  = p_packet->rx.error;
        p_resp->p_node[p_resp->id_count]->length = p_packet->rx.param_length;

        for (i=0; i<p_packet->rx.param_length; i++)
        {
          p_resp->p_node[p_resp->id_count]->p_data[i] = p_packet->rx.p_param[i];
        }

        p_resp->id_count++;

        mem_addr += p_packet->rx.param_length;

        //-- 주소를 4바이트로 정렬( 구조체를 직접 타입변환하여 사용하기 위해서 )
        //
        mem_addr += sizeof(dxlcmd_read_node_t);
        if (mem_addr%4)
        {
          mem_addr += 4 - (mem_addr%4);
        }

        p_resp->p_node[p_resp->id_count] = (dxlcmd_read_node_t *)&p_mem[mem_addr];

        if (p_resp->id_count >= p_param->id_count)
        {
          dxl_ret = DXL_RET_RX_RESP;
          break;
        }
      }
      else
      {
#ifdef _USE_HW_RTOS
        osThreadYield();
#endif
      }

      if (millis()-pre_time_ms >= timeout)
      {
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdSyncWrite(dxl_t *p_packet, dxlcmd_param_sync_write_t *p_param)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;

  uint8_t tx_param[4 + DXLCMD_MAX_NODE * DXLCMD_MAX_NODE_BUFFER_SIZE];
  uint32_t i;
  uint32_t j;
  uint32_t data_index;


  if (dxlIsOpen(p_packet) == true)
  {

    if (p_param->length > DXLCMD_MAX_NODE * DXLCMD_MAX_NODE_BUFFER_SIZE)
    {
      return DXL_RET_ERROR_LENGTH;
    }

    if (p_packet->packet_ver == DXL_PACKET_VER_1_0 )
    {
      tx_param[0] = p_param->addr;
      tx_param[1] = p_param->length;

      data_index = 2;
      for( i=0; i<p_param->id_count; i++)
      {
        tx_param[data_index++] = p_param->node[i].id;
        for (j=0; j<p_param->length; j++)
        {
          tx_param[data_index++] = p_param->node[i].data[j];
        }
      }
    }
    else
    {
      tx_param[0] = p_param->addr >> 0;
      tx_param[1] = p_param->addr >> 8;
      tx_param[2] = p_param->length >> 0;
      tx_param[3] = p_param->length >> 8;

      data_index = 4;
      for( i=0; i<p_param->id_count; i++)
      {
        tx_param[data_index++] = p_param->node[i].id;
        for (j=0; j<p_param->length; j++)
        {
          tx_param[data_index++] = p_param->node[i].data[j];
        }
      }
    }

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, DXL_GLOBAL_ID, INST_SYNC_WRITE, tx_param, data_index);
    p_packet->tx_time = micros() - pre_time_us;
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdBulkWrite(dxl_t *p_packet, dxlcmd_param_bulk_write_t *p_param)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;

  uint8_t tx_param[4 + DXLCMD_MAX_NODE];
  uint32_t i;
  uint32_t j;
  uint32_t data_index;
  uint32_t tx_buf_length;


  if (dxlIsOpen(p_packet) == true)
  {
    tx_buf_length = sizeof(tx_param);
    data_index = 0;

    for( i=0; i<p_param->id_count; i++)
    {
      tx_param[data_index++] = p_param->node[i].id;
      tx_param[data_index++] = p_param->node[i].addr >> 0;
      tx_param[data_index++] = p_param->node[i].addr >> 8;
      tx_param[data_index++] = p_param->node[i].length >> 0;
      tx_param[data_index++] = p_param->node[i].length >> 8;
      for (j=0; j<p_param->node[i].length; j++)
      {
        tx_param[data_index++] = p_param->node[i].data[j];
      }

      if (data_index > tx_buf_length)
      {
        return DXL_RET_ERROR_LENGTH;
      }
    }

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, DXL_GLOBAL_ID, INST_BULK_WRITE, tx_param, data_index);
    p_packet->tx_time = micros() - pre_time_us;
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdMemoryErase(dxl_t *p_packet, uint8_t id, uint32_t addr, uint32_t length, err_code_t *p_err, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;

  uint8_t tx_param[8];


  *p_err = OK;


  if (id == DXL_GLOBAL_ID)
  {
    return DXL_RET_ERROR_NOT_GLOBALID;
  }

  if (dxlIsOpen(p_packet) == true)
  {
    tx_param[0] = addr >> 0;
    tx_param[1] = addr >> 8;
    tx_param[2] = addr >> 16;
    tx_param[3] = addr >> 24;

    tx_param[4] = length >> 0;
    tx_param[5] = length >> 8;
    tx_param[6] = length >> 16;
    tx_param[7] = length >> 24;

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_MEMORY_ERASE, tx_param, 8);
    p_packet->tx_time = micros() - pre_time_us;

    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;


        if (p_packet->rx.error == DXL_RET_OK)
        {
          *p_err  = p_packet->rx.p_param[0]<<0;
          *p_err |= p_packet->rx.p_param[1]<<8;
          *p_err |= p_packet->rx.p_param[2]<<16;
          *p_err |= p_packet->rx.p_param[3]<<24;

          dxl_ret = DXL_RET_RX_RESP;
        }
        else
        {
          dxl_ret = DXL_RET_ERROR;
        }
        break;
      }
      else if (dxl_ret != DXL_RET_EMPTY)
      {
        break;
      }


      if (millis()-pre_time_ms >= timeout)
      {
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  if (dxl_ret != DXL_RET_RX_RESP)
  {
    *p_err = ERR_DXL_ERROR + dxl_ret;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdMemoryRead(dxl_t *p_packet, uint8_t id, uint32_t addr, uint8_t *p_data, uint32_t length, err_code_t *p_err, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;

  uint8_t tx_param[8];
  uint32_t i;


  *p_err = OK;


  if (id == DXL_GLOBAL_ID)
  {
    return DXL_RET_ERROR_NOT_GLOBALID;
  }

  if (dxlIsOpen(p_packet) == true)
  {
    tx_param[0] = addr >> 0;
    tx_param[1] = addr >> 8;
    tx_param[2] = addr >> 16;
    tx_param[3] = addr >> 24;

    tx_param[4] = length >> 0;
    tx_param[5] = length >> 8;
    tx_param[6] = length >> 16;
    tx_param[7] = length >> 24;

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_MEMORY_READ, tx_param, 8);
    p_packet->tx_time = micros() - pre_time_us;

    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;


        if (p_packet->rx.error == DXL_RET_OK)
        {
          *p_err  = p_packet->rx.p_param[0]<<0;
          *p_err |= p_packet->rx.p_param[1]<<8;
          *p_err |= p_packet->rx.p_param[2]<<16;
          *p_err |= p_packet->rx.p_param[3]<<24;

          for (i=4; i<p_packet->rx.param_length; i++)
          {
            p_data[i-4] = p_packet->rx.p_param[i];
          }
          dxl_ret = DXL_RET_RX_RESP;
        }
        else
        {
          dxl_ret = DXL_RET_ERROR;
        }
        break;
      }
      else if (dxl_ret != DXL_RET_EMPTY)
      {
        break;
      }


      if (millis()-pre_time_ms >= timeout)
      {
        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  if (dxl_ret != DXL_RET_RX_RESP)
  {
    *p_err = ERR_DXL_ERROR + dxl_ret;
  }

  return dxl_ret;
}

dxl_error_t dxlcmdMemoryWrite(dxl_t *p_packet, uint8_t id, uint32_t addr, uint8_t *p_data, uint32_t length, err_code_t *p_err, uint32_t timeout)
{
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint32_t pre_time_us;
  uint32_t pre_time_ms;

  uint8_t tx_param[DXLCMD_MAX_BUFFER+8];
  uint32_t i;


  *p_err = OK;


  if (id == DXL_GLOBAL_ID)
  {
    return DXL_RET_ERROR_NOT_GLOBALID;
  }

  if (dxlIsOpen(p_packet) == true)
  {
    tx_param[0] = addr >> 0;
    tx_param[1] = addr >> 8;
    tx_param[2] = addr >> 16;
    tx_param[3] = addr >> 24;

    tx_param[4] = length >> 0;
    tx_param[5] = length >> 8;
    tx_param[6] = length >> 16;
    tx_param[7] = length >> 24;


    if (length > DXLCMD_MAX_BUFFER+8)
    {
      return DXL_RET_ERROR_LENGTH;
    }

    for (i=0; i<length; i++)
    {
      tx_param[8+i] = p_data[i];
    }

    pre_time_us = micros();
    dxl_ret = dxlTxPacketInst(p_packet, id, INST_MEMORY_WRITE, tx_param, 8+length);
    p_packet->tx_time = micros() - pre_time_us;

    pre_time_ms = millis();
    pre_time_us = micros();
    while(1)
    {
      dxl_ret = dxlRxPacket(p_packet);
      if (dxl_ret == DXL_RET_RX_STATUS)
      {
        pre_time_ms = millis();
        p_packet->rx_time = micros() - pre_time_us;


        if (p_packet->rx.error == DXL_RET_OK)
        {
          *p_err  = p_packet->rx.p_param[0]<<0;
          *p_err |= p_packet->rx.p_param[1]<<8;
          *p_err |= p_packet->rx.p_param[2]<<16;
          *p_err |= p_packet->rx.p_param[3]<<24;

          dxl_ret = DXL_RET_RX_RESP;
        }
        else
        {
          dxl_ret = DXL_RET_ERROR;
        }
        break;
      }
      else if (dxl_ret != DXL_RET_EMPTY)
      {
        break;
      }


      if (millis()-pre_time_ms >= timeout)
      {

        break;
      }
    }
  }
  else
  {
    dxl_ret = DXL_RET_NOT_OPEN;
  }

  if (dxl_ret != DXL_RET_RX_RESP)
  {
    *p_err = ERR_DXL_ERROR + dxl_ret;
  }

  return dxl_ret;
}


data_t getDataType(uint8_t *p_data, uint8_t length)
{
  data_t ret;
  uint32_t i;


  ret.u32Data = 0;

  for (i=0; i<length; i++)
  {
    ret.u8Data[i] = p_data[i];
  }

  return ret;
}


#ifdef _USE_HW_CMDIF_DXLCMD
void dxlcmdCmdifInit(void)
{
  cmdifAdd("dxlcmd", dxlcmdCmdif);
}

int dxlcmdCmdif(int argc, char **argv)
{
  bool ret = true;
  uint32_t number;
  uint32_t pre_time;
  static dxl_t dxl_cmd;
  dxl_error_t dxl_ret = DXL_RET_OK;
  uint16_t addr;
  uint16_t length;

  dxlcmd_param_t param;
  dxlcmd_resp_t  resp;


  if (argc == 2)
  {
    if (strcmp("info", argv[1]) == 0)
    {
      if (dxlGetProtocolVersion(&dxl_cmd) == DXL_PACKET_VER_1_0)
      {
        cmdifPrintf("dxl protocol 1.0\n");
      }
      else
      {
        cmdifPrintf("dxl protocol 2.0\n");
      }

      if (dxlIsOpen(&dxl_cmd) == true)
      {
        cmdifPrintf("dxl is opened\n");
      }
      else
      {
        cmdifPrintf("dxl is closed\n");
      }
      cmdifPrintf("param length : %d\n", sizeof(dxlcmd_param_t));
      cmdifPrintf("resp length  : %d\n", sizeof(dxlcmd_resp_t));

      delay(50);
    }
    else if (strcmp("close", argv[1]) == 0)
    {
      if (dxlIsOpen(&dxl_cmd) == true)
      {
        dxlClosePort(&dxl_cmd);
      }
    }
    else if(strcmp("ping", argv[1]) == 0)
    {
      if (dxlIsOpen(&dxl_cmd) == false)
      {
        cmdifPrintf( "dxl not opened \n");
        return 0;
      }

      cmdifPrintf("ping id:0xFE \n");

      pre_time = micros();
      dxl_ret = dxlcmdPing(&dxl_cmd, DXL_GLOBAL_ID, &resp.ping, 256*3);
      if (dxl_ret == DXL_RET_RX_RESP)
      {
        cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
        cmdifPrintf("  rx_time : %d us\n", dxl_cmd.rx_time);

        for (int i=0; i<resp.ping.id_count; i++)
        {
          cmdifPrintf("rx_st : id %03d, md 0x%X(%d), ver %d  %d us\n", resp.ping.p_node[i]->id, resp.ping.p_node[i]->model_number, resp.ping.p_node[i]->model_number, resp.ping.p_node[i]->firmware_version, micros()-pre_time);
        }
      }
      else
      {
        cmdifPrintf("no resp : %d\n", dxl_ret);
      }
    }
    else if(strcmp("monitor", argv[1]) == 0)
    {
      uint8_t data;

      while(cmdifRxAvailable() == 0)
      {
        if (dxlRxAvailable(&dxl_cmd))
        {
          data = dxlRxRead(&dxl_cmd);
          //ret  = dxlRxPacket1_0(&dxl_cmd, data);

          cmdifPrintf("rx : 0x%X\n", data);

          /*
          if (ret != DXL_RET_EMPTY)
          {
            break;
          }
          else
          {
            vcpPrintf("rx_packet : %d\n", ret);
          }
          */
        }
      }
    }
    else
    {
      ret = false;
    }
  }
  else if (argc == 3)
  {
    number = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);

    if (strcmp("open", argv[1]) == 0)
    {
      dxlInit(&dxl_cmd, DXL_PACKET_VER_2_0);
      ret = dxlOpenPort(&dxl_cmd, _DEF_DXL1, number);
      if (ret == true)
      {
        cmdifPrintf("open is OK Ver2.0, _DEF_DXL1, %d bps", number);
      }
      else
      {
        cmdifPrintf("open is FAIL, _DEF_DXL1, %d bps", number);
      }
    }
    else if(strcmp("ping", argv[1]) == 0 && dxlIsOpen(&dxl_cmd) == true)
    {
      cmdifPrintf("ping id:%d(0x%X) \n", number, number);

      pre_time = micros();
      dxl_ret = dxlcmdPing(&dxl_cmd, number, &resp.ping, 100);
      if (dxl_ret == DXL_RET_RX_RESP)
      {
        cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
        cmdifPrintf("  rx_time : %d us\n", dxl_cmd.rx_time);

        for (int i=0; i<resp.ping.id_count; i++)
        {
          cmdifPrintf("rx_st : id %d, md 0x%X(%d), ver %d  %d us\n", resp.ping.p_node[i]->id, resp.ping.p_node[i]->model_number, resp.ping.p_node[i]->model_number, resp.ping.p_node[i]->firmware_version, micros()-pre_time);
        }
      }
      else
      {
        cmdifPrintf("no resp\n");
      }
    }
    else if(strcmp("reboot", argv[1]) == 0 && dxlIsOpen(&dxl_cmd) == true)
    {
      cmdifPrintf("reboot id:%d(0x%X) \n", number, number);

      pre_time = micros();
      dxl_ret = dxlcmdReboot(&dxl_cmd, number, &resp.reboot, 100);
      if (dxl_ret == DXL_RET_RX_RESP)
      {
        cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
        cmdifPrintf("  rx_time : %d us\n", dxl_cmd.rx_time);

        cmdifPrintf("reboot id:%d err:0x%02X(%d)\n", resp.reboot.id, resp.reboot.error, resp.reboot.error);
      }
      else
      {
        cmdifPrintf("no resp\n");
      }
    }

    if (dxlIsOpen(&dxl_cmd) == false)
    {
      cmdifPrintf( "dxl not opened \n");
      return 0;
    }
  }
  else if (argc == 4)
  {
    number = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);

    if (strcmp("open", argv[1]) == 0)
    {
      dxlInit(&dxl_cmd, DXL_PACKET_VER_1_0);
      ret = dxlOpenPort(&dxl_cmd, _DEF_DXL1, number);
      if (ret == true)
      {
        cmdifPrintf("open is OK Ver1.0, _DEF_DXL1, %d bps", number);
      }
      else
      {
        cmdifPrintf("open is FAIL, _DEF_DXL1, %d bps", number);
      }
    }
  }
  else if (argc == 5)
  {
    number = (uint32_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    addr   = (uint16_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);
    length = (uint16_t) strtoul((const char * ) argv[4], (char **)NULL, (int) 0);

    if (strcmp("read", argv[1]) == 0 && dxlIsOpen(&dxl_cmd) == true)
    {
      cmdifPrintf("read id:%d(0x%X) \n", number, number);

      dxl_ret = dxlcmdRead(&dxl_cmd, number, addr, length, &resp.read, 100);
      cmdifPrintf("read\n");

      if (dxl_ret == DXL_RET_RX_RESP)
      {
        cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
        cmdifPrintf("  rx_time : %d us\n", dxl_cmd.rx_time);

        for (int i=0; i<resp.read.id_count; i++)
        {
          for (int j=0; j<resp.read.p_node[i]->length; j++)
          {
            cmdifPrintf("  data : %d -> 0x%02X %03d  \n", addr+j, resp.read.p_node[i]->p_data[j], resp.read.p_node[i]->p_data[j]);
          }
        }
      }
      else
      {
        cmdifPrintf("no resp\n");
      }
    }

    if (dxlIsOpen(&dxl_cmd) == false)
    {
      cmdifPrintf( "dxl not opened \n");
      return 0;
    }
  }
  else if (argc == 6)
  {
    uint8_t s_id;
    uint8_t e_id;

    s_id   = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    e_id   = (uint8_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);
    addr   = (uint16_t) strtoul((const char * ) argv[4], (char **)NULL, (int) 0);
    length = (uint16_t) strtoul((const char * ) argv[5], (char **)NULL, (int) 0);

    if (strcmp("write", argv[1]) == 0 && dxlIsOpen(&dxl_cmd) == true)
    {
      uint32_t tx_data;

      s_id    = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
      addr    = (uint16_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);
      tx_data = (uint32_t) strtoul((const char * ) argv[4], (char **)NULL, (int) 0);
      length  = (uint16_t) strtoul((const char * ) argv[5], (char **)NULL, (int) 0);


      dxl_ret = dxlcmdWrite(&dxl_cmd, s_id, (uint8_t *)&tx_data, addr, length, &resp.write, 100);
      cmdifPrintf("write\n");

      if (dxl_ret == DXL_RET_RX_RESP)
      {
        cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
        cmdifPrintf("  rx_time : %d us\n", dxl_cmd.rx_time);

        cmdifPrintf("write id:%d err:0x%02X(%d)\n", resp.write.id, resp.write.error, resp.write.error);
      }
      else
      {
        cmdifPrintf("no resp %d \n", dxl_ret);
      }
    }
    else if (strcmp("syncread", argv[1]) == 0 && dxlIsOpen(&dxl_cmd) == true)
    {
      param.sync_read.id_count = 0;

      for (uint8_t i=s_id; i<=e_id; i++)
      {
        param.sync_read.id_tbl[param.sync_read.id_count] = i;
        param.sync_read.id_count++;
      }
      param.sync_read.addr   = addr;
      param.sync_read.length = length;

      if (param.sync_read.id_count == 0)
      {
        return 0;
      }

      dxl_ret = dxlcmdSyncRead(&dxl_cmd, &param.sync_read, &resp.sync_read, 100);
      cmdifPrintf("syncread\n");

      if (dxl_ret == DXL_RET_RX_RESP)
      {
        cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
        cmdifPrintf("  rx_time : %d us\n", dxl_cmd.rx_time);

        for (int i=0; i<resp.sync_read.id_count; i++)
        {
          cmdifPrintf("read id:%d %d\n", resp.sync_read.p_node[i]->id, dxl_cmd.rx.param_length);

          for (int j=0; j<resp.sync_read.p_node[i]->length; j++)
          {
            cmdifPrintf("  data : %d -> 0x%02X %03d  \n", addr+j, resp.sync_read.p_node[i]->p_data[j], resp.sync_read.p_node[i]->p_data[j]);
          }
        }
      }
      else
      {
        cmdifPrintf("no resp %d %d\n", dxl_ret, resp.sync_read.id_count);
      }
    }
    else if (strcmp("bulkread", argv[1]) == 0 && dxlIsOpen(&dxl_cmd) == true)
    {
      param.bulk_read.id_count = 0;

      for (uint8_t i=s_id; i<=e_id; i++)
      {
        param.bulk_read.id_tbl[param.bulk_read.id_count] = i;
        param.bulk_read.addr  [param.bulk_read.id_count] = addr;
        param.bulk_read.length[param.bulk_read.id_count] = length;
        param.bulk_read.id_count++;

      }
      if (param.bulk_read.id_count == 0)
      {
        return 0;
      }

      dxl_ret = dxlcmdBulkRead(&dxl_cmd, &param.bulk_read, &resp.bulk_read, 100);
      cmdifPrintf("bulkread\n");

      if (dxl_ret == DXL_RET_RX_RESP)
      {
        cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
        cmdifPrintf("  rx_time : %d us\n", dxl_cmd.rx_time);

        for (int i=0; i<resp.bulk_read.id_count; i++)
        {
          cmdifPrintf("read id:%d %d\n", resp.bulk_read.p_node[i]->id, dxl_cmd.rx.param_length);

          for (int j=0; j<resp.bulk_read.p_node[i]->length; j++)
          {
            cmdifPrintf("  data : %d -> 0x%02X %03d  \n", addr+j, resp.bulk_read.p_node[i]->p_data[j], resp.bulk_read.p_node[i]->p_data[j]);
          }
        }
      }
      else
      {
        cmdifPrintf("no resp %d %d\n", dxl_ret, resp.bulk_read.id_count);
      }
    }
  }
  else if (argc == 7)
  {
    uint8_t s_id;
    uint8_t e_id;
    uint32_t data;

    s_id   = (uint8_t) strtoul((const char * ) argv[2], (char **)NULL, (int) 0);
    e_id   = (uint8_t) strtoul((const char * ) argv[3], (char **)NULL, (int) 0);
    addr   = (uint16_t) strtoul((const char * ) argv[4], (char **)NULL, (int) 0);
    data   = (uint32_t) strtoul((const char * ) argv[5], (char **)NULL, (int) 0);
    length = (uint16_t) strtoul((const char * ) argv[6], (char **)NULL, (int) 0);

    if (strcmp("syncwrite", argv[1]) == 0 && dxlIsOpen(&dxl_cmd) == true)
    {
      uint8_t *p_data = (uint8_t *)&data;

      param.sync_write.id_count = 0;
      param.sync_write.addr     = addr;
      param.sync_write.length   = length;

      for (uint8_t i=s_id; i<=e_id; i++)
      {
        param.sync_write.node[param.sync_write.id_count].id     = i;

        for (uint8_t j=0; j<length; j++)
        {
          param.sync_write.node[param.sync_write.id_count].data[j] = p_data[j];
        }

        param.sync_write.id_count++;
      }

      if (param.sync_write.id_count == 0)
      {
        return 0;
      }


      dxl_ret = dxlcmdSyncWrite(&dxl_cmd, &param.sync_write);
      cmdifPrintf("syncwrite\n");
      cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
      if (dxl_ret == DXL_RET_OK)
      {
        cmdifPrintf("  ret : OK\n");
      }
      else
      {
        cmdifPrintf("  ret fail : %d\n", dxl_ret);
      }
    }
    else if (strcmp("bulkwrite", argv[1]) == 0 && dxlIsOpen(&dxl_cmd) == true)
    {
      uint8_t *p_data = (uint8_t *)&data;

      param.bulk_write.id_count = 0;

      for (uint8_t i=s_id; i<=e_id; i++)
      {
        param.bulk_write.node[param.bulk_write.id_count].id     = i;
        param.bulk_write.node[param.bulk_write.id_count].addr   = addr;
        param.bulk_write.node[param.bulk_write.id_count].length = length;

        for (uint8_t j=0; j<length; j++)
        {
          param.bulk_write.node[param.bulk_write.id_count].data[j] = p_data[j];
        }

        param.bulk_write.id_count++;
      }

      if (param.bulk_write.id_count == 0)
      {
        return 0;
      }


      dxl_ret = dxlcmdBulkWrite(&dxl_cmd, &param.bulk_write);
      cmdifPrintf("bulkwrite\n");
      cmdifPrintf("  tx_time : %d us\n", dxl_cmd.tx_time);
      if (dxl_ret == DXL_RET_OK)
      {
        cmdifPrintf("  ret : OK\n");
      }
      else
      {
        cmdifPrintf("  ret fail : %d\n", dxl_ret);
      }
    }
  }
  else
  {
    ret = false;
  }


  if (dxlIsOpen(&dxl_cmd) == false)
  {
    cmdifPrintf( "dxl not opened \n");
  }

  if (ret == false)
  {
    cmdifPrintf( "dxlcmd info \n");
    cmdifPrintf( "dxlcmd open baud\n");
    cmdifPrintf( "dxlcmd close \n");
    cmdifPrintf( "dxlcmd ping  \n");
    cmdifPrintf( "dxlcmd ping id \n");
    cmdifPrintf( "dxlcmd reboot id \n");
    cmdifPrintf( "dxlcmd read id addr length\n");
    cmdifPrintf( "dxlcmd write id addr data length\n");
    cmdifPrintf( "dxlcmd syncread s_id e_id addr length\n");
    cmdifPrintf( "dxlcmd bulkread s_id e_id addr length\n");
    cmdifPrintf( "dxlcmd syncwrite s_id e_id addr data length\n");
    cmdifPrintf( "dxlcmd bulkwrite s_id e_id addr data length\n");
  }

  return 0;
}
#endif /* _USE_HW_CMDIF_DXLCMD */

#endif /* _USE_HW_DXLCMD */
