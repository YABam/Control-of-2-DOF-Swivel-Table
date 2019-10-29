#ifndef _REMOTE_CONTROL_H_
#define _REMOTE_CONTROL_H_

//#include "stm32f10x.h"
typedef unsigned char u8;

typedef struct Shoot_DataStructure
{
	float angle_A;
	float angle_C;
	int Speed;
} Shoot_DataStructure;

u8 Remote_Decode(unsigned char * input, u8 len, float *pDataA, float *pDataB, int *pDataC);

u8 Angle_Check(float tolerance,float angle_x,float angle_z,u8 Robot_Position,u8 Target_Number);

#endif
