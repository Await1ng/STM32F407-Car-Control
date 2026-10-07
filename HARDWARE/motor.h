#ifndef __MOTOR_H
#define __MOTOR_H

#include "headfile.h"

void Motor_Init(void);
void Motor_A_Control(int16_t speed);
void Motor_B_Control(int16_t speed);
void Motor_Stop(void);

void Motor_Differential_Control(int16_t linear_speed, int16_t angular_speed);
void Motor_Turn(int16_t speed, uint8_t direction);

#endif
