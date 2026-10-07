#include "uart4.h"
#include <stdarg.h>
#include <stdio.h>

#if EN_UART4_RX
// UART4接收缓冲区
uint8_t UART4_RX_BUF[UART4_REC_LEN];//文本数据包 [F/B/L/R, speed/angle, time(非必须)]
// 接收状态标志
// bit15: 接收完成标志
// bit14: 接收到0x0d
// bit13~0: 接收到的有效字节数
uint8_t UART4_FLAG = 0;
uint8_t UART4_OK = 0;
uint8_t UART4_WHERE = 0;

PARSE parse = {0};
#endif

/**
 * 函数名：uart4_send_char
 * 功能：发送单个字符
 * 参数：ch - 要发送的字符
 * 返回值：无
 */
void uart4_send_char(uint8_t ch) {
    while ((UART4->SR & 0X40) == 0); // 等待发送完成
    UART4->DR = ch;
}

/**
 * 函数名：uart4_printf
 * 功能：格式化输出到UART4
 * 参数：format - 格式字符串
 * 返回值：无
 */
void uart4_printf(const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    uart4_send_string((uint8_t*)buffer);
}

/**
 * 函数名：uart4_init
 * 功能：初始化UART4
 * 参数：bound - 波特率
 * 返回值：无
 */
void uart4_init(u32 bound) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能GPIOC和UART4时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    // 配置PC10和PC11为UART4功能
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4); // PC10作为UART4_TX
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4); // PC11作为UART4_RX

    // 配置GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 配置UART4
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure);

    // 使能UART4
    USART_Cmd(UART4, ENABLE);

#if EN_UART4_RX
    // 使能接收中断
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

    // 配置NVIC
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

/**
 * 函数名：uart4_send_string
 * 功能：发送字符串
 * 参数：str - 要发送的字符串
 * 返回值：无
 */
void uart4_send_string(uint8_t *str) {
    while (*str) {
        while ((UART4->SR & 0X40) == 0); // 等待发送完成
        UART4->DR = *str++;
    }
}

#if EN_UART4_RX
/**
 * 函数名：UART4_IRQHandler
 * 功能：UART4中断处理函数
 * 参数：无
 * 返回值：无
 */
void UART4_IRQHandler(void) {
    uint8_t Res;

    if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) {
        Res = USART_ReceiveData(UART4);
		
		if(Res == '[' && UART4_FLAG == 0)
		{
			UART4_FLAG = 1;
			UART4_WHERE = 0;
		}
		else if(UART4_FLAG == 1)
		{
			if(Res == ']')
			{
				UART4_FLAG = 0;
				UART4_RX_BUF[UART4_WHERE] = '\0';
				UART4_OK = 1;
			}
			else
			{
				if(UART4_WHERE < UART4_REC_LEN - 1)
				{
					UART4_RX_BUF[UART4_WHERE] = Res;
					UART4_WHERE ++ ;
				}
			}
		}
    }
}

void parse_packet(uint8_t *str)
{
    parse.cmd = 0;
    parse.val1 = 0;
    parse.val2 = 0;
	
	if (str == NULL || str[0] == '\0') return;
	
    char *token;
    int count = 0;
    token = strtok((char*)str, ",");

    while (token != NULL)
    {
        count++;

        if(count == 1)
        {
			if(token[0] != 'F' && token[0] != 'B' && token[0] != 'L' && token[0] != 'R') return;
            parse.cmd = token[0];
        }
        else if(count == 2)
        {
            parse.val1 = atoi(token);
        }
        else if(count == 3)
        {
            parse.val2 = atoi(token);
        }

        token = strtok(NULL, ",");
    }
}

void car_GO()
{
	if(parse.cmd == 'L')
	{
		need_speed = 0;
		need_angle = AngleZ  + parse.val1;
	}
	else if(parse.cmd == 'R')
	{
		need_speed = 0;
		need_angle = AngleZ  - parse.val1;
	}
	else if(parse.cmd == 'F')
	{
		need_angle = AngleZ;
		need_speed = parse.val1;
		move_tim  =  parse.val2 * 200;
	}
	else if(parse.cmd == 'B')
	{
		need_speed = -parse.val1;
		need_angle = AngleZ ;
		move_tim  =  parse.val2 * 200;
	}
}
#endif
