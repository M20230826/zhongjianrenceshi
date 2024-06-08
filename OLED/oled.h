#ifndef __OLED_H
#define __OLED_H

#include "sys.h"

// OLED控制引脚定义
#define OLED_RST_Clr()  PBout(3) = 0   // 复位引脚
#define OLED_RST_Set()  PBout(3) = 1   
#define OLED_RS_Clr()   PAout(15) = 0   // 数据/命令选择引脚
#define OLED_RS_Set()   PAout(15) = 1   
#define OLED_SCLK_Clr() PBout(5) = 0   // 时钟引脚
#define OLED_SCLK_Set() PBout(5) = 1   
#define OLED_SDIN_Clr() PBout(4) = 0   // 数据引脚
#define OLED_SDIN_Set() PBout(4) = 1   

#define OLED_CMD  0  // 写命令
#define OLED_DATA 1  // 写数据

// OLED函数声明
void OLED_WR_Byte(u8 dat, u8 cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x, u8 y, u8 t);
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode);
void OLED_ShowNumber(u8 x, u8 y, u32 num, u8 len, u8 size);
void OLED_ShowString(u8 x, u8 y, const u8 *p);

#endif
