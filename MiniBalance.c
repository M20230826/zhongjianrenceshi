#include "stm32f10x.h"
#include "sys.h"

// 定义全局符号 __use_no_argv
//__attribute__((used)) static void __use_no_argv(void) { __asm(".global __use_no_argv\n\t__use_no_argv:"); }

void TIM2_Init(void);

// 全局变量定义
u8 Mode = 0, BLUETOOTH_Mode = 1, ELE_Line_Patrol_Mode = 0, APP_RX, APP_Flag;
u8 Flag_Way = 0, Flag_Show = 0, Flag_Stop = 1, Flag_Next;
int Encoder_Left, Encoder_Right;
int Flag_Direction;
float Velocity, Velocity_Set, Angle_Set, Turn;
int Motor_A, Motor_B, Target_A, Target_B;
int Voltage;
u8 delay_50, delay_flag;
float Velocity_KP = 15, Velocity_KI = 7, Velocity_KD = 1, Velocity_KJ = 0.1;
u16 ADV[128] = {0};
float Bluetooth_Velocity = 30;
u8 PID_Send, Flash_Send;
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
    MY_NVIC_PriorityGroupConfig(2);			// 设置中断优先级组
    delay_init();							// 初始化延时函数
    JTAG_Set(JTAG_SWD_DISABLE);				// 禁用JTAG
    JTAG_Set(SWD_ENABLE);					// 启用SWD
    LED_Init();								// 初始化LED
    KEY_Init();								// 初始化按键
    MiniBalance_PWM_Init(7199, 0);			// 初始化PWM，频率为10kHz
    uart_init(115200);						// 初始化串口1，波特率115200
    uart3_init(9600);						// 初始化串口3，波特率9600
    Encoder_Init_TIM3();					// 初始化TIM3编码器
    Encoder_Init_TIM4();					// 初始化TIM4编码器 
    Adc_Init();								// 初始化ADC
    IIC_Init();								// 初始化IIC
    MPU6050_initialize();					// 初始化MPU6050
    DMP_Init();								// 初始化DMP   
    OLED_Init();							// 初始化OLED 
    
    while (select()) {}						// 选择模式 
    delay_ms(300);							// 延时300ms   
    if (ELE_Line_Patrol_Mode)				// 根据模式初始化电感传感器
        ele_Init();

    while (1)
    {
        TIM2_Init();
        Read_DMP();             
		
        if (Flag_Show == 0) // 使用MiniBalance APP或OLED显示
        {
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
