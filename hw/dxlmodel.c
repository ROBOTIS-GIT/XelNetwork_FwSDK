/*
 * dxlmodel.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */





#include "dxlmodel.h"

#ifdef _USE_HW_DXLMODEL

#include <stdarg.h>
#include <stdbool.h>
#include "hw.h"






//-- Internal Variables
//



//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//




const dxl_model_list_t dxl_model_list[] =
{
  { _DEF_DXL_MODEL_AX12A,       "AX12A",    _DEF_DXL_AX, 0    },
  { _DEF_DXL_MODEL_AX18A,       "AX18A",    _DEF_DXL_AX, 0    },
  { _DEF_DXL_MODEL_AX12W,       "AX12W",    _DEF_DXL_AX, 0    },
  { _DEF_DXL_MODEL_XL430_W250,  "XL430",    _DEF_DXL_XL, 640  },
  { _DEF_DXL_MODEL_XM430_W210,  "XM430",    _DEF_DXL_XL, 800  },
  { _DEF_DXL_MODEL_XM430_W350,  "XM430",    _DEF_DXL_XL, 800  },
  { _DEF_DXL_MODEL_XH430_W210,  "XH430",    _DEF_DXL_XL, 900  },
  { _DEF_DXL_MODEL_XH430_W350,  "XH430",    _DEF_DXL_XL, 900  },
  { _DEF_DXL_MODEL_2XL430,      "2XL430",   _DEF_DXL_XL, 640  },
  { 0xFFFF, "Unknown", 0},
};


const dxl_model_ctable_t dxl_ax =
{
  .bulkread_support   = false,
  .indirect_enable    = false,

  .position_info.res  = 1024,
  .position_info.max  = 1023,
  .position_info.zero = 512,
  .position_info.min  = 0,
  .position_info.fangle_max =  150.,
  .position_info.fangle_min = -150.,


  .model_number.addr   = 0,
  .model_number.length = 2,

  .fw_version.addr   = 2,
  .fw_version.length = 1,

  .torque.addr   = 24,
  .torque.length = 1,

  .id.addr   = 3,
  .id.length = 1,

  .baud.addr   = 4,
  .baud.length = 1,

  .goal_position.addr   = 30,
  .goal_position.length = 2,

  .present_position.addr   = 36,
  .present_position.length = 2
};


const dxl_model_ctable_t dxl_xl =
{
  .bulkread_support      = true,
  .bulkread_range.addr   = 126,
  .bulkread_range.length = 10,

  .indirect_enable            = true,
  .indirect_addr_range.addr   = 168,
  .indirect_addr_range.length = 28,
  .indirect_data_range.addr   = 224,
  .indirect_data_range.length = 28,

  .position_info.res  = 4096,
  .position_info.max  = 4095,
  .position_info.zero = 2048,
  .position_info.min  = 0,
  .position_info.fangle_max =  180.,
  .position_info.fangle_min = -180.,


  .position_p_gain.addr = 84,
  .position_p_gain.length = 2,

  .model_number.addr   = 0,
  .model_number.length = 2,

  .fw_version.addr   = 6,
  .fw_version.length = 1,

  .torque.addr   = 64,
  .torque.length = 1,

  .id.addr   = 7,
  .id.length = 1,

  .baud.addr   = 8,
  .baud.length = 1,

  .goal_position.addr   = 116,
  .goal_position.length = 4,

  .present_position.addr   = 132,
  .present_position.length = 4

};

dxl_model_group_t dxl_model_group[DXLMODEL_MAX_MODEL_GROUP];


bool dxlmodelInit(void)
{
  dxl_model_group[_DEF_DXL_AX].p_ctable = (dxl_model_ctable_t *)&dxl_ax;
  dxl_model_group[_DEF_DXL_XL].p_ctable = (dxl_model_ctable_t *)&dxl_xl;

  return true;
}

dxl_model_ctable_t *dxlmodelGetCtableByID(uint8_t group_id)
{
  return dxl_model_group[group_id].p_ctable;
}

dxl_model_ctable_t *dxlmodelGetCtableByModelNumber(uint16_t model_number)
{
  uint16_t i;
  uint8_t group_id;
  dxl_model_ctable_t *p_ret = NULL;

  i = 0;

  while(1)
  {
    if (dxl_model_list[i].model_number == 0xFFFF)
    {
      break;
    }
    if (dxl_model_list[i].model_number == model_number)
    {
      group_id = dxl_model_list[i].model_group;
      p_ret = dxl_model_group[group_id].p_ctable;
    }
    i++;
  }
  return p_ret;
}

dxl_model_list_t *dxlmodelGetModelByModelNumber(uint16_t model_number)
{
  uint16_t i;
  dxl_model_list_t *p_ret = NULL;

  i = 0;

  while(1)
  {
    if (dxl_model_list[i].model_number == 0xFFFF)
    {
      break;
    }
    if (dxl_model_list[i].model_number == model_number)
    {
      p_ret = (dxl_model_list_t *)&dxl_model_list[i];
    }
    i++;
  }

  return p_ret;
}

#endif /* _USE_HW_DXLMODEL */
