#include "sys.h"
#include "usart.h"	  
#if SYSTEM_SUPPORT_OS
#include "includes.h"	//ucos 使用	  
#endif

/**************************************************************************
Function: Serial port 1 initialization
Input   : bound - Baud rate
Output  : none

Description:
- Configures GPIO and USART1 for serial port initialization.
- Sets parameters such as baud rate, data bit length, stop bits, parity, and hardware flow control.

**************************************************************************/
void uart_init(u32 bound)
{
    // GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);	//启用USART1和GPIOA时钟
    
    // USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化GPIOA.9
   
    // USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);	//初始化GPIOA.10  
    
    // USART 初始化配置
    USART_InitStructure.USART_BaudRate = bound;	//设置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位长度为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;	//1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;	//无校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//接收和发送模式
    
    USART_Init(USART1, &USART_InitStructure); //初始化USART1
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	//禁止接收中断
    USART_Cmd(USART1, ENABLE);	//启用USART1
}

#if 1
#pragma import(__use_no_semihosting)             
//需要准备好微库并选择printf功能，这是一个必要的选择
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       
//当_sys_exit()被调用时，使用空函数以避免链接错误
void _sys_exit(int x) 
{ 
    x = x; 
} 
//重定向fputc函数
int fputc(int ch, FILE *f)
{      
    if (Flag_Show == 0)
    {	
        while ((USART3->SR & 0X40) == 0); //Flag_Show=0 使用USART3发送
        USART3->DR = (u8) ch;      
    }
    else
    {	
        while ((USART1->SR & 0X40) == 0); //Flag_Show!=0 使用USART1发送
        USART1->DR = (u8) ch;      
    }	
    return ch;
}
#endif 
