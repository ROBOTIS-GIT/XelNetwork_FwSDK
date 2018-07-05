/*
 *  reset.h
 *
 *  Created on: 2016. 7. 8.
 *      Author: Baram
 */

#ifndef RESET_H
#define RESET_H


#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"

#ifdef _USE_HW_RESET


bool resetInit(void);
uint8_t resetGetStatus(void);
void resetRunWdgReset(void);
void resetRunSoftReset(void);
void resetClearFlag(void);

#endif /* _USE_HW_RESET */

#ifdef __cplusplus
}
#endif


#endif /* RESET_H */
