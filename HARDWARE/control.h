#ifndef __CONTROL_H
#define __CONTROL_H

#include "headfile.h"

extern float L_output;
extern float R_output;
extern float L_error;
extern float L_last_error;
extern float R_error;
extern float R_last_error;
extern float error_speed;



// 左电机PID
extern float Lspeedkp; 
extern float Lspeedki;

// 右电机PID
extern float Rspeedkp; 
extern float Rspeedki; 

// 角度闭环PID
extern float angle_kp; 
extern float angle_kd; 
extern float target_angle; 
extern volatile float angle_output; 

extern volatile float need_speed;  // 前正后负
extern volatile float need_angle;  // 左正右负
extern volatile int32_t move_tim;


// 函数声明
void Control_Init(void);
void Lspeedpid(float target, float actual, float kp, float ki);
void Rspeedpid(float target, float actual, float kp, float ki);
void Angle_PID(float target, float actual, float kp, float kd);
void Differential_Control(float target_speed, float error_speed);

#endif
