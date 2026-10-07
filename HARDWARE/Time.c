#include "Time.h"

/**
  * 函    数：定时器4初始化
  * 参    数：无
  * 返 回 值：无
  */
void Timer4_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    /*初始化定时器*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 42000 - 1;  // 5ms中断
    TIM_TimeBaseInitStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
    
    /*使能定时器更新中断*/
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    /*配置NVIC*/
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
    
    /*启动定时器*/
    TIM_Cmd(TIM4, ENABLE);
}

