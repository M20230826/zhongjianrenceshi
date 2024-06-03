#ifndef __DELAY_H
#define __DELAY_H 			   

#include "sys.h"  

// 初始化延时函数
void delay_init(void);

// 毫秒级延时函数
void delay_ms(u16 nms);

// 微秒级延时函数
void delay_us(u32 nus);

#endif // __DELAY_H
