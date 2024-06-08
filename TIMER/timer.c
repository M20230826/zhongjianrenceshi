#include "timer.h"

// 定义TIM2的输入捕获初始化结构体
TIM_ICInitTypeDef TIM2_ICInitStructure;

/**************************************************************************
函数名: Initialize TIM2 for Input Capture
输入   : 无
输出   : 无
描述   :
 - 该函数通过配置TIM2定时器为输入捕获模式，并设置为50Hz。
**************************************************************************/
void TIM2_Init(void) 
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;   
    NVIC_InitTypeDef NVIC_InitStruct;	
	
    // 使能TIM2和GPIOA的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置GPIOA的引脚用于TIM2 CH2的输入捕获
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;            // 下拉输入模式
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;                // PA1引脚
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置TIM2定时器
    TIM_TimeBaseInitStruct.TIM_Period = 19999;            // 设置自动重装值为19999
    TIM_TimeBaseInitStruct.TIM_Prescaler = 71;            // 设置预分频系数为71
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数模式
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;      // 时钟分频
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);      // 初始化TIM2定时器

    // 配置并使能TIM2中断（但不启用中断处理函数）
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    // 使能自动重装预装载
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    // 使能TIM2更新中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // 使能TIM2
    TIM_Cmd(TIM2, ENABLE);  // 使能TIM2定时器

    // 配置TIM2的输入捕获
    TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; // 选择通道2进行输入捕获
    TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;    // 上升沿极性
    TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;    // 输入捕获预分频: 1
    TIM2_ICInitStructure.TIM_ICFilter = 0x00;    // 输入捕获滤波: 无滤波
    TIM_ICInit(TIM2, &TIM2_ICInitStructure);

    // 使能TIM2的CC2中断
    TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
}
