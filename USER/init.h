#ifndef _INIT_H_
#define _INIT_H_

#include "stm32f10x.h"
#include "string.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "exti.h"
#include "PushRod.h"
#include "led.h"
#include "driver.h"
#include "AX12.h" 
#include "timer.h"
#include "lcd.h"
#include "Math.h"

void Stm32_Init(void);
void Z_Rotate_Init(void);
void Control_Clock_Init(void);
u16 Decode(u8 len, u8 UART4_RX_BUF[], float *p_set_x_angle, float *p_set_z_angle);
void Control(float set_angle,int T_PWM);

#endif
