#include "exti.h"

/**************************************************************************
Function: External interrupt initialization
Input   : none
Output  : none
功能    : 外部中断初始化
输入参数: 无
返回值  : 无
**************************************************************************/
void MiniBalance_EXTI_Init(void)
{  
    // 定义GPIO初始化结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    // 定义EXTI初始化结构体
    EXTI_InitTypeDef EXTI_InitStructure;
    // 定义NVIC初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

    // 配置GPIOA的第12引脚为上拉输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	          
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;        
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 

    // 将GPIOA的第12引脚映射到EXTI线
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);

    // 配置EXTI线12
    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);	 	

    // 配置NVIC
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01; // 子优先级					
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							
    NVIC_Init(&NVIC_InitStructure); 
}
