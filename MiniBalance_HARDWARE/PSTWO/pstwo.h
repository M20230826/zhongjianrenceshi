#ifndef __PSTWO_H
#define __PSTWO_H

#include "delay.h"
#include "sys.h"

/*********************************************************
 * GPIO Pin Definitions
 *********************************************************/
#define DI       PCin(13)          // Data Input: PC13
#define DO_H     PCout(14) = 1     // Data Output High: PC14
#define DO_L     PCout(14) = 0     // Data Output Low: PC14
#define CS_H     PCout(15) = 1     // Chip Select High: PC15
#define CS_L     PCout(15) = 0     // Chip Select Low: PC15
#define CLK_H    PAout(2) = 1      // Clock High: PA2
#define CLK_L    PAout(2) = 0      // Clock Low: PA2

/*********************************************************
 * GPIO Port and Pin Definitions for PS2
 *********************************************************/
#define PS2_DI_GPIO_PORT     GPIOC               // GPIO Port for DI
#define PS2_DI_GPIO_CLK      RCC_APB2Periph_GPIOC // GPIO Clock for DI
#define PS2_DI_GPIO_PIN      GPIO_Pin_13          // GPIO Pin for DI

#define PS2_DO_GPIO_PORT     GPIOC               // GPIO Port for DO
#define PS2_DO_GPIO_CLK      RCC_APB2Periph_GPIOC // GPIO Clock for DO
#define PS2_DO_GPIO_PIN      GPIO_Pin_14          // GPIO Pin for DO

#define PS2_CS_GPIO_PORT     GPIOC               // GPIO Port for CS
#define PS2_CS_GPIO_CLK      RCC_APB2Periph_GPIOC // GPIO Clock for CS
#define PS2_CS_GPIO_PIN      GPIO_Pin_15          // GPIO Pin for CS

#define PS2_CLK_GPIO_PORT    GPIOA               // GPIO Port for CLK
#define PS2_CLK_GPIO_CLK     RCC_APB2Periph_GPIOA // GPIO Clock for CLK
#define PS2_CLK_GPIO_PIN     GPIO_Pin_2           // GPIO Pin for CLK

/*********************************************************
 * Button Constants
 *********************************************************/
#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16

#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      16

/*********************************************************
 * Stick Values
 *********************************************************/
#define PSS_RX 5  // Right Stick X-axis
#define PSS_RY 6  // Right Stick Y-axis
#define PSS_LX 7  // Left Stick X-axis
#define PSS_LY 8  // Left Stick Y-axis

/*********************************************************
 * External Variables
 *********************************************************/
extern u8 Data[9];
extern u16 MASK[16];
extern u16 Handkey;

/*********************************************************
 * Function Prototypes
 *********************************************************/
void PS2_Init(void);                  // PS2 Initialization
u8 PS2_RedLight(void);                // Check if in Red Light Mode
void PS2_ReadData(void);              // Read Data from PS2
void PS2_Cmd(u8 CMD);                 // Send Command to PS2
u8 PS2_DataKey(void);                 // Get Key Data
u8 PS2_AnologData(u8 button);         // Get Analog Data for a Button
void PS2_ClearData(void);             // Clear Data Buffer
void PS2_Vibration(u8 motor1, u8 motor2); // Control Vibration Motors

void PS2_EnterConfing(void);          // Enter Configuration Mode
void PS2_TurnOnAnalogMode(void);      // Turn on Analog Mode
void PS2_VibrationMode(void);         // Set Vibration Mode
void PS2_ExitConfing(void);           // Exit Configuration Mode
void PS2_SetInit(void);               // Initialize PS2 Settings
void PS2_Read(void);                  // Read PS2 Control Data

#endif // __PSTWO_H
