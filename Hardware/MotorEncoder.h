#ifndef __MOTORENCODER_H 
#define __MOTORENCODER_H 

#include "sys.h"
#include "TB6612.h"

// 函数声明

/**
 * @brief 初始化编码器
 */
void MotorEncoder_Init(void);

/**
 * @brief 读取编码器值
 * @return 编码器的当前值
 */
int Read_Encoder(void);

/**
 * @brief 初始化 TIM2 以读取编码器数据
 * @param arr 自动重装载值
 * @param psc 预分频值
 */
void EncoderRead_TIM2(u16 arr, u16 psc);

/**
 * @brief 位置反馈控制（PID 控制）
 * @param Circle 目标圈数
 * @param CurrentPosition 当前的位置
 * @return 控制量
 */
int Position_FeedbackControl(float Circle, int CurrentPosition);

/**
 * @brief 速度反馈控制（PI 控制）
 * @param TargetVelocity 目标速度
 * @param CurrentVelocity 当前速度
 * @return 控制量
 */
int Velocity_FeedbackControl(int TargetVelocity, int CurrentVelocity);

/**
 * @brief 限制 PWM 值
 * @param PWM_P 位置控制的 PWM 值
 * @param TargetVelocity 目标速度
 * @return 受限的 PWM 值
 */
int PWM_Restrict(int PWM_P, int TargetVelocity);

#endif // __MOTORENCODER_H
