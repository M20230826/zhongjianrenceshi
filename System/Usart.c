#include "sys.h"
#include "usart.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h" // 如果使用了ucos操作系统，需要包含这个头文件
#endif

#if 1
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x) 
{ 
    x = x; 
} 
// 重定向fputc函数
int fputc(int ch, FILE *f)
{      
    while((USART1->SR & 0X40) == 0); // 等待发送完成
    USART1->DR = (u8) ch;            // 发送字符
    return ch;
}
#endif

#if EN_USART1_RX // 如果使能了USART1接收

// 定义接收缓冲区和接收状态
u8 USART_RX_BUF[USART_REC_LEN]; // 接收缓冲区
u16 USART_RX_STA = 0;           // 接收状态

// 初始化USART1
void uart_init(u32 bound)
{
    // GPIO初始化结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能USART1和GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 初始化USART1_TX (PA.9)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化USART1_RX (PA.10)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART1 NVIC
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能IRQ通道
    NVIC_Init(&NVIC_InitStructure);

    // 初始化USART1
    USART_InitStructure.USART_BaudRate = bound;                // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;// 字长8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     // 一位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;        // 无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 接收和发送模式
    USART_Init(USART1, &USART_InitStructure);

    // 使能USART1接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

// USART1中断处理函数
void USART1_IRQHandler(void)
{
    u8 Res;
#if SYSTEM_SUPPORT_OS
    OSIntEnter(); // 如果使用OS，进入中断
#endif
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 接收中断
    {
        Res = USART_ReceiveData(USART1); // 读取接收到的数据

        if ((USART_RX_STA & 0x8000) == 0) // 如果接收未完成
        {
            if (USART_RX_STA & 0x4000) // 如果接收到0x0d
            {
                if (Res != 0x0a) USART_RX_STA = 0; // 如果不是0x0a，接收错误，重新开始
                else USART_RX_STA |= 0x8000;       // 接收完成
            }
            else // 如果尚未接收到0x0d
            {
                if (Res == 0x0d) USART_RX_STA |= 0x4000; // 接收到0x0d
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
                    USART_RX_STA++;
                    if (USART_RX_STA > (USART_REC_LEN - 1)) USART_RX_STA = 0; // 接收数据超出缓冲区，重新开始
                }
            }
        }
    }
#if SYSTEM_SUPPORT_OS
    OSIntExit(); // 如果使用OS，退出中断
#endif
}
#endif
