#ifndef __SYS_H
#define __SYS_H

#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "oled.h"
#include "usart.h"
#include "usart3.h"
#include "adc.h"
#include "timer.h"
#include "motor.h"
#include "encoder.h"
#include "ioi2c.h"
#include "mpu6050.h"
#include "show.h"
#include "exti.h"
#include "DataScope_DP.h"
#include "control.h"
#include "filter.h"
#include "KF.h"
#include "stmflash.h"
#include "pstwo.h"
#include "Lidar.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// 0 - 不支持UCOS系统
// 1 - 支持UCOS系统
#define SYSTEM_SUPPORT_UOS 0 // 配置系统是否支持UCOS

// 位带操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

// GPIO输出数据寄存器地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) // 0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) // 0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) // 0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) // 0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) // 0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) // 0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) // 0x40011E0C

// GPIO输入数据寄存器地址映射
#define GPIOA_IDR_Addr (GPIOA_BASE + 8) // 0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8) // 0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8) // 0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8) // 0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8) // 0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8) // 0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8) // 0x40011E08
// GPIO输入输出宏定义
// 确保 n 的值小于16

// GPIOA
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) // 输出
#define PAin(n)  BIT_ADDR(GPIOA_IDR_Addr, n) // 输入

// GPIOB
#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) // 输出
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n) // 输入

// GPIOC
#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) // 输出
#define PCin(n)  BIT_ADDR(GPIOC_IDR_Addr, n) // 输入

// GPIOD
#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) // 输出
#define PDin(n)  BIT_ADDR(GPIOD_IDR_Addr, n) // 输入

// GPIOE
#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) // 输出
#define PEin(n)  BIT_ADDR(GPIOE_IDR_Addr, n) // 输入

// GPIOF
#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) // 输出
#define PFin(n)  BIT_ADDR(GPIOF_IDR_Addr, n) // 输入

// GPIOG
#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) // 输出
#define PGin(n)  BIT_ADDR(GPIOG_IDR_Addr, n) // 输入
/////////////////////////////////////////////////////////////////////////////
// Ex_NVIC_Config专用定义
#define GPIO_A 0
#define GPIO_B 1
#define GPIO_C 2
#define GPIO_D 3
#define GPIO_E 4
#define GPIO_F 5
#define GPIO_G 6 

// JTAG模式设置
#define JTAG_SWD_DISABLE 0X02
#define SWD_ENABLE       0X01
#define JTAG_SWD_ENABLE  0X00

// 车辆类型宏定义
#define Diff_Car 0 // 双轮差动AGV

// 外部变量声明
extern u8 Mode, BLUETOOTH_Mode, ELE_Line_Patrol_Mode; // 小车模式
extern int Flag_Direction; // 方向标志位
extern u8 Flag_Way, Flag_Show, Flag_Stop, Flag_Next; // 标志位
extern int Encoder_Left, Encoder_Right; // 编码器数据
extern float Velocity, Velocity_Set, Angle, Angle_Set, Turn, Bluetooth_Velocity;
extern int Motor_A, Motor_B, Target_A, Target_B; // 电机和舵机控制
extern int Voltage; // 电压
extern u8 delay_50, delay_flag; // 延时标志
extern float Velocity_KP, Velocity_KI, Velocity_KD, Velocity_KJ; // PID参数
extern u8 PID_Send, Flash_Send, APP_RX, APP_Flag; // APP相关标志
extern int Sensor_Left, Sensor_Middle, Sensor_Right, Sensor; // 传感器数据
extern u16 PID_Parameter[10], Flash_Parameter[10]; // PID和Flash参数
extern u8 Way_Angle;                             //»ñÈ¡½Ç¶ÈµÄËã·¨£¬1£ºËÄÔªÊý  2£º¿¨¶ûÂü  3£º»¥²¹ÂË²¨
extern float Angle_Balance; //Æ½ºâÇã½Ç Æ½ºâÍÓÂÝÒÇ ×ªÏòÍÓÂÝÒÇ

/////////////////////////////////////////////////////////////////  
void Stm32_Clock_Init(u8 PLL); // 时钟初始化函数
void Sys_Soft_Reset(void); // 系统软复位函数
void Sys_Standby(void); // 系统待机函数
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset); // 设置NVIC向量表
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group); // 配置NVIC优先级分组
void MY_NVIC_Init(u8 NVIC_PreemptionPriority, u8 NVIC_SubPriority, u8 NVIC_Channel, u8 NVIC_Group); // 初始化NVIC
void Ex_NVIC_Config(u8 GPIOx, u8 BITx, u8 TRIM); // 配置外部中断
void JTAG_Set(u8 mode); // 设置JTAG模式
//////////////////////////////////////////////////////////////////////////////
// 低功耗模式配置
void WFI_SET(void); // 执行WFI指令
void INTX_DISABLE(void); // 关闭所有中断
void INTX_ENABLE(void); // 开启所有中断
void MSR_MSP(u32 addr); // 设置主堆栈指针


#endif // __SYS_H
