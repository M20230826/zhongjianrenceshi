#include "stm32f10x.h"
#include "sys.h"

// 全局变量定义
u8 Mode = 0, BLUETOOTH_Mode = 1, ELE_Line_Patrol_Mode = 0, CCD_Line_Patrol_Mode = 0, PS2_Mode = 0, PS2_Flag = 0, APP_RX, APP_Flag;
u8 Flag_Way = 0, Flag_Show = 0, Flag_Stop = 1, Flag_Next;
int Encoder_Left, Encoder_Right;
int Flag_Direction;
float Velocity, Velocity_Set, Angle_Set, Turn;
int Motor_A, Motor_B, Servo, Target_A, Target_B;
int Voltage;
float Show_Data_Mb;
u8 delay_50, delay_flag;
float Velocity_KP = 15, Velocity_KI = 7, Velocity_KD = 1, Velocity_KJ = 0.1;
u16 ADV[128] = {0};
int PS2_LX, PS2_LY, PS2_RX, PS2_RY, PS2_KEY;
float Bluetooth_Velocity = 30, PS2_Velocity = 30, PS2_Turn_Velocity = 0;
u8 CCD_Zhongzhi, CCD_Yuzhi, PID_Send, Flash_Send;
int Sensor_Left, Sensor_Middle, Sensor_Right, Sensor;
u16 PID_Parameter[10], Flash_Parameter[10];

/**************************************************************************
 * Function: main
 * Description: Main function to initialize and run the system
 * Input: None
 * Output: None
 **************************************************************************/
int main(void)
{ 
    // 设置中断优先级组
    MY_NVIC_PriorityGroupConfig(2);

    // 初始化延时函数
    delay_init();

    // 禁用JTAG，启用SWD
    JTAG_Set(JTAG_SWD_DISABLE);
    JTAG_Set(SWD_ENABLE);

    // 初始化LED
    LED_Init();

    // 初始化按键
    KEY_Init();

    // 初始化PWM，频率为10kHz
    MiniBalance_PWM_Init(7199, 0);

    // 初始化串口1，波特率115200
    uart_init(115200);

    // 初始化串口3，波特率9600
    uart3_init(9600);

    // 初始化TIM3编码器
    Encoder_Init_TIM3();

    // 初始化TIM4编码器
    Encoder_Init_TIM4();

    // 初始化ADC
    Adc_Init();

    // 初始化IIC
    IIC_Init();

    // 初始化OLED
    OLED_Init();

    // 初始化PS2
    PS2_Init();

    // 选择模式
    while (select()) {}

    // 延时300ms
    delay_ms(300);

    // 根据模式初始化CCD或电感传感器
    if (CCD_Line_Patrol_Mode)
        ccd_Init();
    else if (ELE_Line_Patrol_Mode)
        ele_Init();

    // 读取Flash中的PID参数
    Flash_Read();

    // 初始化舵机PWM，频率为100Hz
    Servo_PWM_Init(9999, 71);

    while (1)
    {
        if (Flag_Show == 0) // 使用MiniBalance APP或OLED显示
        {
            PS2_Read(); // 读取PS2数据
            APP_Show(); // 显示APP数据
            oled_show(); // 显示OLED数据
        }
        else // 使用MiniBalance数据记录模式
        {
            DataScope(); // 数据记录
        }

        // 延时处理
        delay_flag = 1;
        delay_50 = 0;
        while (delay_flag);
    }
}
