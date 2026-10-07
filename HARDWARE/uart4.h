#ifndef __UART4_H__
#define __UART4_H__

#include "headfile.h"

// UART4接收缓冲区大小
#define UART4_REC_LEN    200

// 使能UART4接收
#define EN_UART4_RX    1

// 函数声明
void uart4_init(u32 bound);
void uart4_send_char(uint8_t ch);
void uart4_send_string(uint8_t *str);
void uart4_printf(const char *format, ...);

#if EN_UART4_RX

typedef struct {
    char cmd;          // F/B/L/R
    int val1;          // 速度/角度
    int val2;          // 时间（可选）
} PARSE;

extern uint8_t UART4_RX_BUF[UART4_REC_LEN];
extern uint8_t UART4_FLAG;
extern uint8_t UART4_OK;
extern PARSE parse;
void parse_packet(uint8_t *str);
void car_GO(void);
#endif

#endif // __UART4_H__
