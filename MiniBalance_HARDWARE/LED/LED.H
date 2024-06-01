
#ifndef __LED_H
#define __LED_H

#include "sys.h"

// LED pin definition
// PAout(4) is a macro that directly manipulates the output register of GPIOA pin 4
#define LED PAout(4)  

// Function prototypes
void LED_Init(void);  // Initialize LED
void Led_Flash(u16 time); // Flash LED with a specified frequency

#endif
