#ifndef __PWM_H
#define __PWM_H

#include "headfile.h"

void TIM9_PWM_Init(u32 arr, u32 psc);
void TIM9_PWM_Set(u16 compare1, u16 compare2);

#endif
