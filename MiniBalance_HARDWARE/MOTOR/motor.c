#include "motor.h"

/**************************************************************************
Function: Initialize Motor Interface
Input   : none
Output  : none
Description:
 - This function initializes the motor interface by configuring GPIO pins on GPIOB.
Parameters:
 - None
**************************************************************************/
void MiniBalance_Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clock for GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // Configure GPIO pins on GPIOB for motor control
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  // Pins for motor control
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // Output mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);                // Initialize GPIOB for motor control
}

/**************************************************************************
Function: Initialize PWM to drive motor
Input   : arr - Auto reload value, psc - Prescaler coefficient
Output  : none
Description:
 - This function initializes PWM to drive the motor by configuring TIM1 timer as PWM output mode.
Parameters:
 - arr: Auto reload value
 - psc: Prescaler coefficient
**************************************************************************/
void MiniBalance_PWM_Init(u16 arr, u16 psc)
{		 		
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // Initialize motor interface
    MiniBalance_Motor_Init();

    // Enable clock for TIM1 and GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure GPIO pins on GPIOA for TIM1 CH1 and CH4 as PWM output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;  // TIM_CH1, TIM_CH4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // Alternate function output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure TIM1 timer for PWM output
    TIM_TimeBaseStructure.TIM_Period = arr;  // Set auto reload value
    TIM_TimeBaseStructure.TIM_Prescaler = psc;  // Set prescaler coefficient
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // Up counter mode
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);  // Initialize TIM1 time base unit

    // Configure TIM1 channel 1 and 4 for PWM output
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // PWM mode 1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // Enable output
    TIM_OCInitStructure.TIM_Pulse = 0;  // Set initial pulse value
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // Output polarity high
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  // Initialize TIM1 channel 1
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);  // Initialize TIM1 channel 4

    // Enable TIM1 main output
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // Enable preload for TIM1 channel 1 and 4
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // Enable auto-reload preload
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    // Enable TIM1
    TIM_Cmd(TIM1, ENABLE);
}

/**************************************************************************
Function: Initialize PWM signal for servo motor
Input   : arr - Auto reload value, psc - Prescaler coefficient
Output  : none
Description:
 - This function initializes PWM signal for servo motor by configuring TIM2 timer as PWM output mode.
Parameters:
 - arr: Auto reload value
 - psc: Prescaler coefficient
**************************************************************************/
void Servo_PWM_Init(u16 arr, u16 psc) 
{		 	
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;   
    NVIC_InitTypeDef NVIC_InitStruct;	
	
    // Enable clock for TIM2 and GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure GPIO pin on GPIOA for TIM2 CH1 as PWM output
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;          // Alternate function output
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;                // PA0
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configure TIM2 timer for PWM output
    TIM_TimeBaseInitStruct.TIM_Period = arr;              // Set auto reload value
    TIM_TimeBaseInitStruct.TIM_Prescaler = psc;           // Set prescaler coefficient
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  // Up counter mode
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;         // Clock division
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);       // Initialize TIM2

    // Configure TIM2 channel 1 for PWM output
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;             // PWM mode 1
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; // Enable output
    TIM_OCInitStruct.TIM_Pulse = 0;                            // Set initial pulse value
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;     // Output polarity high
    TIM_OC1Init(TIM2, &TIM_OCInitStruct);                       // Initialize TIM2 channel 1

    // Enable preload for TIM2 channel 1
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);   // Enable preload for CH1

    // Configure and enable TIM2 interrupt
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    // Enable auto-reload preload
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    // Enable TIM2 update interrupt
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // Enable TIM2
    TIM_Cmd(TIM2, ENABLE);  // Enable TIM2
    TIM2->CCR1 = 1500;      // Set initial duty cycle
}
