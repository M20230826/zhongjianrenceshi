#include "led.h"

/**************************************************************************
函数: LED初始化
输入: 无
输出: 无
描述: 初始化LED的GPIO引脚。
**************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 启用GPIOA时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;             // 设置引脚4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 设置为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 设置速度为50 MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // 初始化GPIOA引脚4
}

/**************************************************************************
函数: LED闪烁
输入: time - 闪烁频率
输出: 无
描述: 使LED以指定频率闪烁。
**************************************************************************/
void Led_Flash(u16 time)
{
    static int temp;
    if (time == 0) 
    {
        LED = 0; // 关闭LED
    }
    else if (++temp == time) 
    {
        LED = ~LED; // 切换LED状态
        temp = 0;
    }
}
