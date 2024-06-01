#ifndef __MPU6050_H
#define __MPU6050_H

#include "sys.h"

// 设备地址
#define MPU6050_ADDRESS_AD0_LOW     0x68
#define MPU6050_ADDRESS_AD0_HIGH    0x69
#define MPU6050_DEFAULT_ADDRESS     MPU6050_ADDRESS_AD0_LOW

// MPU6050 寄存器地址
#define MPU6050_RA_PWR_MGMT_1       0x6B
#define MPU6050_RA_GYRO_CONFIG      0x1B
#define MPU6050_RA_ACCEL_CONFIG     0x1C
#define MPU6050_RA_WHO_AM_I         0x75
#define MPU6050_RA_INT_PIN_CFG      0x37
#define MPU6050_RA_USER_CTRL        0x6A
#define MPU6050_RA_TEMP_OUT_H       0x41  // 添加温度寄存器高字节地址
#define MPU6050_RA_TEMP_OUT_L       0x42  // 添加温度寄存器低字节地址

// 位定义
#define MPU6050_PWR1_CLKSEL_BIT     2
#define MPU6050_PWR1_CLKSEL_LENGTH  3
#define MPU6050_PWR1_SLEEP_BIT      6
#define MPU6050_GCONFIG_FS_SEL_BIT  4
#define MPU6050_GCONFIG_FS_SEL_LENGTH 2
#define MPU6050_ACONFIG_AFS_SEL_BIT 4
#define MPU6050_ACONFIG_AFS_SEL_LENGTH 2
#define MPU6050_USERCTRL_I2C_MST_EN_BIT 5
#define MPU6050_INTCFG_I2C_BYPASS_EN_BIT 1

// 常量定义
#define MPU6050_CLOCK_PLL_YGYRO     0x01
#define MPU6050_GYRO_FS_2000        0x03
#define MPU6050_ACCEL_FS_2          0x00

// 函数声明
void MPU6050_initialize(void);
void MPU6050_setClockSource(uint8_t source);
void MPU6050_setFullScaleGyroRange(uint8_t range);
void MPU6050_setFullScaleAccelRange(uint8_t range);
void MPU6050_setSleepEnabled(uint8_t enabled);
void MPU6050_setI2CMasterModeEnabled(uint8_t enabled);
void MPU6050_setI2CBypassEnabled(uint8_t enabled);
uint8_t MPU6050_getDeviceID(void);
uint8_t MPU6050_testConnection(void);
void MPU6050_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);
void MPU6050_getlastMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);
int Read_Temperature(void);

#endif // __MPU6050_H
