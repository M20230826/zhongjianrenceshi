#ifndef __USART_H
#define __USART_H

#include "stdio.h"	
#include "sys.h" 

// 定义 USART 接收缓冲区长度
#define USART_REC_LEN 200  // 最大接收字节数为 200
#define EN_USART1_RX 1     // 使能（1）/禁用（0）USART1 接收

// 外部变量声明
extern u8 USART_RX_BUF[USART_REC_LEN]; // 接收缓冲区, 最大接收 USART_REC_LEN 个字节
extern u16 USART_RX_STA;               // 接收状态标志

// 函数声明
void uart_init(u32 bound); // 初始化 USART

#endif // __USART_H
