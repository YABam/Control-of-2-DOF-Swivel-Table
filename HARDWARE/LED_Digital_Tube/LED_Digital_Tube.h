#ifndef _LED_DIGITAL_TUBE_
#define _LED_DIGITAL_TUBE_

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"

//#define Max7219_pinCLK PAout(6)
//#define Max7219_pinCS PAout(5)
//#define Max7219_pinDIN PAout(4)

void Write_Max7219_byte(u8 DATA);
void Write_Max7219(u8 address, u8 data);
void Init_MAX7219(void);
void LED_Display(float Adata,float Cdata);
void MAX7219_Ready(void);
void LED_Msg_Display(u8 cmd,int Adata,int Bdata);

#endif
