/*
 * xel_net_struct.hpp
 *
 *  Created on: Aug 23, 2018
 *      Author: kei
 */

#ifndef XEL_NET_STRUCT_HPP_
#define XEL_NET_STRUCT_HPP_

#include "stdint.h"
#include "xel/xel_data_struct.h"

namespace XelNetwork
{

enum DataDirection
{
  SEND = 0,
  RECEIVE
};

enum XelStatus
{
  NOT_CONNECTTED = 0,
  NEW_CONNECTION,
  LOST_CONNECTION,
  RUNNING
};

enum DataType
{
  BOOLEAN = 0,
  CHAR,
  INT8,
  UINT8,
  INT16,
  UINT16,
  INT32,
  UINT32,
  INT64,
  UINT64,
  FLOAT32,
  FLOAT64,
  VECTOR3,
  QUATERNION,
  POINT,
  TWIST,
  IMU,
  MILLIS,
  LED,
  ANALOG
};

typedef struct XelHeader
{
  XelNetwork::DataType data_type;
  uint32_t             data_get_interval_hz;
  char                 data_name[32];         //ROS2 topic name
  uint8_t              msg_type;              //ros2 message type (topic, service, action..)
  uint16_t             data_addr;
  uint8_t              data_length;
} __attribute__((packed)) XelHeader_t;

struct XelStatus_t
{
  XelStatus  previous;
  XelStatus  current;
  bool       flag_changed;
  bool       flag_get_data;
};

struct XelDDS_t
{
  char    msg_name[32];         //DDS topic name
  uint8_t msg_type;             //ROS2 message type (topic, service, action..)
  void(*p_callback_func)(void* msg, void* arg);
};

typedef struct XelInfo
{
  uint8_t                   xel_id;
  XelHeader_t               header;
  struct XelStatus_t        status;
  uint8_t                   data[128];
  XelNetwork::DataDirection data_direction;
  struct XelDDS_t           dds;
} XelInfo_t;




} //XelNetwork

#endif /* XEL_NET_STRUCT_HPP_ */
