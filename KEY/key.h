#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

// 按键引脚定义
// PAin(5) 是一个宏，直接读取GPIOA引脚5的输入状态
#define KEY PAin(5)

// 函数原型
void KEY_Init(void);          // 初始化按键GPIO
u8 click_N_Double(u8 time);   // 扫描单击和双击
u8 click(void);               // 扫描单击
u8 Long_Press(void);          // 扫描长按
u8 select(void);              // 根据按键输入选择模式

#endif
