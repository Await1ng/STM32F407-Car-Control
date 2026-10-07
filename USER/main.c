#include "headfile.h"

uint16_t count1 = 0;uint16_t count2 = 0;
uint16_t count3 = 0;uint16_t count4 = 0;
float hum = 0.0, temp = 0.0;


void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
        count1++;count2++;
		count3++;count4++;

        if (count1 >= 2) // 10ms获取一次角度
        {
	        Angle_Update();
            Angle_PID(need_angle,  AngleZ, angle_kp, angle_kd);
			count1 = 0;
        }

		if (count2 >= 1) // 每5ms执行一次速度控制
        {
             Encoder_L_Speed = -Encoder_L_Read();
             Encoder_R_Speed =  Encoder_R_Read();
            // // 使用差速控制
             Differential_Control(need_speed, angle_output);

            count2 = 0;
        }
		
		if(count3 >= 1)
		{
			if(move_tim > 0)move_tim -= 5;
			if(move_tim <= 0)need_speed = 0;
			count3 = 0;
		}

        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}


int main(void)
{
    SystemInit();
    delay_init(168);
    OLED_Init();

    uart4_init(19200); // 初始化 UART4
    TIM9_PWM_Init(999, 83); // PWM 初始化
    Motor_Init(); // 电机初始化
    Encoder_Init(); // 编码器初始化
    Control_Init(); // 控制初始化
    delay_ms(500);
	MPU6050_Init();
    Angle_Init();
    Timer4_Init();
    OLED_Clear();
	OLED_Update();
    while(1)
    {
		if(UART4_OK == 1)
		{
			parse_packet(UART4_RX_BUF);
			car_GO();
			memset(UART4_RX_BUF,0,sizeof(UART4_RX_BUF));
			UART4_OK = 0;
		}
		OLED_Printf(0,8,OLED_6X8,"speed:%.3f",need_speed);
		OLED_Printf(0,16,OLED_6X8,"angle:%.3f",need_angle);
		OLED_Printf(0,32,OLED_6X8,"Lsp:%5d",Encoder_L_Speed);
		OLED_Printf(0,40,OLED_6X8,"Rsp:%5d",Encoder_R_Speed);
		OLED_Update();
    }
}
