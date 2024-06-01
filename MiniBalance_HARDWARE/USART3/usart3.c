#include "usart3.h"

uint8_t Usart3_Receive;

/**************************************************************************
* 函数功能: USART3 初始化
* 输入参数: bound:波特率
* 返回值  : 无
**************************************************************************/
void uart3_init(uint32_t bound) { 
    // GPIO 端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能 GPIOB 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // 使能 USART3 时钟

    // USART3_TX 配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3_RX 配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ 通道使能
    NVIC_Init(&NVIC_InitStructure); // 初始化 NVIC 寄存器

    // USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound; // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No; // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; // 接收和发送模式
    USART_Init(USART3, &USART_InitStructure); // 初始化 USART3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 使能 USART 接收中断
    USART_Cmd(USART3, ENABLE); // 使能 USART3
}

/**************************************************************************
* 函数功能: USART3 接收中断处理函数
* 输入参数: 无
* 返回值  : 无
**************************************************************************/
void USART3_IRQHandler(void) { 
    if (USART3->SR & (1 << 5)) { // 接收到数据
        static uint8_t Flag_PID, i, j, Receive[50], Last_Usart3_Receive;
        static float Data;
        Usart3_Receive = USART_ReceiveData(USART3); 
        APP_RX = Usart3_Receive;

        if (Usart3_Receive == 0x41 && Last_Usart3_Receive == 0x41 && APP_Flag == 0) {
            APP_Flag = 1;
        }

        if (Usart3_Receive == 0x45 && Last_Usart3_Receive == 0x45) {
            Flag_Direction = 0x45;
        } else {
            Flag_Direction = Usart3_Receive;
        }

        if (Usart3_Receive < 10) {
            Flag_Direction = Usart3_Receive;
        } else if (Usart3_Receive == 0x5A) {
            Flag_Direction = 0;
        }

        Last_Usart3_Receive = Usart3_Receive;

        // 以下是与 APP 调试界面通讯
        if (Usart3_Receive == 0x7B) Flag_PID = 1; // APP 参数指令开始位
        if (Usart3_Receive == 0x7D) Flag_PID = 2; // APP 参数指令结束位

        if (Flag_PID == 1) { // 采集数据
            Receive[i] = Usart3_Receive;
            i++;
        }

        if (Flag_PID == 2) { // 解析数据
            if (Receive[3] == 0x50) {
                PID_Send = 1;
            } else if (Receive[3] == 0x57) {
                Flash_Send = 1;
            } else if (Receive[1] != 0x23) {
                for (j = i; j >= 4; j--) {
                    Data += (Receive[j - 1] - 48) * pow(10, i - j);
                }
                switch (Receive[1]) {
                    case 0x30: Bluetooth_Velocity = Data; break;
                    case 0x31: Velocity_KP = Data; break;
                    case 0x32: Velocity_KI = Data; break;
                    case 0x33: break;
                    case 0x34: break;
                    case 0x35: break;
                    case 0x36: break;
                    case 0x37: break; // 预留
                    case 0x38: break; // 预留
                }
            }
            Flag_PID = 0; // 相关标志位清零
            i = 0;
            j = 0;
            Data = 0;
            memset(Receive, 0, sizeof(uint8_t) * 50); // 数组清零
        }
    } 
}
