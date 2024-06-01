
#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

// Key pin definition
// PAin(5) is a macro that directly reads the input state of GPIOA pin 5
#define KEY PAin(5)

// Function prototypes
void KEY_Init(void);          // Initialize key GPIO
u8 click_N_Double(u8 time);   // Scan for single and double clicks
u8 click(void);               // Scan for single clicks
u8 Long_Press(void);          // Scan for long press
u8 select(void);              // Select mode based on key input

#endif
