#include "key.h"

/**************************************************************************
Function: Key initialization
Input   : none
Output  : none
Description: Initialize the key GPIO pin.
**************************************************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // Enable GPIOA clock
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;             // Set pin 5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;         // Set as input with pull-up
    GPIO_Init(GPIOA, &GPIO_InitStructure);                // Initialize GPIOA pin 5
}

/**************************************************************************
Function: Key scan
Input   : time - Double click waiting time
Output  : 0 - No action, 1 - Click, 2 - Double click
Description: Scan the key to detect single or double clicks.
**************************************************************************/
u8 click_N_Double(u8 time)
{
    static u8 flag_key, count_key, double_key;    
    static u16 count_single, Forever_count;

    if (KEY == 0)  Forever_count++;   // Increment forever count if key is pressed
    else           Forever_count = 0; // Reset forever count if key is released

    if (KEY == 0 && flag_key == 0) flag_key = 1; // Detect key press

    if (count_key == 0)
    {
        if (flag_key == 1) 
        {
            double_key++;
            count_key = 1; // Register a key press
        }
        if (double_key == 2) 
        {   // Detect double click
            double_key = 0;
            count_single = 0;
            return 2; // Return double click detected
        }
    }

    if (KEY == 1) flag_key = 0, count_key = 0; // Reset flags if key is released

    if (double_key == 1)
    {
        count_single++;
        if (count_single > time && Forever_count < time)
        {
            double_key = 0;
            count_single = 0; // Single click detected within time limit
            return 1; // Return single click detected
        }
        if (Forever_count > time)
        {
            double_key = 0;
            count_single = 0; // Reset counters if time limit exceeded
        }
    }    
    return 0; // No click detected
}

/**************************************************************************
Function: Key scan
Input   : none
Output  : 0 - No action, 1 - Click
Description: Scan the key to detect single clicks.
**************************************************************************/
u8 click(void)
{
    static u8 flag_key = 1; // Key press flag

    if (flag_key && KEY == 0) // Detect key press
    {
        flag_key = 0;
        return 1; // Key press detected
    }
    else if (KEY == 1) flag_key = 1; // Reset flag if key is released

    return 0; // No key press detected
}

/**************************************************************************
Function: Long press detection
Input   : none
Output  : 0 - No action, 1 - Long press for 2 seconds
Description: Detect long press of the key.
**************************************************************************/
u8 Long_Press(void)
{
    static u16 Long_Press_count, Long_Press;

    if (Long_Press == 0 && KEY == 0) Long_Press_count++; // Increment long press count if key is pressed
    else                             Long_Press_count = 0; // Reset long press count if key is released

    if (Long_Press_count > 200) // Detect long press (assuming 10ms scan interval)
    {
        Long_Press = 1;    
        Long_Press_count = 0;
        return 1; // Long press detected
    }                

    if (Long_Press == 1) Long_Press = 0; // Reset long press flag

    return 0; // No long press detected
}

/**************************************************************************
Function: Select mode based on key input
Input   : none
Output  : flag - Mode selection status
Description: Select different modes based on single or double key clicks.
**************************************************************************/
u8 select(void)
{
    u8 i;
    static u8 flag = 1;

    oled_show_once(); // Display on OLED
    i = click_N_Double(50); // Detect single or double click

    if (i == 1)
    { 
        Mode++;
        if (Mode == 0) BLUETOOTH_Mode = 1, CCD_Line_Patrol_Mode = 0, ELE_Line_Patrol_Mode = 0, PS2_Mode = 0;
        if (Mode == 1) BLUETOOTH_Mode = 0, CCD_Line_Patrol_Mode = 0, ELE_Line_Patrol_Mode = 0, PS2_Mode = 1;    
        if (Mode == 2) CCD_Line_Patrol_Mode = 1, BLUETOOTH_Mode = 0, ELE_Line_Patrol_Mode = 0, PS2_Mode = 0;
        if (Mode == 3) ELE_Line_Patrol_Mode = 1, BLUETOOTH_Mode = 0, CCD_Line_Patrol_Mode = 0, PS2_Mode = 0;
        if (Mode == 4) Mode = 0, BLUETOOTH_Mode = 1, CCD_Line_Patrol_Mode = 0, ELE_Line_Patrol_Mode = 0, PS2_Mode = 0;    
    }

    if (i == 2) OLED_Clear(), flag = 0; // Clear OLED screen and exit mode selection

    return flag;    
}
