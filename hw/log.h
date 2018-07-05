/*
 * log.h
 *
 *  Created on: 2017. 8. 25.
 *      Author: baram
 */

#ifndef LOG_H_
#define LOG_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_LOG


bool logInit(void);

void logOn(void);
void logOff(void);
void logPrintf( const char *fmt, ...);


#endif /* _USE_HW_LOG */

#ifdef __cplusplus
}
#endif

#endif /* LOG_H_ */
