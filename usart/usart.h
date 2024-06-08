#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"

// 函数: uart_init
// 描述: 初始化UART通信，设置指定的波特率。
// 输入: bound - 通信的波特率
// 返回: 无
void uart_init(u32 bound);

#endif
