#include "show.h"

// 全局变量定义
unsigned char i;          // 计数变量
unsigned char Send_Count; // 串口需要发送的数据个数
float Vol;                // 电压值

/**************************************************************************
 * 函数功能: 显示OLED内容
 * 输入参数: 无
 * 返回值  : 无
 **************************************************************************/
void oled_show(void)
{
    // 显示蓝牙模式下的信息
    if (BLUETOOTH_Mode) {
        OLED_ShowString(0, 0, "SPEED");
        OLED_ShowNumber(45, 0, Bluetooth_Velocity, 3, 12);
        OLED_ShowString(0, 10, "RX");
        OLED_ShowNumber(30, 10, PID_Send, 3, 12);
    }
    // 显示编码器和电压信息
    OLED_ShowString(0, 20, "EncoLEFT");
    if (Encoder_Left < 0) {
        OLED_ShowString(80, 20, "-");
        OLED_ShowNumber(95, 20, -Encoder_Left, 5, 12);
    } else {
        OLED_ShowString(80, 20, "+");
        OLED_ShowNumber(95, 20, Encoder_Left, 5, 12);
    }

    OLED_ShowString(0, 30, "EncoRIGHT");
    if (Encoder_Right < 0) {
        OLED_ShowString(80, 30, "-");
        OLED_ShowNumber(95, 30, -Encoder_Right, 5, 12);
    } else {
        OLED_ShowString(80, 30, "+");
        OLED_ShowNumber(95, 30, Encoder_Right, 5, 12);
    }

    OLED_ShowString(0, 40, "VOLTAGE");
    OLED_ShowString(68, 40, ".");
    OLED_ShowString(90, 40, "V");
    OLED_ShowNumber(55, 40, Voltage / 100, 2, 12);
    OLED_ShowNumber(78, 40, Voltage % 100, 2, 12);
    if (Voltage % 100 < 10) {
        OLED_ShowNumber(72, 40, 0, 2, 12);
    }

    if (Flag_Stop == 0) {
        OLED_ShowString(103, 40, "O-N");
    } else {
        OLED_ShowString(103, 40, "OFF");
    }

    // 显示当前模式
    OLED_ShowString(0, 50, "MODE-");
    if (BLUETOOTH_Mode) {
        OLED_ShowString(40, 50, "APP");
    } else if (ELE_Line_Patrol_Mode) {
        OLED_ShowString(40, 50, "ELE");
    }

    // 显示车辆类型
    if (Diff_Car == 1) {
        OLED_ShowString(80, 50, "Diff");
    }

    // 刷新OLED显示
    OLED_Refresh_Gram();
}

/**************************************************************************
 * 函数功能: 发送数据到APP进行显示
 * 输入参数: 无
 * 返回值  : 无
 **************************************************************************/
void APP_Show(void)
{    
    static u8 flag;
    int app_2, app_3, app_4;

    // 处理电压数据
    app_4 = (Voltage - 740) * 2 / 3;
    if (app_4 < 0) app_4 = 0;
    if (app_4 > 100) app_4 = 100;

    // 处理编码器数据
    app_3 = Encoder_Right * 1.1;
    if (app_3 < 0) app_3 = -app_3;

    app_2 = Encoder_Left * 1.1;
    if (app_2 < 0) app_2 = -app_2;

    flag = !flag;

    if (PID_Send == 1) {
        // 发送PID参数
        printf("{C%d:%d:%d:%d:%d:%d:%d:%d:%d}$", (int)Bluetooth_Velocity, (int)Velocity_KP, (int)Velocity_KI, 0, 0, 0, 0, 0, 0);
        PID_Send = 0;
    } else if (flag == 0) {
        // 发送处理后的编码器数据
        printf("{A%d:%d:%d}$", (u8)app_2, (u8)app_3, app_4);
    } else {
        // 发送电压和编码器原始数据
        printf("{B%d:%d:%d}$", (int)Voltage, Encoder_Left, Encoder_Right);
    }
}

/**************************************************************************
 * 函数功能: 发送数据到DataScope进行调试
 * 输入参数: 无
 * 返回值  : 无
 **************************************************************************/
void DataScope(void)
{   
    // 获取各通道数据
    DataScope_Get_Channel_Data(Encoder_Left, 1);
    DataScope_Get_Channel_Data(Encoder_Right, 2);

    // 生成数据并发送
    Send_Count = DataScope_Data_Generate(2);
    for (i = 0; i < Send_Count; i++) {
        while ((USART1->SR & 0X40) == 0);
        USART1->DR = DataScope_OutPut_Buffer[i];
    }
}

/**************************************************************************
 * 函数功能: 在OLED上绘制一条垂直的点线
 * 输入参数: x - 横坐标
 *           y - 纵坐标
 *           t - 点的状态（点亮或熄灭）
 * 返回值  : 无
 **************************************************************************/
void OLED_DrawPoint_Shu(u8 x, u8 y, u8 t)
{ 
    u8 i = 0;
    OLED_DrawPoint(x, y, t);
    for (i = 0; i < 8; i++)
    {
        OLED_DrawPoint(x, y + i, t);
    }
}

/**************************************************************************
 * 函数功能: 系统启动时在OLED上显示初始信息
 * 输入参数: 无
 * 返回值  : 无
 **************************************************************************/
void oled_show_once(void)
{
    OLED_ShowString(0, 0, "TO Press UserKey");
    OLED_ShowString(0, 10, "TO Select Mode");
    OLED_ShowString(0, 20, "Current Mode Is");

    if (BLUETOOTH_Mode) {
        OLED_ShowString(50, 30, "APP");
    } 
	
    OLED_ShowString(0, 40, "D-Press User Key");
    OLED_ShowString(0, 50, "TO End Selection");

    // 刷新OLED显示
    OLED_Refresh_Gram();
}
