#ifndef __MOTOR_H
#define __MOTOR_H

#include <sys.h>	 

// Define PWM signals for motor control
#define PWMB   TIM1->CCR4  // PWM signal for motor B (PA11)
#define BIN2   PBout(12)    // Control signal for motor B IN2 (PB12)
#define BIN1   PBout(13)    // Control signal for motor B IN1 (PB13)
#define AIN2   PBout(15)    // Control signal for motor A IN2 (PB15)
#define AIN1   PBout(14)    // Control signal for motor A IN1 (PB14)
#define PWMA   TIM1->CCR1  // PWM signal for motor A (PA8)

#define SERVO   TIM2->CCR1  // PWM signal for servo motor

// Function prototypes
void MiniBalance_PWM_Init(u16 arr, u16 psc);   // Initialize PWM signals for motor control
void MiniBalance_Motor_Init(void);             // Initialize motor interface
void Servo_PWM_Init(u16 arr, u16 psc);         // Initialize PWM signal for servo motor

#endif
