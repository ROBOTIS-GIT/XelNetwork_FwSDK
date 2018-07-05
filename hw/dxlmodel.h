/*
 * dxlmodel.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef DXLMODEL_H_
#define DXLMODEL_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_DXLMODEL


#define DXLMODEL_MAX_MODEL_LIST     5
#define DXLMODEL_MAX_MODEL_GROUP    4




typedef struct
{
  uint16_t addr;
  uint16_t length;
} dxl_model_addr_t;



typedef struct
{
  uint16_t   model_number;
  const char model_string[32];
  uint8_t    model_group;
  uint16_t   position_p_gain;
} dxl_model_list_t;

typedef struct
{
  uint32_t  res;
  int32_t   max;
  int32_t   zero;
  int32_t   min;
  float     fangle_max;
  float     fangle_min;
} dxl_model_position_t;

typedef struct
{
  bool              bulkread_support;
  dxl_model_addr_t  bulkread_range;

  bool              indirect_enable;
  dxl_model_addr_t  indirect_addr_range;
  dxl_model_addr_t  indirect_data_range;

  dxl_model_addr_t  model_number;
  dxl_model_addr_t  fw_version;
  dxl_model_addr_t  torque;
  dxl_model_addr_t  id;
  dxl_model_addr_t  baud;
  dxl_model_addr_t  goal_position;
  dxl_model_addr_t  present_position;

  dxl_model_addr_t  position_p_gain;

  dxl_model_position_t position_info;

} dxl_model_ctable_t;


typedef struct
{
  dxl_model_ctable_t *p_ctable;
} dxl_model_group_t;



bool dxlmodelInit(void);

dxl_model_ctable_t *dxlmodelGetCtableByID(uint8_t group_id);
dxl_model_ctable_t *dxlmodelGetCtableByModelNumber(uint16_t model_number);

dxl_model_list_t *dxlmodelGetModelByModelNumber(uint16_t model_number);


#endif /* _USE_HW_DXLMODEL */

#ifdef __cplusplus
}
#endif

#endif /* DXLMODEL_H_ */
