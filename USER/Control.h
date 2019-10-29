#ifndef _Control_H_
#define _Control_H_

#include "stm32f10x.h"

u8 Friction_Speed(int T_PWM);
void AX_12_Active(float target);
void Terrace_Lock(void);
void Terrace_Unlock(void);
void AX_12_Reset(void);

#endif
