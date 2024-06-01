
#include "led.h"

/**************************************************************************
Function: LED initialization
Input   : none
Output  : none
Description: Initialize the LED GPIO pin.
**************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // Enable GPIOA clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;             // Set pin 4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // Set as push-pull output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // Set speed to 50 MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // Initialize GPIOA pin 4
}

/**************************************************************************
Function: LED flashing
Input   : time - Flicker frequency
Output  : none
Description: Make the LED flash with a specified frequency.
**************************************************************************/
void Led_Flash(u16 time)
{
    static int temp;
    if (time == 0) 
    {
        LED = 0; // Turn off LED
    }
    else if (++temp == time) 
    {
        LED = ~LED; // Toggle LED state
        temp = 0;
    }
}
