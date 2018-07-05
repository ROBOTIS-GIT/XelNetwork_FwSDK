/*
 * cmdif.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: HanCheol Cho
 */

#ifndef CMDIF_H_
#define CMDIF_H_



#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>

#define CMDIF_CMD_STR_MAX             16
#define CMDIF_CMD_LIST_MAX            _HW_DEF_CMDIF_LIST_MAX
#define CMDIF_CMD_HIS_MAX             4
#define CMDIF_CMD_BUF_LENGTH          128





typedef struct
{
  char  cmd_str[CMDIF_CMD_STR_MAX];
  int (*cmd_func)(int argc, char **argv);
} cmdif_cmd_node_t;


typedef struct
{
  bool               init;
  uint8_t            err_code;
  uint8_t            ch;
  uint32_t           baud;
  uint16_t           index;
  bool               exit;

  cmdif_cmd_node_t  *node;

  char  read_buffer[CMDIF_CMD_BUF_LENGTH];
  char *p_read_buffer;

  int32_t  his_count;
  int32_t  his_index;
  char     his_buff[CMDIF_CMD_HIS_MAX][CMDIF_CMD_BUF_LENGTH];
} cmdif_cmd_t;



 bool cmdifInit(void);
 void cmdifClear(void);
 bool cmdifIsInit(void);
 void cmdifBegin(uint8_t ch, uint32_t baud);
 void cmdifLoop(void);
 void cmdifAdd(char *cmd_str, int (*p_func)(int argc, char **argv));
 bool cmdifMain(void);

 void cmdifPrintf(const char *fmt, ...);
 void cmdifPrint(const char *str);
 void cmdifPutch(char data);
 uint8_t  cmdifGetch(void);
 uint32_t cmdifRxAvailable(void);


#ifdef __cplusplus
}
#endif



#endif /* CMDIF_H_ */
