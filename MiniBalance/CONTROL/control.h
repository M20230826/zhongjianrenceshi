#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

/***********************************************
* Brand: WHEELTEC
* Website: wheeltec.net
* Taobao shop: shop114407458.taobao.com 
* Aliexpress: https://minibalance.aliexpress.com/store/4455017
* Version: V1.0
* Update: 2023-01-04
* All rights reserved
***********************************************/

// 常量定义
#define Pi 3.14159265

// 函数声明

/**************************************************************************
 * Function: Kinematic_Analysis
 * Description: Perform kinematic analysis for differential drive vehicle
 * Input: float velocity - Linear velocity
 *        float angle - Turning angle
 * Output: None
 **************************************************************************/
void Kinematic_Analysis(float velocity, float angle);

/**************************************************************************
 * Function: TIM2_IRQHandler
 * Description: Timer 2 interrupt handler
 * Input: None
 * Output: None
 **************************************************************************/
void TIM2_IRQHandler(void);

/**************************************************************************
 * Function: Set_Pwm
 * Description: Set PWM values for motors
 * Input: int motor_a - PWM value for motor A
 *        int motor_b - PWM value for motor B
 * Output: None
 **************************************************************************/
void Set_Pwm(int motor_a, int motor_b);

/**************************************************************************
 * Function: Key
 * Description: Handle key press events
 * Input: None
 * Output: None
 **************************************************************************/
void Key(void);

/**************************************************************************
 * Function: Limit_Pwm
 * Description: Limit PWM values to prevent over-driving the motors
 * Input: None
 * Output: None
 **************************************************************************/
void Limit_Pwm(void);

/**************************************************************************
 * Function: Turn_Off
 * Description: Check battery voltage to decide whether to turn off motors
 * Input: int voltage - Current battery voltage
 * Output: u8 - 1 if voltage is low or stop flag is set, otherwise 0
 **************************************************************************/
u8 Turn_Off(int voltage);

/**************************************************************************
 * Function: Get_Angle
 * Description: Get angle data
 * Input: u8 way - The way to get angle data
 * Output: None
 **************************************************************************/
void Get_Angle(u8 way);

/**************************************************************************
 * Function: myabs
 * Description: Compute the absolute value of an integer
 * Input: int a - Input integer
 * Output: int - Absolute value of input integer
 **************************************************************************/
int myabs(int a);

/**************************************************************************
 * Function: Incremental_PI_A
 * Description: Incremental PI controller for motor A
 * Input: int Encoder - Current encoder value
 *        int Target - Target value
 * Output: int - PWM value
 **************************************************************************/
int Incremental_PI_A(int Encoder, int Target);

/**************************************************************************
 * Function: Incremental_PI_B
 * Description: Incremental PI controller for motor B
 * Input: int Encoder - Current encoder value
 *        int Target - Target value
 * Output: int - PWM value
 **************************************************************************/
int Incremental_PI_B(int Encoder, int Target);

/**************************************************************************
 * Function: Get_RC
 * Description: Handle remote control inputs
 * Input: None
 * Output: None
 **************************************************************************/
void Get_RC(void);

/**************************************************************************
 * Function: Find_CCD_Zhongzhi
 * Description: Calculate the middle value from CCD sensor data
 * Input: None
 * Output: None
 **************************************************************************/
void Find_CCD_Zhongzhi(void);

#endif // __CONTROL_H
