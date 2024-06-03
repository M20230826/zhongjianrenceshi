
#include "stm32f10x.h"                  // Device header
#include "sys.h"


// 全局变量
u8 Mode = 0, BLUETOOTH_Mode = 1, APP_RX, APP_Flag;
u8 Flag_Way = 0, Flag_Show = 0, Flag_Stop = 1, Flag_Next;
int Voltage;
float Bluetooth_Velocity = 30;
int Encoder_Left, Encoder_Right;
uint8_t KEY_Scan(void);

int TargetVelocity = 0, TargetCircle = 0, CurrentPosition = 0, Encoder = 0, PWM = 0; // 目标速度、目标圈数、当前位置、编码器读数、PWM控制变量
float Velocity_Kp = 80, Velocity_Ki = 10, Velocity_Kd = 0; // 速度PID参数
float Position_Kp = 120, Position_Ki = 0.1, Position_Kd = 400; // 位置PID参数
int MotorRun = 0; // 电机运行标志位

/**************************************************************************
函数功能：OLED显示函数
输入参数：无
返回  值：无
**************************************************************************/
void Oled_Show(void)
{  
    OLED_Refresh_Gram(); // 刷新显示
    OLED_ShowString(0, 0, "V_P Feedback"); // 显示标题

    // 显示目标位置
    OLED_ShowString(0, 10, "Target_P :"); 
    int target_position = TargetCircle * 1040 * 1.04; // 计算目标位置
    if (target_position >= 0) 
    {
        OLED_ShowString(80, 10, "+");
        OLED_ShowNumber(90, 10, target_position, 5, 12);
    }
    else 
    {
        OLED_ShowString(80, 10, "-");
        OLED_ShowNumber(90, 10, -target_position, 5, 12);
    }

    // 显示当前位置
    OLED_ShowString(0, 20, "Current_P:"); 
    if (CurrentPosition >= 0) 
    {
        OLED_ShowString(80, 20, "+");
        OLED_ShowNumber(90, 20, CurrentPosition, 5, 12);
    }
    else 
    {
        OLED_ShowString(80, 20, "-");
        OLED_ShowNumber(90, 20, -CurrentPosition, 5, 12);
    }

    // 显示目标速度
    OLED_ShowString(0, 30, "Target_V:"); 		
    if (TargetVelocity >= 0)
    {
        OLED_ShowString(80, 30, "+");
        OLED_ShowNumber(90, 30, TargetVelocity, 5, 12);
    }
    else
    {
        OLED_ShowString(80, 30, "-");
        OLED_ShowNumber(90, 30, -TargetVelocity, 5, 12);
    }

    // 显示当前速度
    OLED_ShowString(0, 40, "Current_V:"); 		
    if (Encoder >= 0)
    {
        OLED_ShowString(80, 40, "+");
        OLED_ShowNumber(90, 40, Encoder, 5, 12);
    }
    else
    {
        OLED_ShowString(80, 40, "-");
        OLED_ShowNumber(90, 40, -Encoder, 5, 12);
    }

    // 显示PWM值
    OLED_ShowString(0, 50, "PWM      :"); 		
    if (PWM >= 0)
    {
        OLED_ShowString(80, 50, "+");
        OLED_ShowNumber(90, 50, PWM, 5, 12);
    }
    else
    {
        OLED_ShowString(80, 50, "-");
        OLED_ShowNumber(90, 50, -PWM, 5, 12);
    }
}

/**************************************************************************
函数功能：主函数
输入参数：无
返回  值：无
**************************************************************************/
int main(void)
{
    // 系统初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置中断优先级组
    delay_init();                // 初始化延时函数
    JTAG_Set(JTAG_SWD_DISABLE);  // 禁用JTAG，启用SWD
    LED_Init();                  // 初始化LED
    OLED_Init();                 // 初始化OLED显示屏
    // uart_init(9600);          // 初始化串口，波特率9600
    MotorEncoder_Init();	        // 初始化编码器，使用定时器4
    TB6612_Init(7199, 0);        // 初始化电机驱动，使用定时器3
    EncoderRead_TIM2(7199, 99);  // 初始化定时器2，用于读取编码器数据，10ms读取一次

    delay_ms(2000);              // 延时等待初始化完成
    
    while (1)
    {		
        delay_ms(5);
        LED = 0; // LED闪烁
        if (KEY_Scan()) MotorRun = !MotorRun; // 按键扫描，切换电机运行状态
                                
        Oled_Show(); // OLED显示更新
        
        // 串口输出目标速度和当前速度 (如果需要)
        // printf("TargetVelocity: %d\r\n", TargetVelocity);
        // printf("Encoder: %d\r\n", Encoder);
        // printf("Speed: %.3f r/s\r\n", Encoder / 1.04);
    }
}
