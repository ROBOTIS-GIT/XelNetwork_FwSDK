/*
 * xel_net_data_struct.h
 *
 *  Created on: Aug 28, 2018
 *      Author: kei
 */

#ifndef XEL_NET_DATA_STRUCT_H_
#define XEL_NET_DATA_STRUCT_H_

#include <stdint.h>

typedef struct Bool
{
  bool data;
}Bool_t;

typedef struct Char
{
  char data;
}Char_t;

typedef struct Int8
{
  int8_t data;
}Int8_t;

typedef struct Int16
{
  int16_t data;
}Int16_t;

typedef struct Int32
{
  int32_t data;
}Int32_t;

typedef struct Int64
{
  int64_t data;
}Int64_t;

typedef struct Uint8
{
  uint8_t data;
}Uint8_t;

typedef struct Uint16
{
  uint16_t data;
}Uint16_t;

typedef struct Uint32
{
  uint32_t data;
}Uint32_t;

typedef struct Uint64
{
  uint64_t data;
}Uint64_t;

typedef struct Float32
{
  float data;
}Float32_t;

typedef struct Float64
{
  double data;
}Float64_t;

typedef struct Imu
{
  double quat_x;
  double quat_y;
  double quat_z;
  double quat_w;
  double acc_x;
  double acc_y;
  double acc_z;
  double gyro_x;
  double gyro_y;
  double gyro_z;
}Imu_t;

#endif /* XEL_NET_DATA_STRUCT_H_ */
