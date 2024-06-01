#include "MPU6050.h"
#include "IOI2C.h"
#include "usart.h"

// 全局变量
uint8_t devAddr = MPU6050_DEFAULT_ADDRESS;
uint8_t buffer[14];
short gyro[3], accel[3];
float Roll, Pitch, Yaw; 
float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
int16_t Gx_offset = 0, Gy_offset = 0, Gz_offset = 0;

void MPU6050_initialize(void) {
    MPU6050_setClockSource(MPU6050_CLOCK_PLL_YGYRO);
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);
    MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
    MPU6050_setSleepEnabled(0);
    MPU6050_setI2CMasterModeEnabled(0);
    MPU6050_setI2CBypassEnabled(0);
}

void MPU6050_setClockSource(uint8_t source) {
    IICwriteBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}

void MPU6050_setFullScaleGyroRange(uint8_t range) {
    IICwriteBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

void MPU6050_setFullScaleAccelRange(uint8_t range) {
    IICwriteBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

void MPU6050_setSleepEnabled(uint8_t enabled) {
    IICwriteBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

void MPU6050_setI2CMasterModeEnabled(uint8_t enabled) {
    IICwriteBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}

void MPU6050_setI2CBypassEnabled(uint8_t enabled) {
    IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}

uint8_t MPU6050_getDeviceID(void) {
    IICreadBytes(devAddr, MPU6050_RA_WHO_AM_I, 1, buffer);
    return buffer[0];
}

uint8_t MPU6050_testConnection(void) {
    return MPU6050_getDeviceID() == 0x68;
}

void MPU6050_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
    // 从 MPU6050 读取加速度和陀螺仪数据
}

void MPU6050_getlastMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
    // 获取最后一次读取的加速度和陀螺仪数据
}

int Read_Temperature(void) {
    float Temp;
    Temp = (I2C_ReadOneByte(devAddr, MPU6050_RA_TEMP_OUT_H) << 8) + I2C_ReadOneByte(devAddr, MPU6050_RA_TEMP_OUT_L);
    if (Temp > 32768) Temp -= 65536;
    Temp = (36.53 + Temp / 340) * 10;
    return (int)Temp;
}
