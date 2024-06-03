#ifndef __OLED_H
#define __OLED_H			  	 

#include "sys.h"

/**************************************************************************
模块名称：OLED显示
品牌：WHEELTEC
网址：http://shop114407458.taobao.com/
**************************************************************************/

//----------------- OLED 引脚定义 ---------------- 
#define OLED_RST_Clr() PBout(3)=0   // 复位引脚
#define OLED_RST_Set() PBout(3)=1   // 复位引脚

#define OLED_RS_Clr() PAout(15)=0   // 数据/命令选择引脚
#define OLED_RS_Set() PAout(15)=1   // 数据/命令选择引脚

#define OLED_SCLK_Clr() PBout(5)=0  // 时钟引脚
#define OLED_SCLK_Set() PBout(5)=1  // 时钟引脚

#define OLED_SDIN_Clr() PBout(4)=0  // 数据引脚
#define OLED_SDIN_Set() PBout(4)=1  // 数据引脚

#define OLED_CMD  0	// 写命令
#define OLED_DATA 1	// 写数据

// OLED 函数声明
void OLED_WR_Byte(u8 dat, u8 cmd);	         // 向 OLED 写入一个字节的数据或命令
void OLED_Display_On(void);                // 打开 OLED 显示
void OLED_Display_Off(void);               // 关闭 OLED 显示
void OLED_Refresh_Gram(void);		   		 // 刷新 OLED 显示
void OLED_Init(void);                      // 初始化 OLED
void OLED_Clear(void);                     // 清屏
void OLED_DrawPoint(u8 x, u8 y, u8 t);     // 画点
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode); // 显示一个字符
void OLED_ShowNumber(u8 x, u8 y, u32 num, u8 len, u8 size); // 显示一个数字
void OLED_ShowString(u8 x, u8 y, const u8 *p);	          // 显示一个字符串

#endif // __OLED_H
