#include "usart3.h"
#include "usart.h"

/**************************************************************************
功能描述：USART3初始化函数
来源：http://shop114407458.taobao.com/
**************************************************************************/
u8 Usart3_Receive;
u8 Flag_Forward, Flag_Backward, Flag_LeftTurn, Flag_RightTurn, Flag_Speed = 2; // 定义标志变量
u8 delay_50, delay_flag, Obstacle_Detection = 0, PID_Send, Flash_Send; // 定义延时和发送标志变量

/**************************************************************************
函数功能：初始化USART3
输入参数：bound - 波特率
返回  值：无
**************************************************************************/
void uart3_init(u32 bound)
{
    // 定义GPIO初始化结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    // 定义USART初始化结构体
    USART_InitTypeDef USART_InitStructure;
    // 定义NVIC初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    // 使能USART3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    // 初始化USART3 TX引脚 (PB.10)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 初始化USART3 RX引脚 (PB.11)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置USART3 NVIC中断
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // 使能中断通道
    NVIC_Init(&NVIC_InitStructure); // 初始化NVIC

    // 配置USART3参数
    USART_InitStructure.USART_BaudRate = bound; // 设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 收发模式
    USART_Init(USART3, &USART_InitStructure); // 初始化USART3

    // 使能USART3接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    // 使能USART3
    USART_Cmd(USART3, ENABLE);
}

/**************************************************************************
函数功能：USART3中断服务程序
输入参数：无
返回  值：无
**************************************************************************/
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收中断标志
    {
        Usart3_Receive = USART_ReceiveData(USART3); // 读取接收到的数据
        // 在此添加处理接收到的数据的代码
    }
}
