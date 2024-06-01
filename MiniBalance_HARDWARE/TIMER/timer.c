#include "timer.h"

///**************************************************************************
//Function: Timer 2 channel 2 input capture initialization
//Input   : arr - Auto reload value, psc - Clock prescaled frequency
//Output  : none
//Description:
// - Initialize Timer 2 channel 2 for input capture
//Parameters:
// - arr: Auto reload value
// - psc: Clock prescaled frequency
//**************************************************************************/	 		
TIM_ICInitTypeDef TIM2_ICInitStructure;

void TIM2_Cap_Init(u16 arr, u16 psc)	
{	 
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    // Enable TIM2 clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   // Enable GPIOA clock
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // PA1 input mode  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);																																																							
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     // PA3 output mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Initialize Timer 2
    TIM_TimeBaseStructure.TIM_Period = arr; // Set auto reload value
    TIM_TimeBaseStructure.TIM_Prescaler = psc;    // Set clock prescaler
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // Clock division: TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // Up counter mode
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); // Initialize TIMx time base unit
    
    // Initialize input capture for TIM2
    TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; // Select channel 2 for input capture
    TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;    // Rising edge polarity
    TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;    // Input capture prescaler: 1
    TIM2_ICInitStructure.TIM_ICFilter = 0x00;    // Input capture filter: no filter
    TIM_ICInit(TIM2, &TIM2_ICInitStructure);
    
    // Enable TIM2 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // TIM2 interrupt
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // Preemption priority: 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  // Subpriority: 1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // Enable IRQ channel
    NVIC_Init(&NVIC_InitStructure);  // Initialize NVIC for TIM2 interrupt
    TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC2, ENABLE); // Enable update and CC2 interrupt
    TIM_Cmd(TIM2, ENABLE);    // Enable TIM2
}

///**************************************************************************
//Function: Ultrasonic receiving echo function
//Input   : none
//Output  : none
//Description:
// - Function for ultrasonic receiving echo
//**************************************************************************/	 	
//u16 TIM2CH2_CAPTURE_STA, TIM2CH2_CAPTURE_VAL;

//void Read_Distane(void)        
//{   
//    GPIO_SetBits(GPIOA, GPIO_Pin_3);         
//    delay_us(15);  
//    GPIO_ResetBits(GPIOA, GPIO_Pin_3);	
//    if (TIM2CH2_CAPTURE_STA & 0x80) // If the capture status is ready
//    {
//        Distance = TIM2CH2_CAPTURE_STA & 0x3F; 
//        Distance *= 65536;    // Convert time to distance
//        Distance += TIM2CH2_CAPTURE_VAL;    
//        Distance = Distance * 170 / 1000;  // Convert time to distance in mm
//        TIM2CH2_CAPTURE_STA = 0;    // Reset capture status
//    }				
//}

///**************************************************************************
//Function: Pulse width reading interruption of ultrasonic echo
//Input   : none
//Output  : none
//Description:
// - Interrupt function for pulse width reading of ultrasonic echo
//**************************************************************************/	 
//void TIM2_IRQHandler(void)
//{ 		    		  			    
//    u16 tsr;
//    tsr = TIM2->SR;
//    if ((TIM2CH2_CAPTURE_STA & 0x80) == 0) // If not capturing
//    {
//        if (tsr & 0x01) // If update event
//        {	    
//            if (TIM2CH2_CAPTURE_STA & 0x40) // If waiting for falling edge
//            {
//                if ((TIM2CH2_CAPTURE_STA & 0x3F) == 0x3F) // If too many rising edges
//                {
//                    TIM2CH2_CAPTURE_STA |= 0x80;  // Start a new capture
//                    TIM2CH2_CAPTURE_VAL = 0xFFFF;
//                } 
//                else 
//                {
//                    TIM2CH2_CAPTURE_STA++;
//                }
//            }	 
//        }
//        if (tsr & 0x04) // If input capture 2 event
//        {	
//            if (TIM2CH2_CAPTURE_STA & 0x40) // If already captured a rising edge
//            {	  	
//                TIM2CH2_CAPTURE_STA |= 0x80;  // Start a new capture for falling edge
//                TIM2CH2_CAPTURE_VAL = TIM2->CCR2;    
//                TIM2->CCER &= ~(1 << 5);    // CC2P=0, set as rising edge
//            }
//            else // If not started yet, set as falling edge
//            {
//                TIM2CH2_CAPTURE_STA = 0;    
//                TIM2CH2_CAPTURE_VAL = 0;
//                TIM2CH2_CAPTURE_STA |= 0x40;    
//                TIM2->CNT = 0;    
//                TIM2->CCER |= 1 << 5;    
//            }		    
//        }			     	    					   
//    }
//    TIM2->SR = 0;    // Clear interrupt flag
//}
