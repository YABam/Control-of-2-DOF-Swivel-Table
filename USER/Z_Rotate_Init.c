//Z_Rotate_Init();
#include "driver.h"
#include "led.h"
#include "init.h"

extern float g_Position;

void Z_Rotate_Init()
{
	Motor_Init_Zero();//准备回零
	LED1 = 0;
	Motor_V(-90);
	while(g_Position > -40)
	{
		//等待限位开关触发
	}
	
	Motor_V(0);
	Motor_Init_Pos();
	delay_ms(500);
	Motor_LA(0);
	LED1 = 1;
	
	g_Position = 0;

}
