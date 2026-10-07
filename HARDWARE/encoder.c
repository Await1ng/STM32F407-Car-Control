#include "encoder.h"

int16_t Encoder_L_Count = 0;
int16_t Encoder_R_Count = 0;

int16_t Encoder_L_Speed = 0;
int16_t Encoder_R_Speed = 0;



void Encoder_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    // 左电机编码器 TIM8 (PC6, PC7)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);

    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM8, &TIM_ICInitStructure);

    TIM_Cmd(TIM8, ENABLE);

    // 右电机编码器 TIM5 (PA0, PA1)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);

    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);

    TIM_ICInit(TIM5, &TIM_ICInitStructure);

    TIM_Cmd(TIM5, ENABLE);
}

int16_t Encoder_L_Read(void)
{
    Encoder_L_Count = (int16_t)TIM8->CNT;
    TIM8->CNT = 0;
    return Encoder_L_Count;
}

int16_t Encoder_R_Read(void)
{
    Encoder_R_Count = (int16_t)TIM5->CNT;
    TIM5->CNT = 0;
    return Encoder_R_Count;
}

void Encoder_Clear(void)
{
    TIM8->CNT = 0;
    TIM5->CNT = 0;
    Encoder_L_Count = 0;
    Encoder_R_Count = 0;
}
