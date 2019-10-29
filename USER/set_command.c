#include "stm32f10x.h"
#include "set_command.h"

u8 Receive_Command_Flag = 0x0A;

u8 Command_Decode(u8 * input,float *p_set_x,float *p_set_z,int *p_set_s)
{
	*p_set_x = 0;
	*p_set_z = 0;
	*p_set_s = 0;
	
	if(input[0] != 0xAA)
	{
		return 255;
	}
	
	if(input[1] != 0xA5)
	{
		return 255;
	}
	
	if(input[2] != Receive_Command_Flag)
	{
		return 255;
	}
	else
	{
		if(Receive_Command_Flag == 0x05)
		{
			Receive_Command_Flag = 0x0A;
		}
		else
		{
			Receive_Command_Flag = 0x05;
		}
		*p_set_x = input[3] + input[4] * 0.01;
		*p_set_z = (input[5] + input[6] * 0.01) - 45;
		*p_set_s = (input[7] * 100 + input[8]);
	}
	return 100;
}
