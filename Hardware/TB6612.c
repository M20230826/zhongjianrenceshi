#include "TB6612.h"

/**************************************************************************
函数功能：初始化TB6612
输入参数：arr - 自动重装载值
          psc - 预分频值
返回  值：无
**************************************************************************/
void TB6612_Init(int arr, int psc)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义GPIO初始化结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue; // 定义定时器基础初始化结构体
    TIM_OCInitTypeDef TIM_OCInitTypeStrue; // 定义定时器输出比较初始化结构体

    // 使能GPIOB时钟，GPIOB属于APB2总线外设
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 使能定时器3时钟，定时器3属于APB1总线外设
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 初始化TB6612控制引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度设置为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化GPIOB引脚

    // 复位TB6612控制引脚
    GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13);

    // 初始化TB6612 PWM引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; // 初始化B1引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度设置为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化GPIOB1引脚

    // 配置定时器3的基础参数
    TIM_TimeBaseInitStrue.TIM_Period = arr; // 设置自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler = psc; // 设置预分频值
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频因子
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStrue); // 初始化定时器3

    // 配置定时器3的PWM参数
    TIM_OCInitTypeStrue.TIM_OCMode = TIM_OCMode_PWM1; // PWM模式1
    TIM_OCInitTypeStrue.TIM_OCPolarity = TIM_OCNPolarity_High; // 高电平有效
    TIM_OCInitTypeStrue.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
    TIM_OCInitTypeStrue.TIM_Pulse = 0; // 设置初始占空比为0
    TIM_OC4Init(TIM3, &TIM_OCInitTypeStrue); // 初始化定时器3通道4

    // 使能定时器3的预装载寄存器
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // 使能定时器3
    TIM_Cmd(TIM3, ENABLE);
}

/**************************************************************************
函数功能：设置TIM3通道4的PWM值
输入参数：pwm - PWM值
返回  值：无
**************************************************************************/
void SetPWM(int pwm)
{
    if (pwm >= 0) // 正转
    {
        PBout(13) = 0; // BIN1 = 0
        PBout(12) = 1; // BIN2 = 1
        TIM3->CCR4 = pwm;
        TIM_SetCompare4(TIM3, pwm);
    }
    else // 反转
    {
        PBout(13) = 1; // BIN1 = 1
        PBout(12) = 0; // BIN2 = 0
        TIM3->CCR4 = -pwm;
        TIM_SetCompare4(TIM3, -pwm);
    }
}
