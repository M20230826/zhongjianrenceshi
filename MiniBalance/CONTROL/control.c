#include "control.h"

// 宏定义
#define T 0.156f
#define L 0.1445f
#define K 622.8f

// 全局变量
u8 Flag_Target;
int Voltage_Temp, Voltage_Count, Voltage_All, sum;
/**************************************************************************
 * Function: Kinematic_Analysis
 * Description: Perform kinematic analysis for differential drive vehicle
 * Input: float velocity - Linear velocity
 *        float turn - Turning angle
 * Output: None
 **************************************************************************/
void Kinematic_Analysis(float velocity, float turn)
{
    Target_A = (velocity + turn); 
    Target_B = (velocity - turn);  // 差速车计算两轮目标速度
}
/**************************************************************************
 * Function: TIM2_IRQHandler
 * Description: Timer 2 interrupt handler
 * Input: None
 * Output: None
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

        if (CCD_Line_Patrol_Mode) 
        {	 
            RD_TSL(); // 读取CCD数据
            Find_CCD_Zhongzhi(); // 计算CCD中值
        }

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
 * Function: Set_Pwm
 * Description: Set PWM values for motors
 * Input: int motor_a - PWM value for motor A
 *        int motor_b - PWM value for motor B
 * Output: None
 **************************************************************************/
void Set_Pwm(int motor_a, int motor_b)
{
    if (ELE_Line_Patrol_Mode == 1 || CCD_Line_Patrol_Mode == 1) // 线巡逻模式下只允许后退
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
 * Function: Limit_Pwm
 * Description: Limit PWM values to prevent over-driving the motors
 * Input: None
 * Output: None
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
 * Function: Incremental_PI_A
 * Description: Incremental PI controller for motor A
 * Input: int Encoder - Current encoder value
 *        int Target - Target value
 * Output: int - PWM value
 **************************************************************************/
int Incremental_PI_A(int Encoder, int Target)
{
    static int Bias, Pwm, Last_bias, Last_Encoder, Last_D;
    int P, I, D, J;

    // 计算偏差
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

    // 保存上一次的值
    Last_bias = Bias;
    Last_Encoder = Encoder;
    Last_D = D;

    return Pwm;
}

/**************************************************************************
 * Function: Incremental_PI_B
 * Description: Incremental PI controller for motor B
 * Input: int Encoder - Current encoder value
 *        int Target - Target value
 * Output: int - PWM value
 **************************************************************************/
int Incremental_PI_B(int Encoder, int Target)
{
    static int Bias, Pwm, Last_bias, Last_Encoder, Last_D;
    int P, I, D, J;

    // 计算偏差
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

    // 保存上一次的值
    Last_bias = Bias;
    Last_Encoder = Encoder;
    Last_D = D;

    return Pwm;
}

/**************************************************************************
 * Function: Key
 * Description: Handle key press events
 * Input: None
 * Output: None
 **************************************************************************/
void Key(void)
{	
    u8 tmp, tmp2;
    tmp = click(); 
    if (tmp == 1) 
        Flag_Stop = !Flag_Stop; // 单击按键，切换停止标志
    if (tmp == 2) 
        Flag_Show = !Flag_Show; // 双击按键，切换显示标志
    tmp2 = Long_Press();          
    if (tmp2 == 1) 
        Flag_Show = !Flag_Show; // 长按按键，切换显示标志
}

/**************************************************************************
 * Function: Turn_Off
 * Description: Check battery voltage to decide whether to turn off motors
 * Input: int voltage - Current battery voltage
 * Output: u8 - 1 if voltage is low or stop flag is set, otherwise 0
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
 * Function: myabs
 * Description: Compute the absolute value of an integer
 * Input: int a - Input integer
 * Output: int - Absolute value of input integer
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
 * Function: Get_RC
 * Description: Handle remote control inputs
 * Input: None
 * Output: None
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
    else if (CCD_Line_Patrol_Mode) // CCD巡线模式
    {
        Velocity = 40; // CCD巡线模式速度
        Bias = CCD_Zhongzhi - 64; // 计算偏差
        Turn = Bias * 0.13 + (Bias - Last_Bias) * 1; // PD控制
        Last_Bias = Bias; // 保存上一次的偏差
    }
    else if (ELE_Line_Patrol_Mode) // 电感巡线模式
    {  
        Velocity = 40; // 电感巡线模式速度
        Bias = 100 - Sensor; // 计算偏差
        Turn = -myabs(Bias) * Bias * 0.0035 - Bias * 0.005 - (Bias - Last_Bias) * 1.0; // PD控制
        Last_Bias = Bias; // 保存上一次的偏差
    }
    else if (PS2_Mode == 1 && PS2_Flag == 1) // PS2遥控模式
    {
        int LY, RX; // 左摇杆Y轴和右摇杆X轴
        int Threshold = 20; // 摇杆死区阈值
        static u8 Key1_Count = 0, Key2_Count = 0; // 按键计数器

        LY = -(PS2_LY - 128); // 左摇杆Y轴值
        RX = -(PS2_RX - 128); // 右摇杆X轴值

        if (LY > -Threshold && LY < Threshold) LY = 0;
        if (RX > -Threshold && RX < Threshold) RX = 0; // 摇杆死区处理

        Velocity = (PS2_Velocity / 128) * LY; // 计算速度

        if (Velocity >= 0)
            Turn = -(PS2_Turn_Velocity / 128) * RX; // 计算转向
        else
            Turn = (PS2_Turn_Velocity / 128) * RX; // 反向转向

        if (PS2_KEY == PSB_L1) // L1键加速
        {	
            if ((++Key1_Count) == 20)
            {
                PS2_KEY = 0;
                Key1_Count = 0;
                if ((PS2_Velocity += 5) > 40)
                    PS2_Velocity = 40;

                if ((PS2_Turn_Velocity += 20) > 80)
                    PS2_Turn_Velocity = 80;
            }
        }
        else if (PS2_KEY == PSB_R1) // R1键减速
        {
            if ((++Key2_Count) == 20)
            {
                PS2_KEY = 0;
                Key2_Count = 0;
                if ((PS2_Velocity -= 5) < 15)
                    PS2_Velocity = 15;

                if ((PS2_Turn_Velocity -= 20) < 20)
                    PS2_Turn_Velocity = 20;
            }
        }
        else
        {
            Key1_Count = 0;
            Key2_Count = 0; // 重置按键计数器
        }
    }
}
/**************************************************************************
 * Function: Find_CCD_Zhongzhi
 * Description: Calculate the middle value from CCD sensor data
 * Input: None
 * Output: None
 **************************************************************************/
void Find_CCD_Zhongzhi(void)
{ 
    static u16 i, Left, Right;
    static u16 value1_max, value1_min;
    
    value1_max = ADV[0]; // 初始化最大值
    for (i = 5; i < 123; i++) // 遍历CCD数据
    {
        if (value1_max <= ADV[i])
            value1_max = ADV[i];
    }
    
    value1_min = ADV[0]; // 初始化最小值
    for (i = 5; i < 123; i++) 
    {
        if (value1_min >= ADV[i])
            value1_min = ADV[i];
    }

    // 计算中值
    for (i = 5; i < 123; i++) 
    {
        if (ADV[i] > (value1_max - value1_min) / 2 + value1_min)
        {
            Left = i;
            break;
        }
    }
    for (i = 122; i > 5; i--) 
    {
        if (ADV[i] > (value1_max - value1_min) / 2 + value1_min)
        {
            Right = i;
            break;
        }
    }
    CCD_Zhongzhi = (Left + Right) / 2; // 计算中值
}
