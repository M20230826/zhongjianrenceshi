#ifndef __MOTOR_H
#define __MOTOR_H

#include <sys.h>	 

// 定义用于电机控制的PWM信号
#define PWMB   TIM1->CCR4  // 电机B的PWM信号 (PA11)
#define BIN2   PBout(12)   // 电机B IN2控制信号 (PB12)
#define BIN1   PBout(13)   // 电机B IN1控制信号 (PB13)
#define AIN2   PBout(15)   // 电机A IN2控制信号 (PB15)
#define AIN1   PBout(14)   // 电机A IN1控制信号 (PB14)
#define PWMA   TIM1->CCR1  // 电机A的PWM信号 (PA8)

// 函数原型
void MiniBalance_Motor_Init(void);
void MiniBalance_PWM_Init(uint16_t arr, uint16_t psc);

#endif
