#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"

// Function: uart_init
// Description: Initialize the UART communication with the specified baud rate.
// Input: bound - Baud rate for communication
// Return: None
void uart_init(u32 bound);

#endif
