#ifndef __KF_H
#define __KF_H

#include "sys.h"

float KF_Y(float acce_X, float acce_Z, float gyro_Y);
float KF_X(float acce_Y, float acce_Z, float gyro_X);

// 矩阵乘法函数，使用内联函数优化
static inline void mul(int A_row, int A_col, int B_row, int B_col, const float *A, const float *B, float *C) {
    if (A_col != B_row) {
        printf("矩阵维度不匹配\n");
        return;
    }

    for (int i = 0; i < A_row; i++) {
        for (int j = 0; j < B_col; j++) {
            C[i * B_col + j] = 0; // 初始化
            for (int k = 0; k < A_col; k++) {
                C[i * B_col + j] += A[i * A_col + k] * B[k * B_col + j];
            }
        }
    }
}

#endif // __KF_H
