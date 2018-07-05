/*
 * qbuffer.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: HanCheol Cho
 */
#include <stdlib.h>

#include "def.h"
#include "error_code.h"
#include "qbuffer.h"




//-- Internal Variables
//


//-- External Variables
//


//-- Internal Functions
//


//-- External Functions
//





bool qbufferInit(void)
{

  return true;
}

err_code_t qbufferCreate(qbuffer_node_t *p_node, uint32_t length)
{
  err_code_t err_code = OK;


  p_node->ptr_in  = 0;
  p_node->ptr_out = 0;
  p_node->length  = length;
  p_node->p_buf   = (uint8_t *)malloc(length);

  if (p_node->p_buf == NULL)
  {
    p_node->length = 0;
    err_code       = ERR_MEMORY;
  }


  return err_code;
}

uint32_t qbufferAvailable(qbuffer_node_t *p_node)
{
  uint32_t length;


  length = (p_node->length + p_node->ptr_in - p_node->ptr_out) % p_node->length;

  return length;
}

err_code_t qbufferWrite(qbuffer_node_t *p_node, uint8_t *p_data, uint32_t length)
{
  err_code_t err_code = OK;
  uint32_t index;
  uint32_t next_index;
  uint32_t i;

  if (p_node->p_buf == NULL) return ERR_NULL;


  for (i=0; i<length; i++)
  {
    index      = p_node->ptr_in;
    next_index = p_node->ptr_in + 1;

    if (next_index == p_node->length)
    {
      next_index = 0;;
    }

    if (next_index != p_node->ptr_out)
    {
      p_node->p_buf[index] = p_data[i];
      p_node->ptr_in       = next_index;
    }
    else
    {
      err_code = ERR_FULL;
      break;
    }
  }

  return err_code;
}

err_code_t qbufferWriteByte(qbuffer_node_t *p_node, uint8_t data)
{
  return qbufferWrite(p_node, &data, 1);
}

err_code_t qbufferRead(qbuffer_node_t *p_node, uint8_t *p_data, uint32_t length)
{
  err_code_t err_code = OK;
  uint32_t index;
  uint32_t next_index;
  uint32_t i;

  if (p_node->p_buf == NULL) return ERR_NULL;


  for (i=0; i<length; i++)
  {
    index      = p_node->ptr_out;
    next_index = p_node->ptr_out + 1;

    if (next_index == p_node->length)
    {
      next_index = 0;
    }

    if (index != p_node->ptr_in)
    {
      p_data[i]       = p_node->p_buf[index];
      p_node->ptr_out = next_index;
    }
    else
    {
      err_code = ERR_EMPTY;
      break;
    }
  }


  return err_code;
}

err_code_t qbufferReadByte(qbuffer_node_t *p_node, uint8_t *p_data)
{
  return qbufferRead(p_node, p_data, 1);

}




