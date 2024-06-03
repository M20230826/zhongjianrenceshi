#ifndef __LED_H 
#define __LED_H 

#include "sys.h"

// 函数声明
void LED_Init(void); // 初始化 LED

// 宏定义
#define LED PAout(4) // 定义 LED 控制宏

#endif // __LED_H
