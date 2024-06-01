
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
// 如果需要使用OS，请包含以下头文件
#if SYSTEM_SUPPORT_OS
#include "includes.h" // ucos 使用	  
#endif

static u8  fac_us=0; // us级延时倍乘数			   
static u16 fac_ms=0; // ms级延时倍乘数,在ucos下,代表每个节拍的ms数

#if SYSTEM_SUPPORT_OS // 如果SYSTEM_SUPPORT_OS定义了,说明要支持OS了(包括UCOS)
// 定义与UCOSII相关的宏
#ifdef 	OS_CRITICAL_METHOD
#define delay_osrunning		OSRunning // OS运行标志,0,不运行;1,运行
#define delay_ostickspersec	OS_TICKS_PER_SEC // OS每秒节拍数
#define delay_osintnesting 	OSIntNesting // 中断嵌套标志
#endif

// 定义与UCOSIII相关的宏
#ifdef 	CPU_CFG_CRITICAL_METHOD
#define delay_osrunning		OSRunning // OS运行标志,0,不运行;1,运行
#define delay_ostickspersec	OSCfg_TickRate_Hz // OS每秒节拍数
#define delay_osintnesting 	OSIntNestingCtr // 中断嵌套标志
#endif

/**************************************************************************
* Function: delay_osschedlock
* Description: 锁定任务调度，防止us级延时被打断
* Input: None
* Output: None
**************************************************************************/
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD // 使用UCOSIII
	OS_ERR err; 
	OSSchedLock(&err); // UCOSIII方法，禁止调度
#else // 否则使用UCOSII
	OSSchedLock(); // UCOSII方法，禁止调度
#endif
}

/**************************************************************************
* Function: delay_osschedunlock
* Description: 解锁任务调度，恢复正常调度
* Input: None
* Output: None
**************************************************************************/
void delay_osschedunlock(void)
{	
#ifdef CPU_CFG_CRITICAL_METHOD // 使用UCOSIII
	OS_ERR err; 
	OSSchedUnlock(&err); // UCOSIII方法，恢复调度
#else // 否则使用UCOSII
	OSSchedUnlock(); // UCOSII方法，恢复调度
#endif
}

/**************************************************************************
* Function: delay_ostimedly
* Description: 调用OS的延时函数进行延时
* Input: ticks - 延时的节拍数
* Output: None
**************************************************************************/
void delay_ostimedly(u32 ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
	OS_ERR err; 
	OSTimeDly(ticks, OS_OPT_TIME_PERIODIC, &err); // UCOSIII延时
#else
	OSTimeDly(ticks); // UCOSII延时
#endif 
}

/**************************************************************************
* Function: SysTick_Handler
* Description: SYSTICK中断服务函数，用于ucos
* Input: None
* Output: None
**************************************************************************/
void SysTick_Handler(void)
{	
	if (delay_osrunning == 1) // OS开始跑了,才执行正常的调度处理
	{
		OSIntEnter(); // 进入中断
		OSTimeTick(); // 调用ucos的时钟服务程序               
		OSIntExit(); // 出中断
	}
}
#endif

/**************************************************************************
* Function: delay_init
* Description: 初始化延时函数
* Input: SYSCLK - 系统时钟频率
* Output: None
**************************************************************************/
void delay_init()
{
#if SYSTEM_SUPPORT_OS // 如果需要支持OS
    u32 reload;
#endif
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // SYSTICK时钟源为HCLK/8
    fac_us = SystemCoreClock / 8000000; // 1/8的系统时钟频率

#if SYSTEM_SUPPORT_OS // 如果需要支持OS
    reload = SystemCoreClock / 8000000; // 每秒钟的计数周期数
    reload *= 1000000 / delay_ostickspersec; // 根据OS的节拍数计算重装载值
    fac_ms = 1000 / delay_ostickspersec; // 每个节拍的ms数

    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // 开启SYSTICK中断
    SysTick->LOAD = reload; // 设置重装载值
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开启SYSTICK

#else
    fac_ms = (u16)fac_us * 1000; // 不使用OS时，直接计算ms级延时倍乘数
#endif
}

#if SYSTEM_SUPPORT_OS // 如果需要支持OS
/**************************************************************************
* Function: delay_us
* Description: us级延时函数
* Input: nus - 延时的us数
* Output: None
**************************************************************************/
void delay_us(u32 nus)
{
    u32 ticks;
    u32 told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD; // LOAD的值
    ticks = nus * fac_us; // 需要的节拍数
    tcnt = 0;
    delay_osschedlock(); // 锁定OS调度
    told = SysTick->VAL; // 读取当前VAL值

    while (1)
    {
        tnow = SysTick->VAL; // 读取当前VAL值
        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow; // 计算经过的时间
            else
                tcnt += reload - tnow + told; // 处理SYSTICK重装载的情况
            told = tnow;
            if (tcnt >= ticks) break; // 如果时间达到要求则退出
        }
    };
    delay_osschedunlock(); // 解锁OS调度
}

/**************************************************************************
* Function: delay_ms
* Description: ms级延时函数
* Input: nms - 延时的ms数
* Output: None
**************************************************************************/
void delay_ms(u16 nms)
{
    if (delay_osrunning && delay_osintnesting == 0) // 如果OS已经运行且未嵌套中断
    {
        if (nms >= fac_ms) // 如果延时大于等于一个节拍
        {
            delay_ostimedly(nms / fac_ms); // 调用OS延时
        }
        nms %= fac_ms; // 剩余的时间
    }
    delay_us((u32)(nms * 1000)); // 调用us级延时
}
#endif

#if !SYSTEM_SUPPORT_OS // 如果不需要支持OS
/**************************************************************************
* Function: delay_us
* Description: us级延时函数
* Input: nus - 延时的us数
* Output: None
**************************************************************************/
void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us; // 计数周期数
    SysTick->VAL = 0x00; // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开启SYSTICK

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // 关闭SYSTICK
    SysTick->VAL = 0x00; // 清空计数器
}

/**************************************************************************
* Function: delay_ms
* Description: ms级延时函数
* Input: nms - 延时的ms数
* Output: None
**************************************************************************/
// 注意nms的范围
// SysTick->LOAD是24位寄存器，最大值为0xFFFFFF
// SYSCLK单位为Hz，nms单位为ms
// 例如，SYSCLK为72MHz时，最大延时为1864ms
void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD = (u32)nms * fac_ms; // 计数周期数
    SysTick->VAL = 0x00; // 清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开启SYSTICK

    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; // 关闭SYSTICK
    SysTick->VAL = 0x00; // 清空计数器
}
#endif
