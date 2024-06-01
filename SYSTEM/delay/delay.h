#ifndef __DELAY_H
#define __DELAY_H

#include "sys.h"

// Function: delay_init
// Description: Initialize the delay function
// Input: None
// Return: None
void delay_init(void);

// Function: delay_ms
// Description: Delay in milliseconds
// Input: nms - Number of milliseconds to delay
// Return: None
void delay_ms(u16 nms);

// Function: delay_us
// Description: Delay in microseconds
// Input: nus - Number of microseconds to delay
// Return: None
void delay_us(u32 nus);

#endif
