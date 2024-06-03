#ifndef __SYS_H
#define __SYS_H

#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <DataScope_dp.h>
#include <Key.h>
#include <LED.h>
#include <MotorEncoder.h>
#include <Oled.h>
#include <TB6612.h>
#include <Usart.h>
#include <Usart3.h>
#include <Show.h>
#include <Delay.h>



// 车辆类型宏定义
#define Diff_Car 0 // 双轮差动AGV

// 外部变量声明
extern u8 Mode, BLUETOOTH_Mode, ELE_Line_Patrol_Mode; // 小车模式
extern int Voltage; // 电压
extern float Velocity, Velocity_Set, Angle, Angle_Set, Turn, Bluetooth_Velocity;
extern u8 PID_Send, Flash_Send, APP_RX, APP_Flag; // APP相关标志
extern int Encoder_Left, Encoder_Right; // 编码器数据
extern float Velocity_KP, Velocity_KI, Velocity_KD, Velocity_KJ; // PID参数
extern u8 Flag_Way, Flag_Show, Flag_Stop, Flag_Next; // 标志位


// 0: 不支持UCOS
// 1: 支持UCOS
#define SYSTEM_SUPPORT_OS 0 // 系统是否支持UCOS

// JTAG模式设置
#define JTAG_SWD_DISABLE 0x02
#define SWD_ENABLE 0x01
#define JTAG_SWD_ENABLE 0x00

// 位带操作，实现在51单片机上模拟GPIO操作
// 具体实现参考《CM3权威指南》87页~92页
// IO地址映射宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

// IO口操作宏定义
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) // 0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) // 0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) // 0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) // 0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) // 0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) // 0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) // 0x40011E0C

#define GPIOA_IDR_Addr (GPIOA_BASE + 8) // 0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8) // 0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8) // 0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8) // 0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8) // 0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8) // 0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8) // 0x40011E08

// IO口操作，只有一个IO口
// 确保n的值小于16
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // 输出
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)  // 输入

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // 输出
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)  // 输入

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // 输出
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)  // 输入

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // 输出
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)  // 输入

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) // 输出
#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr, n)  // 输入

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) // 输出
#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr, n)  // 输入

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) // 输出
#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr, n)  // 输入

// 函数声明
void WFI_SET(void);       // 执行WFI指令
void INTX_DISABLE(void);  // 关闭所有中断
void INTX_ENABLE(void);   // 开启所有中断
void MSR_MSP(u32 addr);   // 设置主堆栈指针
void Stm32_Clock_Init(u8 PLL); // 初始化时钟
void JTAG_Set(u8 mode);   // 设置JTAG模式
void OLED_SHOW(void);     // 显示OLED

#endif // __SYS_H
