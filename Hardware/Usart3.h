#ifndef __USART3_H
#define __USART3_H 
#include "sys.h"	  	

extern u8 Usart3_Receive; // 声明一个外部变量，用于接收USART3的数据

// 函数声明
void uart3_init(u32 bound); // 初始化USART3，设置波特率
void USART3_IRQHandler(void); // USART3中断服务程序

#endif
