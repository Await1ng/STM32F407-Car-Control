#include "motor.h"

void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);

    // AIN1 (PE10), AIN2 (PE8)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // BIN1 (PB1), BIN2 (PC5)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 初始化为停止状态
    GPIO_ResetBits(GPIOE, GPIO_Pin_8 | GPIO_Pin_10);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    GPIO_ResetBits(GPIOC, GPIO_Pin_5);
}

void Motor_A_Control(int16_t speed)
{
    if (speed > 0)
    {
        // 正转
        GPIO_ResetBits(GPIOE, GPIO_Pin_10);  // AIN1 = 0
        GPIO_SetBits(GPIOE, GPIO_Pin_8);  // AIN2 = 1
        TIM9_PWM_Set(speed, TIM_GetCapture2(TIM9));
    }
    else if (speed < 0)
    {
        // 反转
        GPIO_SetBits(GPIOE, GPIO_Pin_10);  // AIN1 = 1
        GPIO_ResetBits(GPIOE, GPIO_Pin_8);  // AIN2 = 0
        TIM9_PWM_Set(-speed, TIM_GetCapture2(TIM9));
    }
    else
    {
        // 停止
        GPIO_ResetBits(GPIOE, GPIO_Pin_8 | GPIO_Pin_10);
        TIM9_PWM_Set(0, TIM_GetCapture2(TIM9));
    }
}

void Motor_B_Control(int16_t speed)
{
    if (speed > 0)
    {
        // 正转
        GPIO_SetBits(GPIOB, GPIO_Pin_1);  // BIN1 = 1
        GPIO_ResetBits(GPIOC, GPIO_Pin_5);  // BIN2 = 0
        TIM9_PWM_Set(TIM_GetCapture1(TIM9), speed);
    }
    else if (speed < 0)
    {
        // 反转
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);  // BIN1 = 0
        GPIO_SetBits(GPIOC, GPIO_Pin_5);  // BIN2 = 1
        TIM9_PWM_Set(TIM_GetCapture1(TIM9), -speed);
    }
    else
    {
        // 停止
        GPIO_ResetBits(GPIOB, GPIO_Pin_1);
        GPIO_ResetBits(GPIOC, GPIO_Pin_5);
        TIM9_PWM_Set(TIM_GetCapture1(TIM9), 0);
    }
}

void Motor_Stop(void)
{
    Motor_A_Control(0);
    Motor_B_Control(0);
}

void Motor_Differential_Control(int16_t linear_speed, int16_t angular_speed)
{
    int16_t left_speed, right_speed;
    
    left_speed = linear_speed - angular_speed;
    right_speed = linear_speed + angular_speed;
    
    if (left_speed > 2000) left_speed = 2000;
    if (left_speed < -2000) left_speed = -2000;
    if (right_speed > 2000) right_speed = 2000;
    if (right_speed < -2000) right_speed = -2000;
    
    Motor_A_Control(left_speed);  
    Motor_B_Control(right_speed);
}

void Motor_Turn(int16_t speed, uint8_t direction)
{
    if (direction == 0) // 向左转
    {
        Motor_A_Control(-speed);
        Motor_B_Control(speed);
    }
    else if (direction == 1) // 向右转
    {
        Motor_A_Control(speed);
        Motor_B_Control(-speed);
    }
}
