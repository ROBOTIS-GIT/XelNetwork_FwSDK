/*
 * qbuffer.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: HanCheol Cho
 */

#ifndef QBUFFER_H_
#define QBUFFER_H_



#ifdef __cplusplus
 extern "C" {
#endif





 typedef struct
 {
   err_code_t err_code;
   uint32_t   ptr_in;
   uint32_t   ptr_out;
   uint32_t   length;
   uint8_t   *p_buf;
 } qbuffer_node_t;



 bool qbufferInit(void);
 err_code_t qbufferCreate(qbuffer_node_t *p_node, uint32_t length);
 uint32_t   qbufferAvailable(qbuffer_node_t *p_node);
 err_code_t qbufferWrite(qbuffer_node_t *p_node, uint8_t *p_data, uint32_t length);
 err_code_t qbufferWriteByte(qbuffer_node_t *p_node, uint8_t data);
 err_code_t qbufferRead(qbuffer_node_t *p_node, uint8_t *p_data, uint32_t length);
 err_code_t qbufferReadByte(qbuffer_node_t *p_node, uint8_t *p_data);


#ifdef __cplusplus
}
#endif



#endif /* QBUFFER_H_ */
