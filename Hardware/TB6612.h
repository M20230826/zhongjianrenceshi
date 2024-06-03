#ifndef __TB6612_H 
#define __TB6612_H 

#include "sys.h"

// 函数声明
void TB6612_Init(int arr, int psc); // 初始化 TB6612 电机驱动
void SetPWM(int pwm);               // 设置 PWM 值

#endif // __TB6612_H
