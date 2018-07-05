/*
 *  cmd.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram
 */



#include "cmd.h"

#ifdef _USE_CMD

#include "ap.h"
#include "hw.h"


#define CMD_STX                     0xEF
#define CMD_ETX                     0xFE

#define CMD_STATE_WAIT_STX          0
#define CMD_STATE_WAIT_CMD          1
#define CMD_STATE_WAIT_ERROR_L      2
#define CMD_STATE_WAIT_ERROR_H      3
#define CMD_STATE_WAIT_LENGTH_L     4
#define CMD_STATE_WAIT_LENGTH_H     5
#define CMD_STATE_WAIT_DATA         6
#define CMD_STATE_WAIT_CHECKSUM     7
#define CMD_STATE_WAIT_ETX          8

#define CMD_BYTE_TIMEOUT_MS         500


static void cmdPutch(uint8_t ch, uint8_t data);


void cmdInit(cmd_t *p_cmd)
{
  p_cmd->init  = false;
  p_cmd->state = CMD_STATE_WAIT_STX;

  p_cmd->rx_packet.error = 0;
  p_cmd->tx_packet.error = 0;
}

void cmdBegin(cmd_t *p_cmd, uint8_t ch, uint32_t baud)
{
  p_cmd->ch   = ch;
  p_cmd->baud = baud;
  p_cmd->init = true;
  p_cmd->state = CMD_STATE_WAIT_STX;

  p_cmd->save_time[0] = millis();
  p_cmd->save_time[1] = millis();

  uartOpen(ch, baud);
}

void cmdPutch(uint8_t ch, uint8_t data)
{
  uartPutch(ch, data);
}

void cmdFlush(cmd_t *p_cmd)
{
  uint32_t t_time = millis();

  while(millis() - t_time < CMD_BYTE_TIMEOUT_MS/2)
  {
    if( uartAvailable(p_cmd->ch) )
    {
      uartFlush(p_cmd->ch);
      t_time = millis();
    }
  }
}

bool cmdReceivePacket(cmd_t *p_cmd)
{
	bool     ret = false;
	uint8_t  ch;
	uint32_t index;

	//-- 명령어 수신
	//
	if( uartAvailable(p_cmd->ch) )
	{
		ch = uartRead(p_cmd->ch);
	}
	else
	{
		return false;
	}

	//-- 바이트간 타임아웃 설정(500ms)
	//
	if((millis()-p_cmd->save_time[0]) > CMD_BYTE_TIMEOUT_MS)
	{
		p_cmd->state        = CMD_STATE_WAIT_STX;
		p_cmd->save_time[0] = millis();
	}

	//-- 명령어 상태
	//
	switch(p_cmd->state)
	{
		//-- STX 문자 기다리는 상태
		//
		case CMD_STATE_WAIT_STX:

			// 시작 문자를 기다림
			if( ch == CMD_STX )
			{
				p_cmd->state               = CMD_STATE_WAIT_CMD;
				p_cmd->rx_packet.check_sum = 0x00;
				p_cmd->rx_packet.length    = 0;
				p_cmd->packet_err          = 0;
			}
			break;

		//-- 명령어 기다리는 상태
		//
		case CMD_STATE_WAIT_CMD:
		  p_cmd->rx_packet.cmd        = ch;
		  p_cmd->rx_packet.check_sum ^= ch;
			p_cmd->state                = CMD_STATE_WAIT_ERROR_L;
			break;

    case CMD_STATE_WAIT_ERROR_L:
      p_cmd->rx_packet.option     = ch;
      p_cmd->rx_packet.error      = ch;
      p_cmd->rx_packet.check_sum ^= ch;
      p_cmd->state                = CMD_STATE_WAIT_ERROR_H;
      break;

    case CMD_STATE_WAIT_ERROR_H:
      p_cmd->rx_packet.option    |= ch << 8;
      p_cmd->rx_packet.error     |= ch << 8;
      p_cmd->rx_packet.check_sum ^= ch;
      p_cmd->state                = CMD_STATE_WAIT_LENGTH_L;
      break;

		//-- 데이터 사이즈 기다리는 상태
		//
		case CMD_STATE_WAIT_LENGTH_L:
      p_cmd->rx_packet.length     = ch;
      p_cmd->rx_packet.check_sum ^= ch;
      p_cmd->state                = CMD_STATE_WAIT_LENGTH_H;
      break;

    case CMD_STATE_WAIT_LENGTH_H:
      p_cmd->rx_packet.length    |= ch<<8;
      p_cmd->rx_packet.check_sum ^= ch;
      p_cmd->state                = CMD_STATE_WAIT_LENGTH_H;

			if (p_cmd->rx_packet.length <= CMD_MAX_DATA_LENGTH)
			{
				if (p_cmd->rx_packet.length > 0)
				{
				  p_cmd->rx_packet.index = 0;
				  p_cmd->state = CMD_STATE_WAIT_DATA;
				}
				else
				{
				  p_cmd->state = CMD_STATE_WAIT_CHECKSUM;
				}
			}
			else
			{
			  p_cmd->packet_err = ERR_INVALID_LENGTH;
			  p_cmd->state      = CMD_STATE_WAIT_STX;
			  ret               = true;
			}
			break;

		//-- 데이터를 기다리는 상태
		//
		case CMD_STATE_WAIT_DATA:
		  index = p_cmd->rx_packet.index;

		  p_cmd->rx_packet.check_sum ^= ch;
		  index = p_cmd->rx_packet.index;
		  p_cmd->rx_packet.data[index] = ch;

		  p_cmd->rx_packet.index++;

			if (p_cmd->rx_packet.index >= p_cmd->rx_packet.length)
			{
			  p_cmd->state = CMD_STATE_WAIT_CHECKSUM;
			}
			break;

		//-- 체크섬을 기다리는 상태
		//
		case CMD_STATE_WAIT_CHECKSUM:
		  p_cmd->rx_packet.check_sum_recv = ch;
		  p_cmd->state                    = CMD_STATE_WAIT_ETX;
			break;

		//-- ETX 기다리는 상태
		//
		case CMD_STATE_WAIT_ETX:

			if (ch == CMD_ETX)
			{
				if (p_cmd->rx_packet.check_sum_recv == p_cmd->rx_packet.check_sum)
				{
					ret = true;
				}
			}
			p_cmd->state = CMD_STATE_WAIT_STX;
			break;
	}

	return ret;
}

void cmdSendResp(cmd_t *p_cmd, uint16_t err_code, uint8_t *p_data, uint32_t length)
{
	uint32_t i;
	uint8_t  ch;
	uint8_t  check_sum = 0;
	uint8_t  data;

	ch = p_cmd->ch;

  p_cmd->tx_packet.cmd   = p_cmd->rx_packet.cmd;
  p_cmd->tx_packet.error = err_code;

  if (p_data != NULL)
  {
    for(i=0; i<length; i++)
    {
      p_cmd->tx_packet.data[i] = p_data[i];
    }
  }
  p_cmd->tx_packet.length = length;


	cmdPutch(ch, CMD_STX);
	cmdPutch(ch, p_cmd->tx_packet.cmd);
	check_sum ^= p_cmd->tx_packet.cmd;

	data = p_cmd->tx_packet.error & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;
  data = (p_cmd->tx_packet.error>>8) & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;

  data = p_cmd->tx_packet.length & 0xFF;
	cmdPutch(ch, data); check_sum ^= data;
  data = (p_cmd->tx_packet.length>>8) & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;

	for( i=0; i<p_cmd->tx_packet.length && i<CMD_MAX_DATA_LENGTH; i++ )
	{
	  cmdPutch(ch, p_cmd->tx_packet.data[i]);
		check_sum ^= p_cmd->tx_packet.data[i];
	}

	cmdPutch(ch, check_sum);
	cmdPutch(ch, CMD_ETX);
}





/*---------------------------------------------------------------------------
     TITLE   : cmdSendCmd
     WORK    :
     ARG     : void
     RET     : void
---------------------------------------------------------------------------*/
void cmdSendCmd(cmd_t *p_cmd, uint8_t cmd, uint8_t *p_data, uint32_t length)
{
  uint32_t i;
  uint8_t  ch;
  uint8_t  check_sum = 0;
  uint8_t  data;

  ch = p_cmd->ch;


  p_cmd->tx_packet.cmd    = cmd;
  p_cmd->tx_packet.option = 0;

  if (p_data != NULL)
  {
    for(i=0; i<length; i++)
    {
      p_cmd->tx_packet.data[i] = p_data[i];
    }
  }
  p_cmd->tx_packet.length = length;


  cmdPutch(ch, CMD_STX);
  cmdPutch(ch, p_cmd->tx_packet.cmd);
  check_sum ^= p_cmd->tx_packet.cmd;

  data = p_cmd->tx_packet.option & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;
  data = (p_cmd->tx_packet.option>>8) & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;

  data = p_cmd->tx_packet.length & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;
  data = (p_cmd->tx_packet.length>>8) & 0xFF;
  cmdPutch(ch, data); check_sum ^= data;


  for( i=0; i<p_cmd->tx_packet.length && i<CMD_MAX_DATA_LENGTH; i++ )
  {
    cmdPutch(ch, p_cmd->tx_packet.data[i]);
    check_sum ^= p_cmd->tx_packet.data[i];
  }

  cmdPutch(ch, check_sum);
  cmdPutch(ch, CMD_ETX);
}


#endif /* _USE_CMD */
