//#define _Debug_Mode_ //�����Ƿ񷵻ؽǶ�����
//#define _LED_Display_ //�����Ƿ���СLED�������ʾ�Ƕ�
////#define _Training_ //����ѵ��ģʽ
//#define _Forecast_ //����Ԥ��ģʽ

////#define _BLUE_ //����ʹ����������
//#define _RED_ //����ʹ�ú쳡����

////#define Data_Report //������ʾ���ݾ���
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
