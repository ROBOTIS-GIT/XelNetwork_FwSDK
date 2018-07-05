/*
 * i2c.h
 *
 *  Created on: 2017. 4. 10.
 *      Author: D.ggavy
 */

#ifndef I2C_H_
#define I2C_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_I2C


#define i2cRead  i2cReadBytes
#define i2cWrite i2cWriteBytes



#define I2C_MAX_CH      DRV_I2C_MAX_CH



bool    i2cInit(void);


err_code_t i2cReadByte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data);
err_code_t i2cReadBytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length);

err_code_t i2cWriteByte (uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t data);
err_code_t i2cWriteBytes(uint8_t ch, uint16_t dev_addr, uint16_t reg_addr, uint8_t *p_data, uint32_t length);

void     i2cSetTimeout(uint8_t ch, uint32_t timeout);
uint32_t i2cGetTimeout(uint8_t ch);

void     i2cClearErrCount(uint8_t ch);
uint32_t i2cGetErrCount(uint8_t ch);


#endif /* _USE_HW_I2C */

#ifdef __cplusplus
 }
#endif

#endif /* I2C_H_ */
