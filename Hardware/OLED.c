#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"

/**************************************************************************
功能：OLED屏幕显示驱动代码
**************************************************************************/

u8 OLED_GRAM[128][8]; // OLED显示缓存，128x64像素，每个像素占用1位，共1024字节

/**************************************************************************
函数功能：刷新OLED显示内容
输入参数：无
返回  值：无
**************************************************************************/
void OLED_Refresh_Gram(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
        OLED_WR_Byte(0x00, OLED_CMD);     // 设置显示位置 - 列低地址
        OLED_WR_Byte(0x10, OLED_CMD);     // 设置显示位置 - 列高地址
        for (n = 0; n < 128; n++)
        {
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA); // 写数据到OLED
        }
    }
}

/**************************************************************************
函数功能：向OLED写入一个字节的数据或命令
输入参数：dat - 要写入的数据
          cmd - 数据/命令标志，0表示命令，1表示数据
返回  值：无
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
函数功能：打开OLED显示
输入参数：无
返回  值：无
**************************************************************************/
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // 设置DCDC使能
    OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}

/**************************************************************************
函数功能：关闭OLED显示
输入参数：无
返回  值：无
**************************************************************************/
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // 设置DCDC使能
    OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}

/**************************************************************************
函数功能：清除OLED显示内容
输入参数：无
返回  值：无
**************************************************************************/
void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 128; n++)
        {
            OLED_GRAM[n][i] = 0X00;
        }
    }
    OLED_Refresh_Gram(); // 更新显示
}

/**************************************************************************
函数功能：绘制一个点
输入参数：x - 点的横坐标（0~127）
          y - 点的纵坐标（0~63）
          t - 1表示点亮，0表示熄灭
返回  值：无
**************************************************************************/
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
    u8 pos, bx, temp = 0;
    if (x > 127 || y > 63) return; // 超出范围
    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
    if (t)
        OLED_GRAM[x][pos] |= temp;
    else
        OLED_GRAM[x][pos] &= ~temp;
}
/**************************************************************************
函数功能：显示一个字符
输入参数：x - 字符的起始横坐标（0~127）
          y - 字符的起始纵坐标（0~63）
          chr - 要显示的字符
          size - 字符大小（12或16）
          mode - 0表示反色显示，1表示正常显示
返回  值：无
**************************************************************************/
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    chr = chr - ' '; // 获取字符对应的ASCII值
    for (t = 0; t < size; t++)
    {
        if (size == 12)
            temp = oled_asc2_1206[chr][t]; // 取1206字体
        else
            temp = oled_asc2_1608[chr][t]; // 取1608字体
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
函数功能：计算m的n次方
输入参数：m - 底数
          n - 指数
返回  值：计算结果
**************************************************************************/
u32 oled_pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--) result *= m;
    return result;
}

/**************************************************************************
函数功能：显示一个数字
输入参数：x - 数字的起始横坐标（0~127）
          y - 数字的起始纵坐标（0~63）
          num - 要显示的数字
          len - 数字的长度
          size - 字符大小（12或16）
返回  值：无
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
            else enshow = 1;
        }
        OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
    }
}
/**************************************************************************
函数功能：显示字符串
输入参数：x - 字符串的起始横坐标（0~127）
          y - 字符串的起始纵坐标（0~63）
          *p - 要显示的字符串指针
返回  值：无
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
函数功能：初始化OLED备份访问和LSE配置
输入参数：无
返回  值：无
**************************************************************************/
void OLED_Init_Backup_LSE(void)
{
    PWR_BackupAccessCmd(ENABLE); // 使能访问备份域
    RCC_LSEConfig(RCC_LSE_OFF);  // 关闭LSE时钟
    BKP_TamperPinCmd(DISABLE);   // 禁用篡改引脚
    PWR_BackupAccessCmd(DISABLE);// 禁用访问备份域
}

/**************************************************************************
函数功能：初始化OLED
输入参数：无
返回  值：无
**************************************************************************/
void OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // 使能GPIOA和GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // 初始化PB3、PB4、PB5为推挽输出
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 初始化PA15为推挽输出
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    OLED_RST_Clr();
    delay_ms(100);
    OLED_RST_Set(); // OLED复位

    OLED_Init_Backup_LSE(); // 初始化备份访问和LSE配置

    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭显示
    OLED_WR_Byte(0xD5, OLED_CMD); // 设置时钟分频因子,震荡频率
    OLED_WR_Byte(80, OLED_CMD);   // 设置分频因子
    OLED_WR_Byte(0xA8, OLED_CMD); // 设置驱动路数
    OLED_WR_Byte(0X3F, OLED_CMD); // 默认0x3F(1/64)
    OLED_WR_Byte(0xD3, OLED_CMD); // 设置显示偏移
    OLED_WR_Byte(0X00, OLED_CMD); // 默认偏移为0
    OLED_WR_Byte(0x40, OLED_CMD); // 设置显示开始行 [5:0],行数.
    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵设置
    OLED_WR_Byte(0x14, OLED_CMD); // bit2，开启/关闭
    OLED_WR_Byte(0x20, OLED_CMD); // 设置内存地址模式
    OLED_WR_Byte(0x02, OLED_CMD); // [1:0],00水平地址模式;01垂直地址模式;10页地址模式(默认)
    OLED_WR_Byte(0xA1, OLED_CMD); // 设置段重映射,bit0:0,0->0;1,0->127;
    OLED_WR_Byte(0xC0, OLED_CMD); // 设置COM扫描方向;bit3:0,普通模式;1,重映射模式 COM[N-1]->COM0;N:驱动路数
    OLED_WR_Byte(0xDA, OLED_CMD); // 设置COM硬件引脚配置
    OLED_WR_Byte(0x12, OLED_CMD); // [5:4]配置
    OLED_WR_Byte(0x81, OLED_CMD); // 对比度设置
    OLED_WR_Byte(0xEF, OLED_CMD); // 1~255;默认0x7F (亮度设置,越大越亮)
    OLED_WR_Byte(0xD9, OLED_CMD); // 设置预充电周期
    OLED_WR_Byte(0xf1, OLED_CMD); // [3:0],PHASE 1;[7:4],PHASE 2;
    OLED_WR_Byte(0xDB, OLED_CMD); // 设置VCOMH 电压倍率
    OLED_WR_Byte(0x30, OLED_CMD); // [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
    OLED_WR_Byte(0xA4, OLED_CMD); // 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
    OLED_WR_Byte(0xA6, OLED_CMD); // 设置显示方式;bit0:1,反相显示;0,正常显示
    OLED_WR_Byte(0xAF, OLED_CMD); // 开启显示
    OLED_Clear();                 // 清屏
}
