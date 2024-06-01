#include "encoder.h"
#include "stm32f10x_gpio.h"

/**************************************************************************
Function: Initialize TIM3 to encoder interface mode
Input   : none
Output  : none
Description:
 - This function initializes TIM3 in encoder interface mode by configuring GPIO pins on GPIOA.
Parameters:
 - None
**************************************************************************/
void Encoder_Init_TIM3(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Enable clock for TIM3 and GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    // Configure GPIO pins on GPIOA for encoder interface
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	// Pins for encoder interface
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Input floating mode
    GPIO_Init(GPIOA, &GPIO_InitStructure);	// Initialize GPIOA for encoder interface
    
    // Initialize TIM3 timer
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0; // Prescaler
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; // Set period value
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // Clock division
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // Up counter mode  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // Configure TIM3 for encoder interface mode
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // Initialize TIM3 input capture
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;	// Input capture filter
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // Clear update flag
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    
    // Enable update interrupt
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    // Reset counter
    TIM_SetCounter(TIM3, 0);
    
    // Enable TIM3
    TIM_Cmd(TIM3, ENABLE); 
}

/**************************************************************************
Function: Initialize TIM4 to encoder interface mode
Input   : none
Output  : none
Description:
 - This function initializes TIM4 in encoder interface mode by configuring GPIO pins on GPIOB.
Parameters:
 - None
**************************************************************************/
void Encoder_Init_TIM4(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Enable clock for TIM4 and GPIOB
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
    // Configure GPIO pins on GPIOB for encoder interface
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	// Pins for encoder interface
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Input floating mode
    GPIO_Init(GPIOB, &GPIO_InitStructure);	// Initialize GPIOB for encoder interface
    
    // Initialize TIM4 timer
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // Prescaler
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; // Set period value
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // Clock division
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // Up counter mode  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // Configure TIM4 for encoder interface mode
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // Initialize TIM4 input capture
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;	// Input capture filter
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
    // Clear update flag
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    
    // Enable update interrupt
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    // Reset counter
    TIM_SetCounter(TIM4, 0);
    
    // Enable TIM4
    TIM_Cmd(TIM4, ENABLE); 
}

/**************************************************************************
Function: Read encoder count per unit time
Input   : TIMX - Timer
Output  : Encoder_TIM - Encoder count
Description:
 - This function reads the encoder count per unit time for the specified timer (TIM2, TIM3, TIM4).
Parameters:
 - TIMX: Timer selection (2, 3, 4)
Return:
 - Encoder_TIM: Encoder count per unit time
**************************************************************************/
int Read_Encoder(u8 TIMX)
{
    int Encoder_TIM;    
    switch(TIMX)
    {
        case 2:  
            Encoder_TIM = (short)TIM2->CNT;  // Read encoder count for TIM2
            TIM2->CNT = 0;  // Reset counter
            break;
        case 3:  
            Encoder_TIM = (short)TIM3->CNT;  // Read encoder count for TIM3
            TIM3->CNT = 0;  // Reset counter
            break;	
        case 4:  
            Encoder_TIM = (short)TIM4->CNT;  // Read encoder count for TIM4
            TIM4->CNT = 0;  // Reset counter
            break;
        default: 
            Encoder_TIM = 0;  // Default value if invalid timer selection
    }
    return Encoder_TIM;  // Return the encoder count
}

/**************************************************************************
Function: TIM4 interrupt service function
Input   : none
Output  : none
Description:
 - This function serves as the interrupt service routine for TIM4.
Parameters:
 - None
**************************************************************************/
void TIM4_IRQHandler(void)
{ 		    		  			    
    if(TIM4->SR & 0x0001)  // Check if update interrupt flag is set
    {    				   				     	    	
        // Perform necessary actions for TIM4 interrupt
        
    }				   
    TIM4->SR &= ~(1 << 0);  // Clear update interrupt flag	    
}

/**************************************************************************
Function: TIM3 interrupt service function
Input   : none
Output  : none
Description:
 - This function serves as the interrupt service routine for TIM3.
Parameters:
 - None
**************************************************************************/
void TIM3_IRQHandler(void)
{ 		    		  			    
    if(TIM3->SR & 0x0001)  // Check if update interrupt flag is set
    {    				   				     	    	
        // Perform necessary actions for TIM3 interrupt
        
    }				   
    TIM3->SR &= ~(1 << 0);  // Clear update interrupt flag	    
}
