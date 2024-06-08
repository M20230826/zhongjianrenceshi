#ifndef __DELAY_H
#define __DELAY_H

#include "sys.h"

// 函数: delay_init
// 描述: 初始化延时函数
// 输入: 无
// 返回: 无
void delay_init(void);

// 函数: delay_ms
// 描述: 毫秒级延时
// 输入: nms - 延时的毫秒数
// 返回: 无
void delay_ms(u16 nms);

// 函数: delay_us
// 描述: 微秒级延时
// 输入: nus - 延时的微秒数
// 返回: 无
void delay_us(u32 nus);

#endif
