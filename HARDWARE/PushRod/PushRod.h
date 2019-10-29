#ifndef _PUSHER_H_
#define _PUSHER_H_

void PWM_Pusher(int pwm);
void Active_Pusher(float dangle,float dangle_l1,float Kp,float Kd,float Ki,float integral);
float Integral_Pusher(float integral,float dangle);
float Push_Reset(void);
void Push_Enable(void);
void Push_Disable(void);

#endif
