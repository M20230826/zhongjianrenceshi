#include "motor.h"

/**************************************************************************
函数名: Initialize Motor Interface
输入   : 无
输出   : 无
描述   :
 - 该函数通过配置GPIOB的引脚来初始化电机接口。
参数   :
 - 无
**************************************************************************/
void MiniBalance_Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOB的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置GPIOB的引脚用于电机控制
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  // 电机控制引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz速度
    GPIO_Init(GPIOB, &GPIO_InitStructure);                // 初始化GPIOB用于电机控制
}

/**************************************************************************
函数名: Initialize PWM to drive motor
输入   : arr - 自动重装值, psc - 预分频系数
输出   : 无
描述   :
 - 该函数通过配置TIM1定时器为PWM输出模式来初始化驱动电机的PWM信号。
参数   :
 - arr: 自动重装值
 - psc: 预分频系数
**************************************************************************/
void MiniBalance_PWM_Init(uint16_t arr, uint16_t psc)
{		 		
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 初始化电机接口
    MiniBalance_Motor_Init();

    // 使能TIM1和GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置GPIOA的引脚用于TIM1 CH1和CH4的PWM输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;  // TIM_CH1, TIM_CH4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用功能推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置TIM1定时器用于PWM输出
    TIM_TimeBaseStructure.TIM_Period = arr;  // 设置自动重装值
    TIM_TimeBaseStructure.TIM_Prescaler = psc;  // 设置预分频系数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);  // 初始化TIM1基础时间单元

    // 配置TIM1的通道1和通道4用于PWM输出
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;  // 设置初始脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // 输出极性高
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  // 初始化TIM1通道1
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);  // 初始化TIM1通道4

    // 使能TIM1主输出
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // 使能TIM1通道1和通道4的预装载
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // 使能自动重装预装载
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // 使能TIM1
    TIM_Cmd(TIM1, ENABLE);
}
