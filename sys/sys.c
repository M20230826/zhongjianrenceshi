#include "sys.h" 

/**************************************************************************
函数: 设置向量表偏移地址
输入: NVIC_VectTab - 基地址, Offset - 偏移值
输出: 无
**************************************************************************/
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{ 	   	 
    SCB->VTOR = NVIC_VectTab | (Offset & (u32)0x1FFFFF80); // 设置NVIC向量表偏移
}

/**************************************************************************
函数: 设置NVIC优先级分组
输入: NVIC_Group - NVIC优先级分组
输出: 无
**************************************************************************/
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
    u32 temp, temp1;	  
    temp1 = (~NVIC_Group) & 0x07; // 获取低三位
    temp1 <<= 8;
    temp = SCB->AIRCR;  // 读取当前AIRCR值
    temp &= 0X0000F8FF; // 清除之前的优先级分组设置
    temp |= 0X05FA0000; // 写入钥匙值
    temp |= temp1;	   
    SCB->AIRCR = temp;  // 设置新的优先级分组	    	  				   
}

/**************************************************************************
函数: 初始化NVIC中断
输入: NVIC_PreemptionPriority - 抢占优先级
      NVIC_SubPriority - 子优先级
      NVIC_Channel - 中断通道
      NVIC_Group - NVIC分组
输出: 无
**************************************************************************/
void MY_NVIC_Init(u8 NVIC_PreemptionPriority, u8 NVIC_SubPriority, u8 NVIC_Channel, u8 NVIC_Group)	 
{ 	
    u32 temp;	
    MY_NVIC_PriorityGroupConfig(NVIC_Group); // 设置优先级分组
    temp = NVIC_PreemptionPriority << (4 - NVIC_Group);	  
    temp |= NVIC_SubPriority & (0x0f >> NVIC_Group);
    temp &= 0xf; // 获取低四位  
    NVIC->ISER[NVIC_Channel / 32] |= (1 << NVIC_Channel % 32); // 使能中断
    NVIC->IP[NVIC_Channel] |= temp << 4; // 设置优先级	    	  				   
} 
/**************************************************************************
函数: 外部中断功能配置
输入: GPIOx - 通用输入/输出
      BITx - 需要使能的端口
      TRIM - 触发模式
输出: 无
**************************************************************************/
void Ex_NVIC_Config(u8 GPIOx, u8 BITx, u8 TRIM) 
{
    u8 EXTADDR;
    u8 EXTOFFSET;
    EXTADDR = BITx / 4; // 获取中断线索引
    EXTOFFSET = (BITx % 4) * 4; 
    RCC->APB2ENR |= 0x01; // 使能AFIO时钟
    AFIO->EXTICR[EXTADDR] &= ~(0x000F << EXTOFFSET); // 清除之前的设置
    AFIO->EXTICR[EXTADDR] |= GPIOx << EXTOFFSET; // 将EXTI线映射到GPIOx
//  EXTI->IMR |= 1 << BITx; // 取消屏蔽中断线BITx
    if (TRIM & 0x01) EXTI->FTSR |= 1 << BITx; // 使能下降沿触发
    if (TRIM & 0x02) EXTI->RTSR |= 1 << BITx; // 使能上升沿触发
}

/**************************************************************************
函数: 重置所有时钟寄存器
输入: 无
输出: 无
**************************************************************************/
void MYRCC_DeInit(void)
{	
    RCC->APB1RSTR = 0x00000000; // 重置APB1外设
    RCC->APB2RSTR = 0x00000000; // 重置APB2外设
	  
    RCC->AHBENR = 0x00000014;  // 使能SRAM和FLITF时钟
    RCC->APB2ENR = 0x00000000; // 禁用所有APB2外设时钟
    RCC->APB1ENR = 0x00000000; // 禁用所有APB1外设时钟
    RCC->CR |= 0x00000001;     // 使能HSI（高速内部时钟）

    RCC->CFGR &= 0xF8FF0000;   // 重置SW, HPRE, PPRE1, PPRE2, ADCPRE, MCO
    RCC->CR &= 0xFEF6FFFF;     // 禁用HSE, CSS, PLL
    RCC->CR &= 0xFFFBFFFF;     // 禁用HSEBYP
    RCC->CFGR &= 0xFF80FFFF;   // 重置PLLSRC, PLLXTPRE, PLLMUL, USBPRE
    RCC->CIR = 0x00000000;     // 禁用所有时钟中断
	  
    // 设置向量表位置
#ifdef  VECT_TAB_RAM
    MY_NVIC_SetVectorTable(0x20000000, 0x0);
#else   
    MY_NVIC_SetVectorTable(0x08000000, 0x0);
#endif
}
// THUMB指令集下的汇编代码，用于进入低功耗等待中断模式
__asm void WFI_SET(void)
{
    WFI;		  
}

// 禁用所有中断的汇编代码
__asm void INTX_DISABLE(void)
{
    CPSID I;		  
}

// 使能所有中断的汇编代码
__asm void INTX_ENABLE(void)
{
    CPSIE I;		  
}

// 设置主堆栈指针的汇编代码
// addr: 堆栈指针地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 // 设置主堆栈指针值
    BX r14
}

/**************************************************************************
函数: 进入待机模式
输入: 无
输出: 无
**************************************************************************/
void Sys_Standby(void)
{
    SCB->SCR |= 1 << 2; // 设置系统控制寄存器中的SLEEPDEEP位
    RCC->APB1ENR |= 1 << 28; // 使能电源接口时钟
    PWR->CSR |= 1 << 8; // 清除唤醒标志
    PWR->CR |= 1 << 2; // 清除待机标志
    PWR->CR |= 1 << 1; // 设置PDDS位进入待机模式
    WFI_SET(); // 执行WFI指令进入低功耗模式
}	     

/**************************************************************************
函数: 系统软复位
输入: 无
输出: 无
**************************************************************************/
void Sys_Soft_Reset(void)
{   
    SCB->AIRCR = 0X05FA0000 | (u32)0x04; // 触发系统复位
} 		 
/**************************************************************************
函数: 设置JTAG模式
输入: mode - JTAG, SWD模式设置
输出: 无
**************************************************************************/
void JTAG_Set(u8 mode)
{
    u32 temp;
    temp = mode << 25;
    RCC->APB2ENR |= 1 << 0; // 使能AFIO时钟
    AFIO->MAPR &= ~(7 << 24); // 清除之前的JTAG/SWD设置
    AFIO->MAPR |= temp; // 设置新的JTAG/SWD模式
}

/**************************************************************************
函数: 系统时钟初始化
输入: PLL - PLL倍频因子
输出: 无
**************************************************************************/
void Stm32_Clock_Init(u8 PLL)
{
    u8 temp = 0;
    RCC->CR |= 0x00010000; // 使能HSE
    while(!(RCC->CR & 0x00020000)); // 等待HSE就绪
    RCC->CFGR = 0x00000000; // 重置CFGR
    RCC->CR &= ~(1 << 24); // 禁用PLL
    RCC->CFGR |= (PLL - 2) << 18; // 设置PLL倍频因子
    RCC->CFGR |= 0x00010000; // 设置PLL源为HSE
    RCC->CR |= 0x01000000; // 使能PLL
    while(!(RCC->CR & 0x02000000)); // 等待PLL就绪
    FLASH->ACR |= 0x32; // 配置Flash预取、指令缓存和数据缓存
    RCC->CFGR |= 0x00000002; // 设置PLL作为系统时钟源
    while(temp != 0x02) // 等待PLL作为系统时钟源
    {
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
}
