#include "key.h"

/**************************************************************************
函数名: KEY_Init
描述  : 按键初始化
输入  : 无
输出  : 无
说明  :
 - 初始化按键的GPIO引脚。
**************************************************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;             // 设置引脚5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // 设置为上拉输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // 初始化GPIOA引脚5
}

/**************************************************************************
函数名: click_N_Double
描述  : 按键扫描
输入  : time - 双击等待时间
输出  : 0 - 无动作, 1 - 单击, 2 - 双击
说明  :
 - 扫描按键以检测单击或双击。
**************************************************************************/
u8 click_N_Double(u8 time)
{
    static u8 flag_key, count_key, double_key;    
    static u16 count_single, Forever_count;

    if (KEY == 0)  Forever_count++;   // 如果按键被按下，永久计数器增加
    else           Forever_count = 0; // 如果按键释放，永久计数器重置

    if (KEY == 0 && flag_key == 0) flag_key = 1; // 检测按键按下

    if (count_key == 0)
    {
        if (flag_key == 1) 
        {
            double_key++;
            count_key = 1; // 注册一次按键按下
        }
        if (double_key == 2) 
        {   // 检测双击
            double_key = 0;
            count_single = 0;
            return 2; // 返回检测到双击
        }
    }

    if (KEY == 1) flag_key = 0, count_key = 0; // 如果按键释放，重置标志

    if (double_key == 1)
    {
        count_single++;
        if (count_single > time && Forever_count < time)
        {
            double_key = 0;
            count_single = 0; // 在时间限制内检测到单击
            return 1; // 返回检测到单击
        }
        if (Forever_count > time)
        {
            double_key = 0;
            count_single = 0; // 如果超过时间限制，重置计数器
        }
    }    
    return 0; // 未检测到点击
}

/**************************************************************************
函数名: click
描述  : 按键扫描
输入  : 无
输出  : 0 - 无动作, 1 - 单击
说明  :
 - 扫描按键以检测单击。
**************************************************************************/
u8 click(void)
{
    static u8 flag_key = 1; // 按键按下标志

    if (flag_key && KEY == 0) // 检测按键按下
    {
        flag_key = 0;
        return 1; // 检测到按键按下
    }
    else if (KEY == 1) flag_key = 1; // 如果按键释放，重置标志

    return 0; // 未检测到按键按下
}

/**************************************************************************
 * 函数名: Key
 * 描述: 处理按键事件
 * 输入: 无
 * 输出: 无
 **************************************************************************/
void Key(void)
{	
    u8 tmp, tmp2;
    tmp = click(); 
    if (tmp == 1) 
        Flag_Stop = !Flag_Stop; // 单击按键，切换停止标志
    tmp2 = Long_Press();          
    if (tmp2 == 1) 
        Flag_Show = !Flag_Show; // 长按按键，切换显示标志
}

/**************************************************************************
函数名: Long_Press
描述  : 长按检测
输入  : 无
输出  : 0 - 无动作, 1 - 长按2秒
说明  :
 - 检测按键的长按。
**************************************************************************/
u8 Long_Press(void)
{
    static u16 Long_Press_count, Long_Press;

    if (Long_Press == 0 && KEY == 0) Long_Press_count++; // 如果按键按下，长按计数器增加
    else                             Long_Press_count = 0; // 如果按键释放，长按计数器重置

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
描述  : 根据按键输入选择模式
输入  : 无
输出  : flag - 模式选择状态
说明  :
 - 根据单击或双击按键选择不同的模式。
**************************************************************************/
u8 select(void)
{
    u8 i;
    static u8 flag = 1;

    oled_show_once(); // 在OLED上显示一次
    i = click_N_Double(50); // 检测单击或双击

    if (i == 1)
    { 
        Mode++;
        if (Mode == 0) BLUETOOTH_Mode = 1, ELE_Line_Patrol_Mode = 0;
        if (Mode == 1) BLUETOOTH_Mode = 0, ELE_Line_Patrol_Mode = 1;
    }

    if (i == 2) OLED_Clear(), flag = 0; // 清除OLED屏幕并退出模式选择

    return flag;    
}
