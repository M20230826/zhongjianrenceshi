#include "control.h"

// 宏定义
#define T 0.156f
#define L 0.1445f
#define K 622.8f

// 全局变量
u8 Flag_Target;
int Voltage_Temp, Voltage_Count, Voltage_All, sum;

/**************************************************************************
 * 函数名: Kinematic_Analysis
 * 描述: 执行差速驱动车辆的运动学分析
 * 输入: float velocity - 线速度
 *        float turn - 转向角
 * 输出: 无
 **************************************************************************/
void Kinematic_Analysis(float velocity, float turn)
{
    Target_A = (velocity + turn); 
    Target_B = (velocity - turn);  // 差速车计算两轮目标速度
}

/**************************************************************************
 * 函数名: TIM2_IRQHandler
 * 描述: 定时器2中断处理程序
 * 输入: 无
 * 输出: 无
 **************************************************************************/
void TIM2_IRQHandler(void) 
{     
    if (TIM_GetITStatus(TIM2, TIM_IT_Update)) // 检查定时器中断状态
    {   
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志位

        if (delay_flag == 1)
        {
            if (++delay_50 == 5) delay_50 = 0, delay_flag = 0; // 50ms延时处理
        }

        Encoder_Left = -Read_Encoder(3); // 读取左编码器值
        Encoder_Right = -Read_Encoder(4); // 读取右编码器值

        if (ELE_Line_Patrol_Mode)		
        {
            Sensor_Left = Get_Adc(2); // 读取左传感器值
            Sensor_Right = Get_Adc(9); // 读取右传感器值
            Sensor_Middle = Get_Adc(1); // 读取中间传感器值
            sum = Sensor_Left * 1 + Sensor_Middle * 100 + Sensor_Right * 199; // 计算加权和
            Sensor = sum / (Sensor_Left + Sensor_Middle + Sensor_Right); // 计算传感器中值
        }						 

        Get_RC(); // 获取遥控器数据

        Kinematic_Analysis(Velocity, Turn); // 运动学分析
        if (Turn_Off(Voltage) == 0) // 检查电压是否正常
        {
            Motor_A = Incremental_PI_A(Encoder_Left, Target_A); // 计算左电机PWM
            Motor_B = Incremental_PI_B(Encoder_Right, Target_B); // 计算右电机PWM
            Limit_Pwm(); // 限制PWM值
            Set_Pwm(Motor_A, Motor_B); // 设置PWM
        }		
        else  
        {
            Set_Pwm(0, 0); // 停止电机
        }	

        Voltage_Temp = Get_battery_volt(); // 获取电池电压
        Voltage_Count++; // 电压计数
        Voltage_All += Voltage_Temp; // 累加电压值
        if (Voltage_Count == 50) // 每50次计算一次平均电压
        {
            Voltage = Voltage_All / 50;
            Voltage_All = 0;
            Voltage_Count = 0;
        }

        Key(); // 处理按键事件
    }
}

/**************************************************************************
 * 函数名: Set_Pwm
 * 描述: 设置电机的PWM值
 * 输入: int motor_a - 电机A的PWM值
 *        int motor_b - 电机B的PWM值
 * 输出: 无
 **************************************************************************/
void Set_Pwm(int motor_a, int motor_b)
{
    if (ELE_Line_Patrol_Mode == 1) // 线巡逻模式下只允许后退
    {
        if (motor_a > 0) motor_a = 0;
        if (motor_b > 0) motor_b = 0;
    }

    if (motor_a > 0) AIN1 = 0, AIN2 = 1;
    else AIN1 = 1, AIN2 = 0;
    PWMA = myabs(motor_a);

    if (motor_b > 0) BIN1 = 0, BIN2 = 1;
    else BIN1 = 1, BIN2 = 0;
    PWMB = myabs(motor_b);
}

/**************************************************************************
 * 函数名: Limit_Pwm
 * 描述: 限制PWM值以防止电机过载
 * 输入: 无
 * 输出: 无
 **************************************************************************/
void Limit_Pwm(void)
{	
    int Amplitude = 6900; // PWM限制幅度
    if (Motor_A < -Amplitude) Motor_A = -Amplitude;	
    if (Motor_A > Amplitude)  Motor_A = Amplitude;	
    if (Motor_B < -Amplitude) Motor_B = -Amplitude;	
    if (Motor_B > Amplitude)  Motor_B = Amplitude;
}

/**************************************************************************
 * 函数名: Incremental_PI_A
 * 描述: 电机A的增量PI控制器
 * 输入: int Encoder - 当前编码器值
 *        int Target - 目标值
 * 输出: int - PWM值
 **************************************************************************/
int Incremental_PI_A(int Encoder, int Target)
{
    static int Bias, Pwm, Last_bias, Last_Encoder, Last_D;
    int P, I, D, J;

    Bias = Encoder - Target;

    // 比例项
    P = Velocity_KP * Bias;

    // 积分项
    I = Velocity_KI * Bias;

    // 微分项
    D = Velocity_KD * (Encoder - Last_Encoder);

    // 增量项
    J = Velocity_KJ * (D - Last_D);

    // 计算PWM
    Pwm += P + I + D + J;

    // 使用 Last_bias 进行一些操作，例如计算变化率
    int Bias_Change = Bias - Last_bias;

    // 直接使用 Bias_Change 进行控制
    Pwm += Bias_Change; // 可以根据需要调整这个公式

    // 保存上一次的值
    Last_bias = Bias;
    Last_Encoder = Encoder;
    Last_D = D;

    return Pwm;
}


/**************************************************************************
 * 函数名: Incremental_PI_B
 * 描述: 电机B的增量PI控制器
 * 输入: int Encoder - 当前编码器值
 *        int Target - 目标值
 * 输出: int - PWM值
 **************************************************************************/
int Incremental_PI_B(int Encoder, int Target)
{
    static int Bias, Pwm, Last_bias, Last_Encoder, Last_D;
    int P, I, D, J;

    Bias = Encoder - Target;

    // 比例项
    P = Velocity_KP * Bias;

    // 积分项
    I = Velocity_KI * Bias;

    // 微分项
    D = Velocity_KD * (Encoder - Last_Encoder);

    // 增量项
    J = Velocity_KJ * (D - Last_D);

    // 计算PWM
    Pwm += P + I + D + J;

    // 使用 Last_bias 进行一些操作，例如计算变化率
    int Bias_Change = Bias - Last_bias;

    // 直接使用 Bias_Change 进行控制
    Pwm += Bias_Change; // 可以根据需要调整这个公式

    // 保存上一次的值
    Last_bias = Bias;
    Last_Encoder = Encoder;
    Last_D = D;

    return Pwm;
}

/**************************************************************************
 * 函数名: 电压过低保护
 * 描述: 检查电池电压以决定是否关闭电机
 * 输入: int voltage - 当前电池电压
 * 输出: u8 - 如果电压低或停止标志位被设置则返回1，否则返回0
 **************************************************************************/
u8 Turn_Off(int voltage)
{
    u8 temp;
    if (voltage < 1000 || Flag_Stop == 1) // 如果电压低于10V或停止标志位被设置
    {	                                                
        temp = 1;    
    }
    else
    {
        temp = 0;
    }
    return temp;			
}

/**************************************************************************
 * 函数名: myabs
 * 描述: 计算整数的绝对值
 * 输入: int a - 输入整数
 * 输出: int - 输入整数的绝对值
 **************************************************************************/
int myabs(int a)
{ 		   
    int temp;
    if (a < 0)  
        temp = -a;  
    else 
        temp = a;
    return temp;
}
/**************************************************************************
 * 函数名: Get_RC
 * 描述: 处理遥控器输入
 * 输入: 无
 * 输出: 无
 **************************************************************************/
void Get_RC(void)
{
    static float Bias, Last_Bias;

    if (BLUETOOTH_Mode == 1 && APP_Flag == 1) // 蓝牙模式
    {
        if (Flag_Direction == 0) 
        {
            Velocity = 0;
            Turn = 0;   // 停止
        }
        else if (Flag_Direction == 0x41) 
        {
            Velocity = Bluetooth_Velocity;
            Turn = 0;  // 前进
        }
        else if (Flag_Direction == 0x42) 
        {
            Velocity = Bluetooth_Velocity;
            Turn = 20;  // 左前
        }
        else if (Flag_Direction == 0x43) 
        {
            Velocity = 0;
            Turn = 20;   // 左转
        }
        else if (Flag_Direction == 0x44) 
        {
            Velocity = -Bluetooth_Velocity;
            Turn = -20;  // 左后
        }
        else if (Flag_Direction == 0x45) 
        {
            Velocity = -Bluetooth_Velocity;
            Turn = 0;    // 后退
        }
        else if (Flag_Direction == 0x46) 
        {
            Velocity = -Bluetooth_Velocity;
            Turn = 20;  // 右后
        }
        else if (Flag_Direction == 0x47) 
        {
            Velocity = 0;
            Turn = -20;  // 右转
        }
        else if (Flag_Direction == 0x48) 
        {
            Velocity = Bluetooth_Velocity;
            Turn = -20;  // 右前
        }
    }
    else if (ELE_Line_Patrol_Mode) // 电感巡线模式
    {  
        Velocity = 40; // 电感巡线模式速度
        Bias = 100 - Sensor; // 计算偏差
        Turn = -myabs(Bias) * Bias * 0.0035 - Bias * 0.005 - (Bias - Last_Bias) * 1.0; // PD控制
        Last_Bias = Bias; // 保存上一次的偏差
    }
}
