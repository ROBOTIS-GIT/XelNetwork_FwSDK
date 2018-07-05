/*
 *  flash.h
 *
 *  Created on: 2016. 5. 14.
 *      Author: Baram
 */

#ifndef FLASH_H
#define FLASH_H


#ifdef __cplusplus
 extern "C" {
#endif



#include "hw_def.h"

#ifdef _USE_HW_FLASH

bool flashInit(void);

err_code_t flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length);
err_code_t flashRead(uint32_t addr, uint8_t *p_data, uint32_t length);
err_code_t flashErase(uint32_t addr, uint32_t length);

#endif /* _USE_HW_FLASH */


#ifdef __cplusplus
}
#endif


#endif /* FLASH_H */
