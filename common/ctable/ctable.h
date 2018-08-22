/*
 * ctable.h
 *
 *  Created on: 2017. 7. 18.
 *      Author: Baram
 */

#ifndef CTABLE_H_
#define CTABLE_H_



#ifdef __cplusplus
 extern "C" {
#endif










#define _ATTR_EEPROM        (1<<1)
#define _ATTR_RAM           (1<<2)
#define _ATTR_VIR           (1<<3)
#define _ATTR_RD            (1<<4)
#define _ATTR_WR            (1<<5)
#define _ATTR_CONST         (1<<6)



#define _UPDATE_NONE        0x00
#define _UPDATE_RD          (1<<1)
#define _UPDATE_WR          (1<<2)
#define _UPDATE_INIT        (1<<3)
#define _UPDATE_RESET       (1<<4)
#define _UPDATE_NOT_RESET   (1<<5)
#define _UPDATE_STARTUP     (1<<6)




typedef struct
{
  void *p_ctable;

  uint16_t index;
  uint8_t  mode;
  uint16_t addr;
  uint8_t *p_mem;
  uint16_t length;

} ctable_param_t;


typedef struct ctable_t_ ctable_t;

typedef struct ctable_attribute_t_
{
  uint16_t  address;
  uint16_t  length;
  uint16_t  count;
  uint8_t   attr;
  int32_t   init_data;
  uint8_t   update;
  uint16_t  module;
  void     (*update_func)(uint32_t addr, uint8_t mode, uint16_t update_addr, uint8_t *p_data, uint16_t update_length);
} ctable_attribute_t;


typedef struct ctable_t_
{
  bool                init;
  uint8_t            *p_mem;
  ctable_attribute_t *p_attr;
  uint16_t            attr_length;
  int8_t              data_type;
  uint32_t            mem_length;
} ctable_t;



bool ctableInit(ctable_t *p_ctable, uint8_t *p_mem, uint32_t mem_length, ctable_attribute_t *p_attr);
void ctableUpdate(ctable_t *p_ctable);
void ctableReset(ctable_t *p_ctable);
void ctableRemoveReset(ctable_t *p_ctable, uint16_t addr);

uint8_t ctableRead(ctable_t *p_ctable, uint16_t addr, uint8_t *p_data, uint16_t length);
uint8_t ctableWrite(ctable_t *p_ctable, uint16_t addr, uint8_t *p_data, uint16_t length);

uint8_t ctableWriteUpdate(ctable_t *p_ctable, uint16_t addr, uint8_t *p_data, uint16_t length, uint8_t mode);
uint8_t ctableWriteUpdate(ctable_t *p_ctable, uint16_t addr, uint8_t *p_data, uint16_t length, uint8_t mode);

#ifdef __cplusplus
}
#endif



#endif /* DXL_H_ */
