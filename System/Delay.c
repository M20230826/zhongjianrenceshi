#include "key.h"
#include "delay.h"

u8 click_N_Double(u8 interval);


// 按键初始化函数
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 定义 GPIO 初始化结构体

    // 使能 GPIOA 时钟，初始化 GPIOA 引脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

    // 配置 GPIOA 引脚 5 为上拉输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

// 按键扫描函数
u8 KEY_Scan(void)
{
    static u8 flag_key = 1; // 按键状态标志，static 变量在函数间保持值

    // 检测按键按下
    if (flag_key == 1 && KEY == 0) 
    {
        delay_ms(1); // 消抖延时

        // 二次确认按键按下
        if (KEY == 0)
        {
            flag_key = 0; // 更新按键状态标志
            return 1;     // 返回按键按下状态
        }
    }
    else if (KEY == 1) 
    {
        flag_key = 1; // 按键释放，重置按键状态标志
    }

    return 0; // 按键未按下
}

/**************************************************************************
函数名: Long_Press
参数  : 无
返回值: 0 - 无动作, 1 - 长按2秒
描述  : 检测按键长按。
**************************************************************************/
u8 Long_Press(void)
{
    static u16 Long_Press_count, Long_Press;

    if (Long_Press == 0 && KEY == 0) Long_Press_count++; // 如果按键按下，增加长按计数
    else                             Long_Press_count = 0; // 如果按键释放，重置长按计数

    if (Long_Press_count > 200) // 检测长按（假设10ms扫描间隔）
    {
        Long_Press = 1;    
        Long_Press_count = 0;
        return 1; // 检测到长按
    }                

    if (Long_Press == 1) Long_Press = 0; // 重置长按标志

    return 0; // 未检测到长按
}

/**************************************************************************
函数名: select
参数  : 无
返回值: flag - 模式选择状态
描述  : 根据按键输入选择不同的模式。
**************************************************************************/
u8 select(void)
{
    u8 i;
    static u8 flag = 1;

    oled_show_once(); // OLED 显示一次
    i = click_N_Double(50); // 检测单击或双击

    if (i == 1)
    { 
        Mode++;
        if (Mode == 0) BLUETOOTH_Mode = 1;
        if (Mode == 1) BLUETOOTH_Mode = 1;
    }

    if (i == 2) OLED_Clear(), flag = 0; // 清除OLED屏幕并退出模式选择

    return flag;    
}
