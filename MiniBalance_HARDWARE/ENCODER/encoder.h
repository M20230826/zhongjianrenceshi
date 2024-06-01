#ifndef __ENCODER_H
#define __ENCODER_H

#include <sys.h>	 

#define ENCODER_TIM_PERIOD (u16)(65535)   // Encoder timer period set to 65535 for F103 series with 16-bit timer

// Function prototypes
void Encoder_Init_TIM3(void);  // Initialize TIM3 for encoder interface mode
void Encoder_Init_TIM4(void);  // Initialize TIM4 for encoder interface mode
int Read_Encoder(u8 TIMX);      // Read encoder count per unit time for specified timer
void TIM4_IRQHandler(void);     // TIM4 interrupt service function
void TIM3_IRQHandler(void);     // TIM3 interrupt service function

#endif
