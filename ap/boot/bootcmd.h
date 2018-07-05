/*
 *  boot.h
 *
 *  Created on: 2016. 5. 14.
 *      Author: Baram
 */

#ifndef BOOTCMD_H
#define BOOTCMD_H


#ifdef __cplusplus
 extern "C" {
#endif

#include "ap_def.h"


#ifdef _USE_BOOTCMD

#include "hw_def.h"
#include "ap.h"

#define FW_TAG_TYPE_A 0

typedef struct
{
  uint32_t type;
  uint32_t address;
  uint32_t length;
  uint16_t crc;
} fw_tag_type_a_t;


void bootCmdInit(void);
void bootCmdProcess(cmd_t *p_cmd);
err_code_t checkFw(uint32_t type, uint32_t address);


#endif /* _USE_BOOTCMD */


#ifdef __cplusplus
}
#endif


#endif /* BOOTCMD_H */
