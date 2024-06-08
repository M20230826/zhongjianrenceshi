#ifndef __EXTI_H
#define __EXTI_H

#include "sys.h"

// 定义宏 INT，用于读取 PA12 引脚的状态
#define INT PAin(12)   // PA12引脚，用于连接 MPU6050 的中断信号

// 函数声明：初始化外部中断
void MiniBalance_EXTI_Init(void);	// 微型平衡车外部中断初始化

#endif
















