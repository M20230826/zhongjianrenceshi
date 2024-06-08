#ifndef __LED_H
#define __LED_H

#include "sys.h"

// LED引脚定义
// PAout(4) 是一个宏，直接操作GPIOA引脚4的输出寄存器
#define LED PAout(4)  

// 函数原型
void LED_Init(void);  // 初始化LED
void Led_Flash(u16 time); // 以指定频率闪烁LED

#endif
