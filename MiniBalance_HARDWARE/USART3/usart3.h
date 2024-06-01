#ifndef __USART3_H
#define __USART3_H 

#include "sys.h"

extern uint8_t Usart3_Receive;

void uart3_init(uint32_t bound);
void USART3_IRQHandler(void);

#endif
