#ifndef __TIMER_H
#define __TIMER_H

#include <sys.h>	 // 包含系统头文件，确保能够使用sys.h中定义的内容

/**
  * @brief 初始化定时器2通道2的输入捕获功能
  * @param arr 自动重装载值
  * @param psc 时钟分频系数
  * @retval 无
  */
void TIM2_Cap_Init(u16 arr, u16 psc);

/**
  * @brief 超声波接收回波函数
  * @param 无
  * @retval 无
  */
void Read_Distane(void);

/**
  * @brief 定时器2中断处理函数
  * @param 无
  * @retval 无
  */
void TIM2_IRQHandler(void);

#endif
