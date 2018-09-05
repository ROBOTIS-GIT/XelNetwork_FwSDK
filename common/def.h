/*
 * def.h
 *
 *  Created on: 2017. 2. 14.
 *      Author: HanCheol Cho
 */

#ifndef DEF_H_
#define DEF_H_



#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

#include "error_code.h"
#include "cmdif/cmdif.h"



#define _DEF_UART1          0
#define _DEF_UART2          1
#define _DEF_UART3          2
#define _DEF_UART4          3
#define _DEF_UART5          4
#define _DEF_UART6          5

#define _DEF_CAN1           0
#define _DEF_CAN2           1
#define _DEF_CAN_BAUD_125K  0
#define _DEF_CAN_BAUD_250K  1
#define _DEF_CAN_BAUD_500K  2
#define _DEF_CAN_BAUD_1M    3
#define _DEF_CAN_STD        0
#define _DEF_CAN_EXT        1

#define _DEF_LEFT           0
#define _DEF_RIGHT          1

#define _DEF_INPUT            0
#define _DEF_INPUT_PULLUP     1
#define _DEF_INPUT_PULLDOWN   2
#define _DEF_OUTPUT           3
#define _DEF_OUTPUT_PULLUP    4
#define _DEF_OUTPUT_PULLDOWN  5

#define _DEF_EXTI_RISING    0
#define _DEF_EXTI_FALLING   1
#define _DEF_EXTI_BOTH      2
#define _DEF_EVT_RISING     3
#define _DEF_EVT_FALLING    4
#define _DEF_EVT_BOTH       5

#define _DEF_FLOAT          2
#define _DEF_HIGH           1
#define _DEF_LOW            0


#define _DEF_TIMER1         0
#define _DEF_TIMER2         1
#define _DEF_TIMER3         2
#define _DEF_TIMER4         3

#define _DEF_GPIO1           0
#define _DEF_GPIO2           1
#define _DEF_GPIO3           2
#define _DEF_GPIO4           3


#define _DEF_I2C1           0
#define _DEF_I2C2           1
#define _DEF_I2C3           2
#define _DEF_I2C4           3

#define _DEF_LED1           0
#define _DEF_LED2           1
#define _DEF_LED3           2
#define _DEF_LED4           3
#define _DEF_LED5           4
#define _DEF_LED6           5
#define _DEF_LED7           6
#define _DEF_LED8           7

#define _DEF_DXL1           0
#define _DEF_DXL2           1
#define _DEF_DXL3           2
#define _DEF_DXL4           3
#define _DEF_DXL5           4

#define _DEF_BUTTON1        0
#define _DEF_BUTTON2        1
#define _DEF_BUTTON3        2
#define _DEF_BUTTON4        3
#define _DEF_BUTTON5        4

#define _DEF_ADC1           0
#define _DEF_ADC2           1
#define _DEF_ADC3           2
#define _DEF_ADC4           3
#define _DEF_ADC5           4
#define _DEF_ADC6           5
#define _DEF_ADC7           6
#define _DEF_ADC8           7

#define _DEF_DAC_CH1        0
#define _DEF_DAC_CH2        1

#define _DEF_PWM1           0
#define _DEF_PWM2           1
#define _DEF_PWM3           2
#define _DEF_PWM4           3
#define _DEF_PWM5           4
#define _DEF_PWM6           5
#define _DEF_PWM7           6
#define _DEF_PWM8           7

#define _DEF_I2S1           0
#define _DEF_I2S2           1

#define _DEF_SPI1           0
#define _DEF_SPI2           1
#define _DEF_SPI3           2
#define _DEF_SPI4           3
#define _DEF_SPI5           4
#define _DEF_SPI6           5
#define _DEF_SPI7           6
#define _DEF_SPI8           7

#define _DEF_EXTI1          0
#define _DEF_EXTI2          1
#define _DEF_EXTI3          2
#define _DEF_EXTI4          3
#define _DEF_EXTI5          4
#define _DEF_EXTI6          5
#define _DEF_EXTI7          6
#define _DEF_EXTI8          7
#define _DEF_EXTI9          8
#define _DEF_EXTI10         9
#define _DEF_EXTI11         10
#define _DEF_EXTI12         11
#define _DEF_EXTI13         12
#define _DEF_EXTI14         13
#define _DEF_EXTI15         14
#define _DEF_EXTI16         15

#define _DEF_RESET_POWER    0
#define _DEF_RESET_PIN      1
#define _DEF_RESET_WDG      2
#define _DEF_RESET_SOFT     3


#define _DEF_DXL_BAUD_9600      0
#define _DEF_DXL_BAUD_57600     1
#define _DEF_DXL_BAUD_115200    2
#define _DEF_DXL_BAUD_1000000   3
#define _DEF_DXL_BAUD_2000000   4
#define _DEF_DXL_BAUD_3000000   5
#define _DEF_DXL_BAUD_4000000   6
#define _DEF_DXL_BAUD_4500000   7


#define _DEF_DXL_AX             0
#define _DEF_DXL_RX             1
#define _DEF_DXL_MX             2
#define _DEF_DXL_XL             3



#define _DEF_DXL_MODEL_AX12A        12
#define _DEF_DXL_MODEL_AX18A        18
#define _DEF_DXL_MODEL_AX12W        300
#define _DEF_DXL_MODEL_XL320        350
#define _DEF_DXL_MODEL_XL430_W250   1060
#define _DEF_DXL_MODEL_XM430_W210   1030
#define _DEF_DXL_MODEL_XM430_W350   1020
#define _DEF_DXL_MODEL_XH430_W210   1010
#define _DEF_DXL_MODEL_XH430_W350   1000
#define _DEF_DXL_MODEL_2XL430       1090


#define _DEF_OLLO_IO_NONE                       0
#define _DEF_OLLO_IO_MOTOR_SPEED                1
#define _DEF_OLLO_IO_SERVO_POSITION             2
#define _DEF_OLLO_IO_SERVO_SPEED                3
#define _DEF_OLLO_IO_IR                         4
#define _DEF_OLLO_IO_DMS                        5
#define _DEF_OLLO_IO_TOUCH                      6
#define _DEF_OLLO_IO_LED                        7
#define _DEF_OLLO_IO_USERDEVICE                 8
#define _DEF_OLLO_IO_TEMPERATURE                9
#define _DEF_OLLO_IO_ULTRASONIC                 10
#define _DEF_OLLO_IO_MAGNETIC                   11
#define _DEF_OLLO_IO_MOTION_DETECTION           12
#define _DEF_OLLO_IO_COLOR                      13
#define _DEF_OLLO_IO_MOISTURE                   14
#define _DEF_OLLO_IO_MOISTURE_TEMPERATURE       15
#define _DEF_OLLO_IO_BRIGHTNESS                 16
#define _DEF_OLLO_IO_LED_PWM_CH_P               17
#define _DEF_OLLO_IO_LED_PWM_CH_M               18
#define _DEF_OLLO_IO_MAX                        19


#define _DEF_OLLO_MOTOR_WHEEL_MODE              0
#define _DEF_OLLO_MOTOR_SERVO_MODE              1

#define _DEF_DIR_CW                             0
#define _DEF_DIR_CCW                            1

#define _DEF_OLLO_TYPE_MOTOR                    0
#define _DEF_OLLO_TYPE_SENSOR                   1


#define _DEF_TYPE_S08                           0
#define _DEF_TYPE_U08                           1
#define _DEF_TYPE_S16                           2
#define _DEF_TYPE_U16                           3
#define _DEF_TYPE_S32                           4
#define _DEF_TYPE_U32                           5
#define _DEF_TYPE_F32                           6


typedef uint32_t  err_code_t;




typedef void (*voidFuncPtr)(void);



typedef union
{
  uint8_t  u8Data[4];
  uint16_t u16Data[2];
  uint32_t u32Data;

  int8_t   s8Data[4];
  int16_t  s16Data[2];
  int32_t  s32Data;

  uint8_t  u8D;
  uint16_t u16D;
  uint32_t u32D;

  int8_t   s8D;
  int16_t  s16D;
  int32_t  s32D;
} data_t;


typedef struct
{
  data_t data;
  bool   ret;
} data_ret_t;


typedef struct
{
  uint32_t  ptr_in;
  uint32_t  ptr_out;
  uint32_t  length;
  uint8_t  *p_buf;
} ring_buf_t;


typedef struct
{
  uint32_t addr;
  uint32_t end;
  uint32_t length;
} flash_attr_t;



#define PI              3.1415926535897932384626433832795
#define HALF_PI         1.5707963267948966192313216916398
#define TWO_PI          6.283185307179586476925286766559
#define DEG_TO_RAD      0.017453292519943295769236907684886
#define RAD_TO_DEG      57.295779513082320876798154814105
#define EULER           2.718281828459045235360287471352

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#ifndef max
#define max(a,b) (((a) (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef map
#define map(value, in_min, in_max, out_min, out_max) ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif

#ifndef byte
#define byte uint8_t
#endif

#ifndef radians
#define radians(deg) ((deg)*DEG_TO_RAD)
#endif
#ifndef degress
#define degrees(rad) ((rad)*RAD_TO_DEG)
#endif
#ifndef sq
#define sq(x) ((x)*(x))
#endif


#endif /* DEF_H_ */
