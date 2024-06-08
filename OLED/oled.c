#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"

u8 OLED_GRAM[128][8];

/**************************************************************************
函数名: OLED_Refresh_Gram
描述  : 刷新OLED屏幕
输入  : 无
输出  : 无
说明  :
 - 使用OLED_GRAM的内容刷新OLED显示。
**************************************************************************/
void OLED_Refresh_Gram(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);    // 设置页地址（0~7）
        OLED_WR_Byte(0x00, OLED_CMD);        // 设置显示起始行低地址
        OLED_WR_Byte(0x10, OLED_CMD);        // 设置显示起始行高地址
        for (n = 0; n < 128; n++)
        {
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
        }
    }
}

/**************************************************************************
函数名: OLED_WR_Byte
描述  : 向OLED写入字节
输入  : dat - 要写入的数据/命令, cmd - 数据/命令标志（0: 命令, 1: 数据）
输出  : 无
说明  :
 - 向OLED显示器写入一个字节（数据或命令）。
**************************************************************************/
void OLED_WR_Byte(u8 dat, u8 cmd)
{
    u8 i;
    if (cmd)
        OLED_RS_Set();
    else
        OLED_RS_Clr();

    for (i = 0; i < 8; i++)
    {
        OLED_SCLK_Clr();
        if (dat & 0x80)
            OLED_SDIN_Set();
        else
            OLED_SDIN_Clr();
        OLED_SCLK_Set();
        dat <<= 1;
    }
    OLED_RS_Set();
}
/**************************************************************************
函数名: OLED_Display_On
描述  : 打开OLED显示
输入  : 无
输出  : 无
说明  :
 - 打开OLED显示。
**************************************************************************/
void OLED_Display_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  // 设置DCDC命令
    OLED_WR_Byte(0x14, OLED_CMD);  // DCDC ON
    OLED_WR_Byte(0xAF, OLED_CMD);  // DISPLAY ON
}

/**************************************************************************
函数名: OLED_Display_Off
描述  : 关闭OLED显示
输入  : 无
输出  : 无
说明  :
 - 关闭OLED显示。
**************************************************************************/
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  // 设置DCDC命令
    OLED_WR_Byte(0x10, OLED_CMD);  // DCDC OFF
    OLED_WR_Byte(0xAE, OLED_CMD);  // DISPLAY OFF
}

/**************************************************************************
函数名: OLED_Clear
描述  : 清屏
输入  : 无
输出  : 无
说明  :
 - 清除OLED屏幕，使整个屏幕变黑。
**************************************************************************/
void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 128; n++)
        {
            OLED_GRAM[n][i] = 0x00;
        }
    }
    OLED_Refresh_Gram(); // 刷新以应用更改
}
/**************************************************************************
函数名: OLED_DrawPoint
描述  : 画点
输入  : x - x坐标, y - y坐标, t - 1: 填充, 0: 清除
输出  : 无
说明  :
 - 在指定坐标处绘制或清除一个点。
**************************************************************************/
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
    u8 pos, bx, temp = 0;
    if (x > 127 || y > 63) return; // 防止越界

    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
    if (t)
        OLED_GRAM[x][pos] |= temp;
    else
        OLED_GRAM[x][pos] &= ~temp;
}

/**************************************************************************
函数名: OLED_ShowChar
描述  : 显示字符
输入  : x - 起始x坐标, y - 起始y坐标, chr - 要显示的字符, size - 字体大小, mode - 0: 反显, 1: 正常显示
输出  : 无
说明  :
 - 在指定位置显示一个字符。
**************************************************************************/
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    chr = chr - ' '; // 获取偏移值
    for (t = 0; t < size; t++)
    {
        if (size == 12)
            temp = oled_asc2_1206[chr][t]; // 12x6字体
        else
            temp = oled_asc2_1608[chr][t]; // 16x8字体

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
                OLED_DrawPoint(x, y, mode);
            else
                OLED_DrawPoint(x, y, !mode);
            temp <<= 1;
            y++;
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}
/**************************************************************************
函数名: oled_pow
描述  : 计算m的n次幂
输入  : m - 基数, n - 指数
输出  : result - m的n次幂
说明  :
 - 计算一个数的幂。
**************************************************************************/
u32 oled_pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
    {
        result *= m;
    }
    return result;
}

/**************************************************************************
函数名: OLED_ShowNumber
描述  : 显示数字
输入  : x - 起始x坐标, y - 起始y坐标, num - 要显示的数字, len - 数字位数, size - 字体大小
输出  : 无
说明  :
 - 在指定位置显示一个数字。
**************************************************************************/
void OLED_ShowNumber(u8 x, u8 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + (size / 2) * t, y, ' ', size, 1);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
    }
}

/**************************************************************************
函数名: OLED_ShowString
描述  : 显示字符串
输入  : x - 起始x坐标, y - 起始y坐标, *p - 字符串起始地址
输出  : 无
说明  :
 - 在指定位置显示一个字符串。
**************************************************************************/
void OLED_ShowString(u8 x, u8 y, const u8 *p)
{
    #define MAX_CHAR_POSX 122
    #define MAX_CHAR_POSY 58

    while (*p != '\0')
    {
        if (x > MAX_CHAR_POSX)
        {
            x = 0;
            y += 16;
        }
        if (y > MAX_CHAR_POSY)
        {
            y = x = 0;
            OLED_Clear();
        }
        OLED_ShowChar(x, y, *p, 12, 1);
        x += 8;
        p++;
    }
}
/**************************************************************************
函数名: OLED_Init
描述  : 初始化OLED
输入  : 无
输出  : 无
说明  :
 - 初始化OLED显示。
**************************************************************************/
void OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能PB时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;      // 2 MHz速度
    GPIO_Init(GPIOB, &GPIO_InitStructure);                // 初始化GPIOB

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能PA时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // 初始化GPIOA

    PWR_BackupAccessCmd(ENABLE); // 使能访问备份域
    RCC_LSEConfig(RCC_LSE_OFF);  // 关闭LSE时钟
    BKP_TamperPinCmd(DISABLE);   // 禁用防篡改引脚
    PWR_BackupAccessCmd(DISABLE);// 禁用访问备份域

    OLED_RST_Clr();
    delay_ms(100);
    OLED_RST_Set();

    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭显示
    OLED_WR_Byte(0xD5, OLED_CMD); // 设置显示时钟分频比/振荡频率
    OLED_WR_Byte(80, OLED_CMD);   // 设置分频比
    OLED_WR_Byte(0xA8, OLED_CMD); // 设置多路复用比
    OLED_WR_Byte(0x3F, OLED_CMD); // 1/64占空比
    OLED_WR_Byte(0xD3, OLED_CMD); // 设置显示偏移
    OLED_WR_Byte(0x00, OLED_CMD); // 无偏移
    OLED_WR_Byte(0x40, OLED_CMD); // 设置起始行地址

    OLED_WR_Byte(0x8D, OLED_CMD); // 使能电荷泵
    OLED_WR_Byte(0x14, OLED_CMD); // 使能电荷泵
    OLED_WR_Byte(0x20, OLED_CMD); // 设置内存地址模式
    OLED_WR_Byte(0x02, OLED_CMD); // 页地址模式
    OLED_WR_Byte(0xA1, OLED_CMD); // 设置段重映射
    OLED_WR_Byte(0xC0, OLED_CMD); // 设置COM输出扫描方向
    OLED_WR_Byte(0xDA, OLED_CMD); // 设置COM引脚硬件配置
    OLED_WR_Byte(0x12, OLED_CMD); // 设置COM引脚硬件配置

    OLED_WR_Byte(0x81, OLED_CMD); // 设置对比度控制
    OLED_WR_Byte(0xEF, OLED_CMD); // 设置对比度值
    OLED_WR_Byte(0xD9, OLED_CMD); // 设置预充电周期
    OLED_WR_Byte(0xF1, OLED_CMD); // 设置预充电周期
    OLED_WR_Byte(0xDB, OLED_CMD); // 设置VCOMH取消选择级别
    OLED_WR_Byte(0x30, OLED_CMD); // 设置VCOMH取消选择级别

    OLED_WR_Byte(0xA4, OLED_CMD); // 整个显示开启
    OLED_WR_Byte(0xA6, OLED_CMD); // 设置正常显示
    OLED_WR_Byte(0xAF, OLED_CMD); // 打开显示
    OLED_Clear();
}
