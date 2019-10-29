//#include "switch.h"

#include "stm32f10x.h"
#include "Remote_Control.h"
#include "set_command.h"

//======================================
extern u8 g_SWITCH_Debug_Mode_Flag;
extern u8 g_SWITCH_LED_Display_Flag;

extern u8 g_SWITCH_Training_Flag;
extern u8 g_SWITCH_Forecast_Flag;

//extern u8 g_SWITCH_BLUE_Flag;
extern u8 g_SWITCH_RED_Flag;
//======================================

extern Shoot_DataStructure g_ShootData[5][7];
extern Shoot_DataStructure g_DeffenceData;
extern Shoot_DataStructure g_HitBall[7];

//蓝场数据
Shoot_DataStructure g_ShootData_Blue[5][7] =
{
	{//位置1
		{10,0,1050},{11,1,1055},{12,2,1060},{13,3,1065},{14,4,1070},{15,5,1075},{16,6,1080}
	},
	{//位置2
		{ 17,7,1050 },{ 18,8,1055 },{ 19,9,1060 },{ 20,10,1065 },{ 21,11,1070 },{ 22,12,1075 },{ 23,13,1080 }
	},
	{//位置3
		{ 14.33,-25,1142 },{ 20.88,-12.1,1155 },{ 19.70,2.28,1137 },{ 17.1,23.24,1133 },{ 12.48,41.24,1138 },{ 13.41,8.6,1391 },{ 34.95,-3.57,1041 }
	},
	{//位置4
		{ 11,-1,1050 },{ 12,-2,1055 },{ 13,-3,1060 },{ 14,-4,1065 },{ 15,-5,1070 },{ 16,-6,1075 },{ 17,-7,1080 }
	},
	{//位置5
		{ 18,-8,1050 },{ 19,-9,1055 },{ 20,-10,1060 },{ 21,-11,1065 },{ 22,-12,1070 },{ 23,-13,1075 },{ 24,-14,1080 }
	},
};

Shoot_DataStructure g_HitBall_Blue[7] = 
{
	{ 14.95,-25,1142 },{ 18.08,-12.85,1126 },{ 19.35,1.79,1134 },{ 15.38,22.15,1124 },{ 14.92,43.45,1146 },{ 13.44,2.25,1330 },{ 35.75,-4.47,1041 }
};

Shoot_DataStructure g_DeffenceData_Blue = {1.9,-8.6,1131};




//红场数据
Shoot_DataStructure g_ShootData_Red[5][7] =
{
	{//位置1
		{10,0,1050},{11,1,1055},{12,2,1060},{13,3,1065},{14,4,1070},{15,5,1075},{16,6,1080}
	},
	{//位置2
		{ 17,7,1050 },{ 18,8,1055 },{ 19,9,1060 },{ 20,10,1065 },{ 21,11,1070 },{ 22,12,1075 },{ 23,13,1080 }
	},
	{//位置3
		{ 17.3,-24.7,1176 },{ 19.24,-13.6,1144 },{ 20.28,2.09,1140 },{ 15.68,22.23,1121 },{ 16.21,42.15,1154 },{ 13.29,6.05,1360 },{ 35.58,-5.2,1040 }
	},
	{//位置4
		{ 11,-1,1050 },{ 12,-2,1055 },{ 13,-3,1060 },{ 14,-4,1065 },{ 15,-5,1070 },{ 16,-6,1075 },{ 17,-7,1080 }
	},
	{//位置5
		{ 18,-8,1050 },{ 19,-9,1055 },{ 20,-10,1060 },{ 21,-11,1065 },{ 22,-12,1070 },{ 23,-13,1075 },{ 24,-14,1080 }
	},
};

Shoot_DataStructure g_HitBall_Red[7] = 
{
	{ 18.48,-25,1169 },{ 19.72,-12.05,1144 },{ 20.61,2.89,1140 },{ 14.29,21.75,1121 },{ 15.1,42.55,1154 },{ 12.41,6.55,1400 },{ 35.93,-6.47,1040 }
};

Shoot_DataStructure g_DeffenceData_Red = {1.75,-6.9,1131};



u8 g_Robot_Position = 0;
u8 g_Target_Number = 0;

u8 g_DeffenceFlag = 0;

//Training
extern u8 g_Training_TP;
extern u8 g_Training_TS;

//LED_Display
extern u8 g_Display_State_Flag;

u8 Remote_Decode(unsigned char * input, u8 len, float *pDataA, float *pDataB, int *pDataC)
/*
	return 255 Error/None
	return 1 Read Real angle
	return 2 Output delta
	return 3 Shoot!!
	return 4 Deffence
	return 111 Reset Push
	return 127 Read Robot Position
*/
{
	float Set_Mul = 0.1;

	*pDataA = 0;
	*pDataB = 0;
	*pDataC = 0;

	if (input[0] != 0xAA)//如果头不是0xAA
	{
		return 255;
	}

	if(input [1] == 0xA5)//如果1是A5
	{
		return Command_Decode(input,pDataA,pDataB,pDataC);
	}
	
	//下面开始数据解析
	if (input[1] == 0x00 && input[2] == 0x01)
	{
		return 3;
	}
	//手调 按上加数字
	if (input[2] == 0x08)//上
	{
		if (input[1] == 0x40)//6,俯仰加 1011 1111 1101 abcd 0000 0010 0100 0000
		{
			*pDataA = Set_Mul;
		}
		else if (input[1] == 0x80)//7,俯仰减 0000 0001 1000 0000
		{
			*pDataA = -Set_Mul;
		}
		else if (input[1] == 0x04)//2,偏航减 0010 0000 0000 0100
		{
			*pDataB = -Set_Mul;
		}
		else if (input[1] == 0x10)//4,偏航加 0000 1000  0001 0000
		{
			*pDataB = Set_Mul;
		}
		else if (input[1] == 0x02)//1,摩擦论加速
		{
			*pDataC = 1;
		}
		else if(input[1] == 0x20)//5，摩擦论减速
		{
			*pDataC = -1;
		}
		else if(input[1] == 0x08)//3，reset推杆
		{
			return 111;
		}
		else
		{
			return 255;
		}
		return 2;
	}
	//读底盘位置
	if(input[2] == 0x40)//右 0000 0010 0100 0000
	{
		if(input[1] == 0x02)//1 0100 0000 0000 0010
		{
			g_Robot_Position = 1;
		}
		else if(input[1] == 0x04)//2 0010 0000 0000 0100
		{
			g_Robot_Position = 2;
		}
		else if(input[1] == 0x08)//3 0001 0000 0000 1000
		{
			if(g_Robot_Position == 6 && g_Target_Number == 0)
			{
				g_Robot_Position = 3;
//				g_Target_Number = 3;
				g_Target_Number = 1;
				*pDataA = g_ShootData[2][0].angle_A;
				*pDataB = g_ShootData[2][0].angle_C;
				*pDataC = g_ShootData[2][0].Speed;
				return 1;
			}
			else
			{
				g_Robot_Position = 3;
				if(g_Target_Number >=1 && g_Target_Number <= 7)
				{
					*pDataA = g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].angle_A;
					*pDataB = g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].angle_C;
					*pDataC = g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].Speed;
					return 1;
				}
			}
		}
		else if(input[1] == 0x10)//4 0000 1000 0001 0000
		{
			g_Robot_Position = 4;
		}
		else if(input[1] == 0x20)//5 0000 0100 0010 0000
		{
			g_Robot_Position = 5;
		}
		else if(input[1] == 0x40)//6 0000 0010 0100 0000
		{
			g_Robot_Position = 6;//取弹
			if(g_SWITCH_RED_Flag == 0)//蓝场
			{
				*pDataA = 20;
				*pDataB = -35;
				*pDataC = 1050;
			}
			else//红场
			{
				*pDataA = 20;
				*pDataB = 40;
				*pDataC = 1050;
			}
			return 1;
		}
		else if(input[1] == 0x80)//7 0000 0001 1000 0000
		{
			g_Target_Number = 0;//初始化
			g_Robot_Position = 7;//回零
			*pDataA = 20;
			*pDataB = 0;
			*pDataC = 800;
			return 1;
		}
		else
		{
			return 255;
		}
		return 127;
	}
	//修正底盘位置
	if(input[2] == 0x20)//左 0000 0100 0010 0000
	{
		if(input[1] == 0x02)//1 0100 0000 0000 0010
		{
			g_Robot_Position = 1;
		}
		else if(input[1] == 0x04)//2
		{
			g_Robot_Position = 2;
		}
		else if(input[1] == 0x08)//3
		{
			g_Robot_Position = 3;
		}
		else if(input[1] == 0x10)//4
		{
			g_Robot_Position = 4;
		}
		else if(input[1] == 0x20)//5
		{
			g_Robot_Position = 5;
		}
		else if(input[1] == 0x40)//6
		{
			g_Robot_Position = 6;//取弹
		}
		else if(input[1] == 0x80)//7
		{
			g_Target_Number = 0;//初始化
			g_Robot_Position = 7;//回零
			*pDataA = 20;
			*pDataB = 0;
			*pDataC = 800;
			return 1;
		}
		else
		{
			return 255;
		}
		return 127;
	}
	//读数据
	else if (input[2] == 0x10)//下 0000 1000 0001 0000
	{
		if(g_SWITCH_LED_Display_Flag == 1)
		{
			g_Display_State_Flag = 1;
		}
		if (g_Robot_Position > 5)
		{
			return 255;
		}
		else
		{
			if (input[1] == 0x02)//1
			{
				g_Target_Number = 1;
			}
			else if (input[1] == 0x04)//2
			{
				g_Target_Number = 2;
			}
			else if (input[1] == 0x08)//3
			{
				g_Target_Number = 3;
			}
			else if (input[1] == 0x10)//4
			{
				g_Target_Number = 4;
			}
			else if (input[1] == 0x20)//5
			{
				g_Target_Number = 5;
			}
			else if (input[1] == 0x40)//6
			{
				g_Target_Number = 6;
			}
			else if (input[1] == 0x80)//7
			{
				g_Target_Number = 7;
			}
			else
			{
				return 255;
			}

			//读存档
			*pDataA = g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].angle_A;
			*pDataB = g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].angle_C;
			*pDataC = g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].Speed;
			return 1;
		}
	}
	else if(input[2] == 0)//[2]没有按
	{
		if(g_SWITCH_Training_Flag == 1)
		{
			if(input[3] == 0x80)//防守按键
			{
				if(g_Training_TP == 0 && g_Training_TS == 0)
				{
					if(input[1] == 0x02)//1,拍照
					{
						g_Training_TP = 1;
					}
					else if(input[1] == 0x20)//5,确认命中
					{
						g_Training_TS = 1;
					}
					return 255;
				}
				else
				{
					return 255;
				}
			}		
		}
		else
		{
			if(input[3] == 0x80)//防守
			{
				g_Target_Number = 7;
				g_DeffenceFlag = 1;
				*pDataA = g_DeffenceData.angle_A;
				*pDataB = g_DeffenceData.angle_C;
				*pDataC = g_DeffenceData.Speed;
				return 4;
			}
		}
		if(input[3] == 0x40)//打球
		{
			if (g_Robot_Position > 5)
			{
				return 255;
			}
			else
			{
				if (input[1] == 0x02)//1
				{
					g_Target_Number = 1;
				}
				else if (input[1] == 0x04)//2
				{
					g_Target_Number = 2;
				}
				else if (input[1] == 0x08)//3
				{
					g_Target_Number = 3;
				}
				else if (input[1] == 0x10)//4
				{
					g_Target_Number = 4;
				}
				else if (input[1] == 0x20)//5
				{
					g_Target_Number = 5;
				}
				else if (input[1] == 0x40)//6
				{
					g_Target_Number = 6;
				}
				else if (input[1] == 0x80)//7
				{
					g_Target_Number = 7;
				}
				else
				{
					return 255;
				}

				//读存档
				*pDataA = g_HitBall[g_Target_Number - 1].angle_A;
				*pDataB = g_HitBall[g_Target_Number - 1].angle_C;
				*pDataC = g_HitBall[g_Target_Number - 1].Speed;
				return 1;
			}
		}
		else
		{
			if(g_SWITCH_LED_Display_Flag == 1)
			{		
				g_Display_State_Flag = 0;
			}
			
			if(g_SWITCH_Training_Flag == 1)
			{
				g_Training_TP = 0;
				g_Training_TS = 0;
			}
			return 255;
		}
	}
	else
	{
		#ifdef _LED_Display_
		g_Display_State_Flag = 0;
		#endif
		return 255;
	}
}

u8 Angle_Check(float tolerance,float angle_x,float angle_z,u8 Robot_Position,u8 Target_Number)
{
	if(Robot_Position <1 || Robot_Position > 5 || Target_Number<1 || Target_Number > 7)
	{
		return 0;
	}
	else
	{
		if(angle_x < g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].angle_A - tolerance
			|| angle_x > g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].angle_A + tolerance)//x角度出界
		{
			return 0;
		}
		else if(angle_z < g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].angle_C - tolerance
			|| angle_z > g_ShootData[g_Robot_Position - 1][g_Target_Number - 1].angle_C + tolerance)//z角度出界
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}
