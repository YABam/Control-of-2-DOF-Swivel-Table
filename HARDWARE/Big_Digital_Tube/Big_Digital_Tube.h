#ifndef _BIG_DIGITAL_TUBE_H_
#define _BIG_DIGITAL_TUBE_H_

#include "sys.h"
#include "stm32f10x.h"

#define _PORT_F_  //选择要使用的端口

#ifdef _PORT_A_
#define LED_Out(n) PAout(n)
#define LED_Port GPIOA
#define LED_IO_RCC RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)
#endif

#ifdef _PORT_B_
#define LED_Out(n) PBout(n)
#define LED_Port GPIOB
#define LED_IO_RCC RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)
#endif

#ifdef _PORT_C_
#define LED_Out(n) PCout(n)
#define LED_Port GPIOC
#define LED_IO_RCC RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)
#endif

#ifdef _PORT_D_
#define LED_Out(n) PDout(n)
#define LED_Port GPIOD
#define LED_IO_RCC RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE)
#endif

#ifdef _PORT_E_
#define LED_Out(n) PEout(n)
#define LED_Port GPIOE
#define LED_IO_RCC RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE)
#endif

#ifdef _PORT_F_
#define LED_Out(n) PFout(n)
#define LED_Port GPIOF
#define LED_IO_RCC RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE)
#endif

#define LED_Display_Test 0x7F
#define LED_Display_Error 0x40

void LED_Display_Number(u8 input);
void LED_Pin_Control(u8 input);

void me_LED_Display_Test(void);
void me_LED_Display_Error(void);

void Big_LED_Init(void);

#endif
