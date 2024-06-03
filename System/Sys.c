#include "sys.h"

// THUMB指令集下的一些汇编指令
// 进入低功耗等待模式
void WFI_SET(void)
{
    __ASM volatile("wfi");
}

// 禁用所有中断
void INTX_DISABLE(void)
{
    __ASM volatile("cpsid i");
}

// 使能所有中断
void INTX_ENABLE(void)
{
    __ASM volatile("cpsie i");
}

// 设置主堆栈指针 (MSP)
// addr: 堆栈地址
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 // 设置主堆栈指针
    BX r14
}

// 系统时钟初始化函数
// pll: 选择PLL倍频因子，取值范围为2到16
// void Stm32_Clock_Init(u8 PLL)
// {
//     unsigned char temp = 0;
//     MYRCC_DeInit(); // 复位并初始化RCC寄存器
//     RCC->CR |= 0x00010000; // HSEON, 使能外部高速晶振
//     while (!(RCC->CR >> 17)); // 等待外部高速晶振稳定
//     RCC->CFGR = 0X00000400; // 配置APB1=DIV2, APB2=DIV1, AHB=DIV1
//     PLL -= 2; // PLL倍频因子减2后存入寄存器
//     RCC->CFGR |= PLL << 18; // 设置PLL倍频因子
//     RCC->CFGR |= 1 << 16; // 选择HSE作为PLL输入时钟源
//     FLASH->ACR |= 0x32; // 配置FLASH预取指使能和等待状态

//     RCC->CR |= 0x01000000; // 使能PLL
//     while (!(RCC->CR >> 25)); // 等待PLL稳定
//     RCC->CFGR |= 0x00000002; // 选择PLL作为系统时钟
//     while (temp != 0x02) // 等待PLL作为系统时钟稳定
//     {
//         temp = RCC->CFGR >> 2;
//         temp &= 0x03;
//     }
// }

// 配置JTAG模式
// mode: JTAG模式选择，0: 全JTAG, 1: 仅SWD, 2: 禁用JTAG和SWD
void JTAG_Set(u8 mode)
{
    u32 temp;
    temp = mode;
    temp <<= 25;
    RCC->APB2ENR |= 1 << 0; // 使能AFIO时钟
    AFIO->MAPR &= 0xF8FFFFFF; // 清除MAPR寄存器的[26:24]位
    AFIO->MAPR |= temp; // 设置JTAG模式
}
