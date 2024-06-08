#ifndef __ADC_H
#define __ADC_H

#include "sys.h"  // 包含系统头文件，确保能够使用sys.h中定义的内容

#define Battery_Ch 8  // 定义电池通道号为8

// 函数声明
void Dly_us(void);  // 微秒级延时函数声明
void RD_TSL(void);  // CCD数据读取函数声明
void Adc_Init(void);  // ADC模块初始化函数声明
u16 Get_Adc(u8 ch);  // 获取ADC数据函数声明
int Get_battery_volt(void);  // 获取电池电压函数声明
void CCD(void);  // CCD函数声明
void ccd_Init(void);  // CCD初始化函数声明
void ele_Init(void);  // 电子元件初始化函数声明

#endif
