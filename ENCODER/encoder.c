#include "encoder.h"
#include "stm32f10x_gpio.h"

/**************************************************************************
函数名: Encoder_Init_TIM3
描述  : 初始化TIM3为编码器接口模式
输入  : 无
输出  : 无
说明  :
 - 该函数通过配置GPIOA上的引脚，将TIM3初始化为编码器接口模式。
参数  :
 - 无
**************************************************************************/
void Encoder_Init_TIM3(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能TIM3和GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    // 配置GPIOA引脚用于编码器接口
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	// 编码器接口引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);	// 初始化GPIOA用于编码器接口
    
    // 初始化TIM3定时器
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // 预分频器
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; // 设置周期值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // 配置TIM3为编码器接口模式
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 初始化TIM3输入捕获
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;	// 输入捕获滤波器
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // 清除更新标志
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    
    // 使能更新中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    // 复位计数器
    TIM_SetCounter(TIM3, 0);
    
    // 使能TIM3
    TIM_Cmd(TIM3, ENABLE); 
}

/**************************************************************************
函数名: Encoder_Init_TIM4
描述  : 初始化TIM4为编码器接口模式
输入  : 无
输出  : 无
说明  :
 - 该函数通过配置GPIOB上的引脚，将TIM4初始化为编码器接口模式。
参数  :
 - 无
**************************************************************************/
void Encoder_Init_TIM4(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能TIM4和GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
    // 配置GPIOB引脚用于编码器接口
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	// 编码器接口引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);	// 初始化GPIOB用于编码器接口
    
    // 初始化TIM4定时器
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; // 设置周期值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // 配置TIM4为编码器接口模式
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 初始化TIM4输入捕获
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;	// 输入捕获滤波器
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
    // 清除更新标志
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    
    // 使能更新中断
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    // 复位计数器
    TIM_SetCounter(TIM4, 0);
    
    // 使能TIM4
    TIM_Cmd(TIM4, ENABLE); 
}

/**************************************************************************
函数名: Read_Encoder
描述  : 读取编码器每单位时间的计数
输入  : TIMX - 定时器
输出  : Encoder_TIM - 编码器计数
说明  :
 - 该函数读取指定定时器（TIM2, TIM3, TIM4）的编码器每单位时间的计数。
参数  :
 - TIMX: 定时器选择（2, 3, 4）
返回  :
 - Encoder_TIM: 编码器每单位时间的计数
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;    
    switch(TIMX)
    {
        case 2:  
            Encoder_TIM = (short)TIM2->CNT;  // 读取TIM2的编码器计数
            TIM2->CNT = 0;  // 复位计数器
            break;
        case 3:  
            Encoder_TIM = (short)TIM3->CNT;  // 读取TIM3的编码器计数
            TIM3->CNT = 0;  // 复位计数器
            break;	
        case 4:  
            Encoder_TIM = (short)TIM4->CNT;  // 读取TIM4的编码器计数
            TIM4->CNT = 0;  // 复位计数器
            break;
        default: 
            Encoder_TIM = 0;  // 如果定时器选择无效则返回0
    }
    return Encoder_TIM;  // 返回编码器计数
}

/**************************************************************************
函数名: TIM4_IRQHandler
描述  : TIM4中断服务函数
输入  : 无
输出  : 无
说明  :
 - 该函数作为TIM4的中断服务例程。
参数  :
 - 无
**************************************************************************/
void TIM4_IRQHandler(void)
{ 		    		  			    
    if(TIM4->SR & 0x0001)  // 检查更新中断标志是否设置
    {    				   				     	    	
        // 执行TIM4中断所需的操作
        
    }				   
    TIM4->SR &= ~(1 << 0);  // 清除更新中断标志	    
}

/**************************************************************************
函数名: TIM3_IRQHandler
描述  : TIM3中断服务函数
输入  : 无
输出  : 无
说明  :
 - 该函数作为TIM3的中断服务例程。
参数  :
 - 无
**************************************************************************/
void TIM3_IRQHandler(void)
{ 		    		  			    
    if(TIM3->SR & 0x0001)  // 检查更新中断标志是否设置
    {    				   				     	    	
        // 执行TIM3中断所需的操作
        
    }				   
    TIM3->SR &= ~(1 << 0);  // 清除更新中断标志	    
}
