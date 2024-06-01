#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"

u8 OLED_GRAM[128][8];

/**************************************************************************
Function: Refresh the OLED screen
Input   : none
Output  : none
Description: Refresh the OLED display with the contents of OLED_GRAM.
**************************************************************************/
void OLED_Refresh_Gram(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WR_Byte(0xb0 + i, OLED_CMD);    // Set page address (0~7)
        OLED_WR_Byte(0x00, OLED_CMD);        // Set display start line lower address
        OLED_WR_Byte(0x10, OLED_CMD);        // Set display start line higher address
        for (n = 0; n < 128; n++)
        {
            OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
        }
    }
}

/**************************************************************************
Function: Write byte to OLED
Input   : dat - data/command to write, cmd - data/command flag (0: command, 1: data)
Output  : none
Description: Write a byte to the OLED display (data or command).
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
Function: Turn on the OLED display
Input   : none
Output  : none
Description: Turn on the OLED display.
**************************************************************************/
void OLED_Display_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  // Set DCDC command
    OLED_WR_Byte(0x14, OLED_CMD);  // DCDC ON
    OLED_WR_Byte(0xAF, OLED_CMD);  // DISPLAY ON
}

/**************************************************************************
Function: Turn off the OLED display
Input   : none
Output  : none
Description: Turn off the OLED display.
**************************************************************************/
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD);  // Set DCDC command
    OLED_WR_Byte(0x10, OLED_CMD);  // DCDC OFF
    OLED_WR_Byte(0xAE, OLED_CMD);  // DISPLAY OFF
}

/**************************************************************************
Function: Clear the screen
Input   : none
Output  : none
Description: Clear the OLED screen, making the entire screen black.
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
    OLED_Refresh_Gram(); // Refresh to apply changes
}

/**************************************************************************
Function: Draw a point
Input   : x - x coordinate, y - y coordinate, t - 1: fill, 0: clear
Output  : none
Description: Draw or clear a point at the specified coordinates.
**************************************************************************/
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
    u8 pos, bx, temp = 0;
    if (x > 127 || y > 63) return; // Prevent out-of-bounds

    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);
    if (t)
        OLED_GRAM[x][pos] |= temp;
    else
        OLED_GRAM[x][pos] &= ~temp;
}

/**************************************************************************
Function: Display a character
Input   : x - starting x coordinate, y - starting y coordinate, chr - character to display, size - font size, mode - 0: inverse display, 1: normal display
Output  : none
Description: Display a character at the specified position.
**************************************************************************/
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    chr = chr - ' '; // Get offset value
    for (t = 0; t < size; t++)
    {
        if (size == 12)
            temp = oled_asc2_1206[chr][t]; // 12x6 font
        else
            temp = oled_asc2_1608[chr][t]; // 16x8 font

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
Function: Calculate m to the power of n
Input   : m - base number, n - exponent
Output  : result - m raised to the power of n
Description: Calculate the power of a number.
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
Function: Display a number
Input   : x - starting x coordinate, y - starting y coordinate, num - number to display, len - number of digits, size - font size
Output  : none
Description: Display a number at the specified position.
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
Function: Display a string
Input   : x - starting x coordinate, y - starting y coordinate, *p - starting address of the string
Output  : none
Description: Display a string at the specified position.
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
Function: Initialize the OLED
Input   : none
Output  : none
Description: Initialize the OLED display.
**************************************************************************/
void OLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // Enable PB clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // Push-pull output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;      // 2 MHz speed
    GPIO_Init(GPIOB, &GPIO_InitStructure);                // Initialize GPIOB

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // Enable PA clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // Initialize GPIOA

    PWR_BackupAccessCmd(ENABLE); // Enable access to backup domain
    RCC_LSEConfig(RCC_LSE_OFF);  // Disable LSE clock
    BKP_TamperPinCmd(DISABLE);   // Disable tamper pin
    PWR_BackupAccessCmd(DISABLE);// Disable access to backup domain

    OLED_RST_Clr();
    delay_ms(100);
    OLED_RST_Set();

    OLED_WR_Byte(0xAE, OLED_CMD); // Display off
    OLED_WR_Byte(0xD5, OLED_CMD); // Set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(80, OLED_CMD);   // Set divide ratio
    OLED_WR_Byte(0xA8, OLED_CMD); // Set multiplex ratio
    OLED_WR_Byte(0x3F, OLED_CMD); // 1/64 duty
    OLED_WR_Byte(0xD3, OLED_CMD); // Set display offset
    OLED_WR_Byte(0x00, OLED_CMD); // No offset
    OLED_WR_Byte(0x40, OLED_CMD); // Set start line address

    OLED_WR_Byte(0x8D, OLED_CMD); // Enable charge pump
    OLED_WR_Byte(0x14, OLED_CMD); // Enable charge pump
    OLED_WR_Byte(0x20, OLED_CMD); // Set memory addressing mode
    OLED_WR_Byte(0x02, OLED_CMD); // Page addressing mode
    OLED_WR_Byte(0xA1, OLED_CMD); // Set segment re-map
    OLED_WR_Byte(0xC0, OLED_CMD); // Set COM output scan direction
    OLED_WR_Byte(0xDA, OLED_CMD); // Set COM pins hardware configuration
    OLED_WR_Byte(0x12, OLED_CMD); // Set COM pins hardware configuration

    OLED_WR_Byte(0x81, OLED_CMD); // Set contrast control
    OLED_WR_Byte(0xEF, OLED_CMD); // Set contrast value
    OLED_WR_Byte(0xD9, OLED_CMD); // Set pre-charge period
    OLED_WR_Byte(0xF1, OLED_CMD); // Set pre-charge period
    OLED_WR_Byte(0xDB, OLED_CMD); // Set VCOMH deselect level
    OLED_WR_Byte(0x30, OLED_CMD); // Set VCOMH deselect level

    OLED_WR_Byte(0xA4, OLED_CMD); // Entire display ON
    OLED_WR_Byte(0xA6, OLED_CMD); // Set normal display
    OLED_WR_Byte(0xAF, OLED_CMD); // Display ON
    OLED_Clear();
}
