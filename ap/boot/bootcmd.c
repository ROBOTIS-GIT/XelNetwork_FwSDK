/*
 *  ap.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */

#include "bootcmd.h"

#ifdef _USE_BOOTCMD

#include <string.h>
#include "hw.h"
#include "core/util.h"


#define BOOT_ERR_INVAILD_CMD            0xF0F0

#define BOOT_CMD_READ_VERSION           0x80
#define BOOT_CMD_PACKET_DATA_SIZE_MAX   0x81
#define BOOT_CMD_READ_BOARD_NAME        0x82
#define BOOT_CMD_FLASH_ERASE            0x83
#define BOOT_CMD_FLASH_WRITE            0x84
#define BOOT_CMD_FLASH_READ             0x85
#define BOOT_CMD_FLASH_VERIFY           0x86
#define BOOT_CMD_JUMP                   0x87




static void bootCmdReadVersion(cmd_t *p_cmd);
static void bootCmdPacketDataSizeMax(cmd_t *p_cmd);
static void bootCmdReadBoardName(cmd_t *p_cmd);
static void bootCmdFlashErase(cmd_t *p_cmd);
static void bootCmdFlashWrite(cmd_t *p_cmd);
static void bootCmdFlashRead(cmd_t *p_cmd);
static void bootCmdJumpToAddress(cmd_t *p_cmd);
static void bootCmdFlashVerify(cmd_t *p_cmd);

void bootCmdInit(void)
{
}

err_code_t checkFw(uint32_t type, uint32_t address)
{
  fw_tag_type_a_t *p_tag;
  uint8_t *p_data;
  uint32_t i;
  uint16_t crc = 0;

  /* Check Address */
  if(address < _HW_DEF_FLASH_ADDR_APP_START)
    return ERR_FLASH_INVALID_ADDR;

  if(type != *(uint32_t*) address)
    return ERR_FLASH_INVALID_TAG;

  /* Check Type & CRC16 */
  if(type == FW_TAG_TYPE_A)
  {
    p_tag = (fw_tag_type_a_t*) (address);
    p_data = (uint8_t*) p_tag->address;

    if(p_data < (uint8_t*)_HW_DEF_FLASH_ADDR_APP_START)
    {
      return ERR_FLASH_INVALID_TAG;
    }

    for (i = 0; i < p_tag->length; i++)
    {
      utilUpdateCrc(&crc, p_data[i]);
    }

    if(crc != p_tag->crc)
    {
      return ERR_FLASH_CRC;
    }
  }
  else
  {
    return ERR_FLASH_INVALID_TAG;
  }

  return OK;
}

void bootCmdReadVersion(cmd_t *p_cmd)
{
  uint8_t data[8] = {0, };
  uint32_t i = 0;

  data[i++] = _BSP_BOARD_VER_NUM;
  data[i++] = 0;

  cmdSendResp(p_cmd, OK, data, i);
}

void bootCmdPacketDataSizeMax(cmd_t *p_cmd)
{
  uint8_t data[8] = {0, };
  uint16_t t_size = (uint16_t) CMD_MAX_DATA_LENGTH;
  uint32_t i = 0;

  data[i++] = (uint8_t) t_size;
  data[i++] = (uint8_t) (t_size >> 8);

  cmdSendResp(p_cmd, OK, data, i);
}

void bootCmdReadBoardName(cmd_t *p_cmd)
{
  cmdSendResp(p_cmd, OK, (uint8_t*)_BSP_DEF_BOARD_NAME, strlen((char *)_BSP_DEF_BOARD_NAME)+1);
}

void bootCmdFlashErase(cmd_t *p_cmd)
{
  err_code_t err_code = OK;
  uint32_t addr_begin;
  uint32_t addr_end;
  uint32_t length;

  addr_begin = utilConvert8ToU32(&p_cmd->rx_packet.data[0]);
  length     = utilConvert8ToU32(&p_cmd->rx_packet.data[4]);
  addr_end   = addr_begin + length - 1;

  if ((addr_begin >= _HW_DEF_FLASH_ADDR_APP_START)
      && (addr_end < _HW_DEF_FLASH_ADDR_APP_END))
  {
    if (addr_begin % 4 == 0)
    {
      err_code = flashErase(addr_begin, length);
    }
    else
    {
      err_code = ERR_FLASH_ADDR_ALIGN;
    }
  }
  else
  {
    err_code = ERR_FLASH_INVALID_ADDR;
  }

  cmdSendResp(p_cmd, err_code, NULL, 0);

}

void bootCmdFlashWrite(cmd_t *p_cmd)
{
  err_code_t err_code = OK;
  uint32_t addr_begin;
  uint32_t addr_end;
  uint32_t length;
  uint8_t *p_flash, *p_data;

  addr_begin = utilConvert8ToU32(&p_cmd->rx_packet.data[0]);
  length     = utilConvert8ToU32(&p_cmd->rx_packet.data[4]);
  addr_end   = addr_begin + length - 1;

  if ((addr_begin >= _HW_DEF_FLASH_ADDR_APP_START)
      && (addr_end < _HW_DEF_FLASH_ADDR_APP_END))
  {
    if (addr_begin % 4 == 0)
    {
      err_code = flashWrite(addr_begin, &p_cmd->rx_packet.data[8], length);
    }
    else
    {
      err_code = ERR_FLASH_ADDR_ALIGN;
    }
  }
  else
  {
    err_code = ERR_FLASH_INVALID_ADDR;
  }

  if(err_code == OK)
  {
    p_flash = (uint8_t*) addr_begin;
    p_data  = &p_cmd->rx_packet.data[8];
    for(uint32_t i=0; i<length; i++)
    {
      if(p_flash[i] != p_data[i])
      {
        err_code = ERR_FLASH_WRITE;
        break;
      }
    }
  }

  cmdSendResp(p_cmd, err_code, NULL, 0);
}

void bootCmdFlashRead(cmd_t *p_cmd)
{
  err_code_t err_code = OK;
  uint32_t addr_begin;
  uint32_t length;
  uint32_t i;
  uint8_t *p_data;

  addr_begin = utilConvert8ToU32(&p_cmd->rx_packet.data[0]);
  length     = utilConvert8ToU32(&p_cmd->rx_packet.data[4]);

  if (length > sizeof(p_cmd->tx_packet.data))
  {
    err_code = ERR_INVALID_LENGTH;
  }
  else
  {
    p_data = (uint8_t *) addr_begin;
    for (i = 0; i < length; i++)
    {
      p_cmd->tx_packet.data[i] = p_data[i];
    }
  }

  if (err_code == OK)
  {
    cmdSendResp(p_cmd, err_code, p_cmd->tx_packet.data, length);
  }
  else
  {
    cmdSendResp(p_cmd, err_code, NULL, 0);
  }
}

void bootCmdFlashVerify(cmd_t *p_cmd)
{
  err_code_t err_code = OK;
  uint32_t addr_begin;
  uint32_t length;
  uint16_t rx_crc;
  uint16_t tx_crc = 0;
  uint8_t *p_data;
  uint32_t i;

  addr_begin = utilConvert8ToU32(&p_cmd->rx_packet.data[0]);
  length     = utilConvert8ToU32(&p_cmd->rx_packet.data[4]);
  rx_crc     = utilConvert8ToU16(&p_cmd->rx_packet.data[8]);
  p_data     = (uint8_t *) addr_begin;

  for (i = 0; i < length; i++)
  {
    utilUpdateCrc(&tx_crc, p_data[i]);
  }

  if(tx_crc != rx_crc)
  {
    err_code = ERR_FLASH_CRC;
  }

  cmdSendResp(p_cmd, err_code, (uint8_t *)&tx_crc, 2);
}

void bootCmdJumpToAddress(cmd_t *p_cmd)
{
  uint32_t address = utilConvert8ToU32(&p_cmd->rx_packet.data[0]);

  err_code_t err = checkFw(FW_TAG_TYPE_A, address);

  cmdSendResp(p_cmd, err, NULL, 0);
  if(err == OK)
  {
    fw_tag_type_a_t *p_tag = (fw_tag_type_a_t*) (address);
    delay(100);
    bspJumpToAddress(p_tag->address);
  }
}

void bootCmdProcess(cmd_t *p_cmd)
{
  switch (p_cmd->rx_packet.cmd)
  {
    case BOOT_CMD_READ_VERSION:
      bootCmdReadVersion(p_cmd);
      break;

    case BOOT_CMD_PACKET_DATA_SIZE_MAX:
      bootCmdPacketDataSizeMax(p_cmd);
      break;

    case BOOT_CMD_READ_BOARD_NAME:
      bootCmdReadBoardName(p_cmd);
      break;

    case BOOT_CMD_FLASH_ERASE:
      bootCmdFlashErase(p_cmd);
      break;

    case BOOT_CMD_FLASH_WRITE:
      bootCmdFlashWrite(p_cmd);
      break;

    case BOOT_CMD_FLASH_READ:
      bootCmdFlashRead(p_cmd);
      break;

    case BOOT_CMD_FLASH_VERIFY:
      bootCmdFlashVerify(p_cmd);
      break;

    case BOOT_CMD_JUMP:
      bootCmdJumpToAddress(p_cmd);
      break;

    default:
      cmdSendResp(p_cmd, BOOT_ERR_INVAILD_CMD, NULL, 0);
      break;
  }
}

#endif /* _USE_BOOTCMD */

