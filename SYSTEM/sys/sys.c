#include "sys.h" 

/**************************************************************************
Function: Set the vector table offset address
Input   : NVIC_VectTab - Base address, Offset - Offset value
Output  : none
**************************************************************************/
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{ 	   	 
    SCB->VTOR = NVIC_VectTab | (Offset & (u32)0x1FFFFF80); // Set NVIC vector table offset
}

/**************************************************************************
Function: Set NVIC group
Input   : NVIC_Group - NVIC priority group
Output  : none
**************************************************************************/
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
    u32 temp, temp1;	  
    temp1 = (~NVIC_Group) & 0x07; // Get the lower 3 bits
    temp1 <<= 8;
    temp = SCB->AIRCR;  // Read current AIRCR value
    temp &= 0X0000F8FF; // Clear previous priority group settings
    temp |= 0X05FA0000; // Write key value
    temp |= temp1;	   
    SCB->AIRCR = temp;  // Set new priority group	    	  				   
}

/**************************************************************************
Function: Initialize NVIC interrupt
Input   : NVIC_PreemptionPriority - Preemption priority
          NVIC_SubPriority - Sub priority
          NVIC_Channel - Interrupt channel
          NVIC_Group - NVIC group
Output  : none
**************************************************************************/
void MY_NVIC_Init(u8 NVIC_PreemptionPriority, u8 NVIC_SubPriority, u8 NVIC_Channel, u8 NVIC_Group)	 
{ 	
    u32 temp;	
    MY_NVIC_PriorityGroupConfig(NVIC_Group); // Set priority group
    temp = NVIC_PreemptionPriority << (4 - NVIC_Group);	  
    temp |= NVIC_SubPriority & (0x0f >> NVIC_Group);
    temp &= 0xf; // Get lower 4 bits  
    NVIC->ISER[NVIC_Channel / 32] |= (1 << NVIC_Channel % 32); // Enable interrupt
    NVIC->IP[NVIC_Channel] |= temp << 4; // Set priority	    	  				   
} 

/**************************************************************************
Function: External interrupt function configuration
Input   : GPIOx - General-purpose input/output
          BITx - The port needed enable
          TRIM - Trigger mode
Output  : none
**************************************************************************/
void Ex_NVIC_Config(u8 GPIOx, u8 BITx, u8 TRIM) 
{
    u8 EXTADDR;
    u8 EXTOFFSET;
    EXTADDR = BITx / 4; // Get the interrupt line index
    EXTOFFSET = (BITx % 4) * 4; 
    RCC->APB2ENR |= 0x01; // Enable AFIO clock
    AFIO->EXTICR[EXTADDR] &= ~(0x000F << EXTOFFSET); // Clear previous settings
    AFIO->EXTICR[EXTADDR] |= GPIOx << EXTOFFSET; // Map EXTI line to GPIOx
    EXTI->IMR |= 1 << BITx; // Unmask interrupt line BITx
    if (TRIM & 0x01) EXTI->FTSR |= 1 << BITx; // Enable falling trigger
    if (TRIM & 0x02) EXTI->RTSR |= 1 << BITx; // Enable rising trigger
}

/**************************************************************************
Function: Reset all clock registers
Input   : none
Output  : none
**************************************************************************/
void MYRCC_DeInit(void)
{	
    RCC->APB1RSTR = 0x00000000; // Reset APB1 peripheral
    RCC->APB2RSTR = 0x00000000; // Reset APB2 peripheral
	  
    RCC->AHBENR = 0x00000014;  // Enable SRAM and FLITF clock
    RCC->APB2ENR = 0x00000000; // Disable all APB2 peripheral clocks
    RCC->APB1ENR = 0x00000000; // Disable all APB1 peripheral clocks
    RCC->CR |= 0x00000001;     // Enable HSI (High Speed Internal clock)

    RCC->CFGR &= 0xF8FF0000;   // Reset SW, HPRE, PPRE1, PPRE2, ADCPRE, MCO
    RCC->CR &= 0xFEF6FFFF;     // Disable HSE, CSS, PLL
    RCC->CR &= 0xFFFBFFFF;     // Disable HSEBYP
    RCC->CFGR &= 0xFF80FFFF;   // Reset PLLSRC, PLLXTPRE, PLLMUL, USBPRE
    RCC->CIR = 0x00000000;     // Disable all clock interrupts
	  
    // Set vector table location
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
    MSR MSP, r0 // Set Main Stack Pointer value
    BX r14
}

/**************************************************************************
Function: Go to standby mode
Input   : none
Output  : none
**************************************************************************/
void Sys_Standby(void)
{
    SCB->SCR |= 1 << 2; // Set SLEEPDEEP bit in System Control Register
    RCC->APB1ENR |= 1 << 28; // Enable Power interface clock
    PWR->CSR |= 1 << 8; // Clear Wake-up flag
    PWR->CR |= 1 << 2; // Clear Standby flag
    PWR->CR |= 1 << 1; // Set PDDS bit to enter Standby mode
    WFI_SET(); // Execute WFI instruction to enter low power mode
}	     

/**************************************************************************
Function: System soft reset
Input   : none
Output  : none
**************************************************************************/
void Sys_Soft_Reset(void)
{   
    SCB->AIRCR = 0X05FA0000 | (u32)0x04; // Trigger system reset
} 		 

/**************************************************************************
Function: Set JTAG mode
Input   : mode - JTAG, SWD mode settings
Output  : none
**************************************************************************/
void JTAG_Set(u8 mode)
{
    u32 temp;
    temp = mode << 25;
    RCC->APB2ENR |= 1 << 0; // Enable AFIO clock
    AFIO->MAPR &= ~(7 << 24); // Clear previous JTAG/SWD settings
    AFIO->MAPR |= temp; // Set new JTAG/SWD mode
}

/**************************************************************************
Function: System clock initialization
Input   : PLL - PLL multiplier
Output  : none
**************************************************************************/
void Stm32_Clock_Init(u8 PLL)
{
    u8 temp = 0;
    RCC->CR |= 0x00010000; // Enable HSE
    while(!(RCC->CR & 0x00020000)); // Wait for HSE to be ready
    RCC->CFGR = 0x00000000; // Reset CFGR
    RCC->CR &= ~(1 << 24); // Disable PLL
    RCC->CFGR |= (PLL - 2) << 18; // Set PLL multiplier
    RCC->CFGR |= 0x00010000; // Set PLL source to HSE
    RCC->CR |= 0x01000000; // Enable PLL
    while(!(RCC->CR & 0x02000000)); // Wait for PLL to be ready
    FLASH->ACR |= 0x32; // Configure Flash prefetch, instruction cache, and data cache
    RCC->CFGR |= 0x00000002; // Set PLL as system clock source
    while(temp != 0x02) // Wait for PLL to be used as system clock source
    {
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
}
