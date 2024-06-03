#include "motorencoder.h"

// 外部变量声明
extern int Encoder, CurrentPosition; // 当前编码器值和当前位置
extern int TargetVelocity, TargetCircle, CurrentPosition, Encoder, PWM; // 目标速度、目标圈数、当前位置、编码器值、PWM控制变量
extern float Velocity_Kp, Velocity_Ki, Velocity_Kd; // 速度PID参数
extern float Position_Kp, Position_Ki, Position_Kd; // 位置PID参数
extern int MotorRun; // 电机运行标志位

/**************************************************************************
函数功能：初始化电机编码器
输入参数：无
返回  值：无
**************************************************************************/
void MotorEncoder_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // GPIO初始化结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // 定时器基础初始化结构体
    TIM_ICInitTypeDef TIM_ICInitStructure; // 定时器输入捕获初始化结构体

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // 使能TIM4时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOB时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // TIM4_CH1和TIM4_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入模式
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化GPIO

    TIM_TimeBaseStructure.TIM_Period = 0xffff; // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // 预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 计数器向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); // 初始化TIM4

    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); // 编码器模式配置
    TIM_ICStructInit(&TIM_ICInitStructure); // 初始化捕获结构体
    TIM_ICInitStructure.TIM_ICFilter = 10; // 输入捕获滤波
    TIM_ICInit(TIM4, &TIM_ICInitStructure); // 初始化TIM4输入捕获

    TIM_ClearFlag(TIM4, TIM_FLAG_Update); // 清除更新标志
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); // 使能更新中断
    TIM_SetCounter(TIM4, 0); // 计数器清零

    TIM_Cmd(TIM4, ENABLE); // 使能TIM4
}

/**************************************************************************
函数功能：读取TIM4编码器值
输入参数：无
返回  值：编码器当前值
**************************************************************************/
int Read_Encoder(void)
{
    int Encoder_TIM;
    Encoder_TIM = TIM4->CNT; // 读取计数器值
    if (Encoder_TIM > 0xefff) Encoder_TIM = Encoder_TIM - 0xffff; // 转换为有符号数
    TIM4->CNT = 0; // 清零计数器
    return Encoder_TIM; // 返回编码器值
}

/**************************************************************************
函数功能：TIM4中断处理函数
输入参数：无
返回  值：无
**************************************************************************/
void TIM4_IRQHandler(void)
{
    if (TIM4->SR & 0X0001) // 检查更新中断标志
    {
        // 中断处理代码
    }
    TIM4->SR &= ~(1 << 0); // 清除更新中断标志
}
/**************************************************************************
函数功能：初始化TIM2以读取编码器数据
输入参数：arr 自动重装载值
          psc 预分频值
返回  值：无
**************************************************************************/
void EncoderRead_TIM2(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue; // 定时器基础初始化结构体
    NVIC_InitTypeDef NVIC_InitStrue; // 中断控制器初始化结构体

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 使能TIM2时钟

    TIM_TimeBaseInitStrue.TIM_Period = arr; // 自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler = psc; // 预分频值
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStrue); // 初始化TIM2

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 使能更新中断

    NVIC_InitStrue.NVIC_IRQChannel = TIM2_IRQn; // TIM2中断通道
    NVIC_InitStrue.NVIC_IRQChannelCmd = ENABLE; // 使能中断
    NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStrue.NVIC_IRQChannelSubPriority = 1; // 响应优先级
    NVIC_Init(&NVIC_InitStrue); // 初始化NVIC

    TIM_Cmd(TIM2, ENABLE); // 使能TIM2
}

/**************************************************************************
函数功能：TIM2中断处理函数
输入参数：无
返回  值：无
**************************************************************************/
void TIM2_IRQHandler()
{
    static int PWM_P, PWM_V, TimeCount; // 位置控制PWM值、速度控制PWM值、时间计数
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == 1) // 检查更新中断标志
    {
        Encoder = Read_Encoder(); // 读取当前编码器值
        CurrentPosition += Encoder; // 更新当前位置

        if (MotorRun) // 如果电机运行
        {
            TimeCount++; // 时间计数
            if (TimeCount <= 600) TargetCircle = 15, TargetVelocity = 40; // 前6秒目标位置和速度
            else if (600 < TimeCount) TargetCircle = 30, TargetVelocity = 20; // 6秒后目标位置和速度
            PWM_P = Position_FeedbackControl(TargetCircle, CurrentPosition); // 位置反馈控制
            PWM_P = PWM_Restrict(PWM_P, TargetVelocity); // 限制PWM值
            PWM_V = PWM_P / 76; // PWM值转换为速度值
            PWM = Velocity_FeedbackControl(PWM_V, Encoder); // 速度反馈控制

            SetPWM(PWM); // 设置PWM
        }
        else PWM = 0, SetPWM(PWM); // 如果电机停止，PWM设为0

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除更新中断标志
    }
}
/**************************************************************************
函数功能：速度反馈控制（PI控制）
输入参数：TargetVelocity 目标速度
          CurrentVelocity 当前速度
返回  值：控制量
**************************************************************************/
int Velocity_FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
    int Bias; // 偏差
    static int ControlVelocity, Last_bias; // 控制量和上一次偏差

    Bias = TargetVelocity - CurrentVelocity; // 计算偏差

    ControlVelocity += Velocity_Kp * (Bias - Last_bias) + Velocity_Ki * Bias; // PI控制公式
    Last_bias = Bias; // 更新上一次偏差

    return ControlVelocity; // 返回控制量
}

/**************************************************************************
函数功能：位置反馈控制（PID控制）
输入参数：Circle 目标圈数
          CurrentPosition 当前的位置
返回  值：控制量
**************************************************************************/
int Position_FeedbackControl(float Circle, int CurrentPosition)
{
    float TargetPosition, Bias, ControlVelocity; // 目标位置、偏差和控制量
    static float Last_bias, Integral_Bias; // 上一次偏差和积分偏差

    TargetPosition = Circle * 1040 * 1.04; // 计算目标位置

    Bias = TargetPosition - CurrentPosition; // 计算偏差
    Integral_Bias += Bias;
    if (Integral_Bias > 970) Integral_Bias = 970; // 限制积分偏差
    if (Integral_Bias < -970) Integral_Bias = -970; // 限制积分偏差

    ControlVelocity = Position_Kp * Bias + Position_Ki * Integral_Bias + Position_Kd * (Bias - Last_bias); // PID控制公式
    Last_bias = Bias; // 更新上一次偏差

    return ControlVelocity; // 返回控制量
}

/**************************************************************************
函数功能：限制PWM值
输入参数：PWM_P 位置控制的PWM值
          TargetVelocity 目标速度
返回  值：受限的PWM值
**************************************************************************/
int PWM_Restrict(int PWM_P, int TargetVelocity)
{
    if (PWM_P > +TargetVelocity * 76) PWM_P = +TargetVelocity * 76;
    else if (PWM_P < -TargetVelocity * 76) PWM_P = -TargetVelocity * 76;
    return PWM_P;
}
