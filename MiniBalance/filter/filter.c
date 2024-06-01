#include "filter.h"

// 时间间隔，单位为秒
static const float dt = 0.005; 

// 卡尔曼滤波器结构体
typedef struct {
    float angle;
    float bias;
    float rate;
    float P[2][2];
} KalmanFilter;

// 初始化卡尔曼滤波器
void Kalman_Init(KalmanFilter* kf) {
    kf->angle = 0.0f;
    kf->bias = 0.0f;
    kf->rate = 0.0f;
    kf->P[0][0] = 1.0f;
    kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f;
    kf->P[1][1] = 1.0f;
}

// 卡尔曼滤波器更新
float Kalman_Update(KalmanFilter* kf, float newAngle, float newRate) {
    const float Q_angle = 0.001f;
    const float Q_gyro = 0.003f;
    const float R_angle = 0.5f;

    kf->rate = newRate - kf->bias;
    kf->angle += dt * kf->rate;

    kf->P[0][0] += dt * (dt * kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + Q_angle);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += Q_gyro * dt;

    float S = kf->P[0][0] + R_angle;
    float K[2];
    K[0] = kf->P[0][0] / S;
    K[1] = kf->P[1][0] / S;

    float y = newAngle - kf->angle;
    kf->angle += K[0] * y;
    kf->bias += K[1] * y;

    float P00_temp = kf->P[0][0];
    float P01_temp = kf->P[0][1];

    kf->P[0][0] -= K[0] * P00_temp;
    kf->P[0][1] -= K[0] * P01_temp;
    kf->P[1][0] -= K[1] * P00_temp;
    kf->P[1][1] -= K[1] * P01_temp;

    return kf->angle;
}

static KalmanFilter kf_x;
static KalmanFilter kf_y;

// 初始化卡尔曼滤波器
void Filters_Init() {
    Kalman_Init(&kf_x);
    Kalman_Init(&kf_y);
}

// 卡尔曼滤波器X轴
float Kalman_Filter_x(float Accel, float Gyro) {
    return Kalman_Update(&kf_x, Accel, Gyro);
}

// 卡尔曼滤波器Y轴
float Kalman_Filter_y(float Accel, float Gyro) {
    return Kalman_Update(&kf_y, Accel, Gyro);
}

// 一阶互补滤波器X轴
float Complementary_Filter_x(float angle_m, float gyro_m) {
    static float angle = 0.0f;
    const float K1 = 0.02f;
    angle = K1 * angle_m + (1.0f - K1) * (angle + gyro_m * dt);
    return angle;
}

// 一阶互补滤波器Y轴
float Complementary_Filter_y(float angle_m, float gyro_m) {
    static float angle = 0.0f;
    const float K1 = 0.02f;
    angle = K1 * angle_m + (1.0f - K1) * (angle + gyro_m * dt);
    return angle;
}
