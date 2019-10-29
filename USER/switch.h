//#define _Debug_Mode_ //开关是否返回角度数据
//#define _LED_Display_ //开关是否在小LED数码管显示角度
////#define _Training_ //开关训练模式
//#define _Forecast_ //开关预测模式

////#define _BLUE_ //开关使用蓝场数据
//#define _RED_ //开关使用红场数据

////#define Data_Report //编译显示数据警告
#ifndef _SWITCH_H_
#define _SWITCH_H_

#include "stm32f10x.h"

u8 g_SWITCH_Debug_Mode_Flag = 1;
u8 g_SWITCH_LED_Display_Flag = 1;

u8 g_SWITCH_Training_Flag = 0;
u8 g_SWITCH_Forecast_Flag = 1;

//u8 g_SWITCH_BLUE_Flag = 0;
u8 g_SWITCH_RED_Flag = 1;

#endif
