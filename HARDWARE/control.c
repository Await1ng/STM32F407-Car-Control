#include "control.h"

// 全局变量
float L_output = 0;
float R_output = 0;
float L_error = 0;
float L_last_error = 0;
float R_error = 0;
float R_last_error = 0;
float error_speed = 0;



//速度环限幅
int SPEEDMIN_OUT = -2000;
int SPEEDMAX_OUT =  2000;
//角度环限幅
int ANGLEMAX_OUT =  60;
int ANGLEMIN_OUT = -60;



// 左电机PID
float Lspeedkp 		= 0.15f; 
float Lspeedki 		= 0.35f;	

// 右电机PID
float Rspeedkp 		= 0.15f; 
float Rspeedki 		= 0.35f;	

// 角度闭环PID
float angle_kp 		= 1.0f;  
float angle_kd 		= 3.5f;   
float target_angle 	= 0.0f;   
volatile float angle_output 	= 0.0f;   
float angle_error0 	= 0.0f;   
float angle_error1  = 0.0f; 


float L_error0 = 0;
float L_error1 = 0;
float R_error0 = 0;
float R_error1 = 0;

volatile float need_speed  = 0.0f;  // 前正后负
volatile float need_angle  = 0.0f;  // 右正左负
volatile int32_t move_tim = 0;


// 左电机增量式PI控制
void Lspeedpid(float target, float actual, float kp, float ki)
{
    L_error0 = target - actual;
    L_output += kp * (L_error0 - L_error1) + ki * L_error0;
    L_error1 = L_error0;

    
     if (L_output > SPEEDMAX_OUT)
         L_output = SPEEDMAX_OUT;
     else if (L_output < SPEEDMIN_OUT)
         L_output = SPEEDMIN_OUT;

    Motor_A_Control(L_output);


}

// 右电机增量式PI控制
void Rspeedpid(float target, float actual, float kp, float ki)
{
    R_error0 = target - actual;
    R_output += kp * (R_error0 - R_error1) + ki * R_error0;
    R_error1 = R_error0;


    if (R_output > SPEEDMAX_OUT)
        R_output = SPEEDMAX_OUT;
    else if (R_output < SPEEDMIN_OUT)
        R_output = SPEEDMIN_OUT;


    Motor_B_Control(R_output);
}

// 控制初始化
void Control_Init(void)
{
    // 初始化变量
    L_output = 0;
    R_output = 0;
    L_error = 0;
    L_last_error = 0;
    R_error = 0;
    R_last_error = 0;
    angle_output = 0;
    angle_error0 = 0;
    angle_error1 = 0;
	need_speed = 0;
	need_angle = 0;
	move_tim = 0;
}

// 角度PID控制
void Angle_PID(float target, float actual, float kp, float kd)
{
    angle_error1 = angle_error0;
    angle_error0 = target - actual;
    
    angle_output = kp * angle_error0 + kd * (angle_error0 - angle_error1);
    
    if (angle_output > ANGLEMAX_OUT) angle_output = ANGLEMAX_OUT;
    if (angle_output < ANGLEMIN_OUT) angle_output = ANGLEMIN_OUT;
}

// 差速控制
void Differential_Control(float target_speed, float error_speed)
{
    float left_speed, right_speed;
    
    // 计算左右电机速度
    left_speed = target_speed - error_speed;
    right_speed = target_speed + error_speed;
    
    // 限制速度范围
    if (left_speed > 2000) left_speed = 2000;
    if (left_speed < -2000) left_speed = -2000;
    if (right_speed > 2000) right_speed = 2000;
    if (right_speed < -2000) right_speed = -2000;
    
    // 速度闭环控制
    Lspeedpid(left_speed, Encoder_L_Speed, Lspeedkp, Lspeedki);
    Rspeedpid(right_speed, Encoder_R_Speed, Rspeedkp, Rspeedki);
    
}
