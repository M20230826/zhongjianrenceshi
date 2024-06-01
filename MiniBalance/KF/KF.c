#include "kf.h"
#include <math.h>
#include <stdio.h>

// 卡尔曼滤波器更新
float Kalman_Filter(float acce_1, float acce_2, float gyro, float x_hat[2][1], float p_hat[2][2]) {
    const float Ts = 0.005; // 采样时间
    const float I[2][2] = {{1, 0}, {0, 1}};
    const float A[2][2] = {{1, -Ts}, {0, 1}};
    const float B[2][1] = {{Ts}, {0}};
    const float C[1][2] = {{1, 0}};
    const float Q[2][2] = {{1e-10, 0}, {0, 1e-10}};
    const float R[1][1] = {{1e-4}};
    const float A_T[2][2] = {{1, 0}, {-Ts, 1}};
    const float C_T[2][1] = {{1}, {0}};

    float u[1][1] = {{gyro}};
    float y = atan2(-acce_1, acce_2);

    float x_hat_minus[2][1] = {0};
    float p_hat_minus[2][2] = {0};
    float K[2][1] = {0};

    float temp_1[2][1] = {0};
    float temp_2[2][1] = {0};
    float temp_3[2][2] = {0};
    float temp_4[2][2] = {0};
    float temp_5[1][2] = {0};
    float temp_6[1][1] = {0};

    // 预测
    mul(2, 2, 2, 1, (const float *)A, (const float *)x_hat, (float *)temp_1);
    mul(2, 1, 1, 1, (const float *)B, (const float *)u, (float *)temp_2);
    x_hat_minus[0][0] = temp_1[0][0] + temp_2[0][0];
    x_hat_minus[1][0] = temp_1[1][0] + temp_2[1][0];

    mul(2, 2, 2, 2, (const float *)A, (const float *)p_hat, (float *)temp_3);
    mul(2, 2, 2, 2, (const float *)temp_3, (const float *)A_T, (float *)temp_4);
    p_hat_minus[0][0] = temp_4[0][0] + Q[0][0];
    p_hat_minus[0][1] = temp_4[0][1] + Q[0][1];
    p_hat_minus[1][0] = temp_4[1][0] + Q[1][0];
    p_hat_minus[1][1] = temp_4[1][1] + Q[1][1];

    // 更新
    mul(1, 2, 2, 2, (const float *)C, (const float *)p_hat_minus, (float *)temp_5);
    mul(1, 2, 2, 1, (const float *)temp_5, (const float *)C_T, (float *)temp_6);
    temp_6[0][0] = 1.0f / (temp_6[0][0] + R[0][0]);
    mul(2, 2, 2, 1, (const float *)p_hat_minus, (const float *)C_T, (float *)temp_1);
    mul(2, 1, 1, 1, (const float *)temp_1, (const float *)temp_6, (float *)K);

    mul(1, 2, 2, 1, (const float *)C, (const float *)x_hat_minus, (float *)temp_6);
    temp_6[0][0] = y - temp_6[0][0];
    mul(2, 1, 1, 1, (const float *)K, (const float *)temp_6, (float *)temp_1);
    x_hat[0][0] = x_hat_minus[0][0] + temp_1[0][0];
    x_hat[1][0] = x_hat_minus[1][0] + temp_1[1][0];

    mul(2, 1, 1, 2, (const float *)K, (const float *)C, (float *)temp_3);
    temp_3[0][0] = I[0][0] - temp_3[0][0];
    temp_3[0][1] = I[0][1] - temp_3[0][1];
    temp_3[1][0] = I[1][0] - temp_3[1][0];
    temp_3[1][1] = I[1][1] - temp_3[1][1];
    mul(2, 2, 2, 2, (const float *)temp_3, (const float *)p_hat_minus, (float *)p_hat);

    return x_hat[0][0];
}

// 卡尔曼滤波器X轴
float KF_X(float acce_Y, float acce_Z, float gyro_X) {
    static float x_hat[2][1] = {0};
    static float p_hat[2][2] = {{1, 0}, {0, 1}};
    return Kalman_Filter(acce_Y, acce_Z, gyro_X, x_hat, p_hat);
}

// 卡尔曼滤波器Y轴
float KF_Y(float acce_X, float acce_Z, float gyro_Y) {
    static float x_hat[2][1] = {0};
    static float p_hat[2][2] = {{1, 0}, {0, 1}};
    return Kalman_Filter(acce_X, acce_Z, gyro_Y, x_hat, p_hat);
}
