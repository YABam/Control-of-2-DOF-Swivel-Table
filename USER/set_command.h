#ifndef _SET_COMMAND_H_
#define _SET_COMMAND_H_

#include "stm32f10x.h"

u8 Command_Decode(u8 * input,float *p_set_x,float *p_set_z,int *p_set_s);

#endif
