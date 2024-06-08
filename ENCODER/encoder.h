#ifndef __ENCODER_H
#define __ENCODER_H

#include <sys.h>	 

#define ENCODER_TIM_PERIOD (u16)(65535)   // 编码器定时器周期设定为65535，适用于F103系列的16位定时器

// 函数原型
void Encoder_Init_TIM3(void);  // 初始化TIM3用于编码器接口模式
void Encoder_Init_TIM4(void);  // 初始化TIM4用于编码器接口模式
int Read_Encoder(u8 TIMX);      // 读取指定定时器的编码器计数值
void TIM4_IRQHandler(void);     // TIM4中断服务函数
void TIM3_IRQHandler(void);     // TIM3中断服务函数

#endif
