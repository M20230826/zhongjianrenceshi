#include "DataScope_dp.h"

unsigned char DataScope_OutPut_Buffer[42] = {0}; // 输出缓冲区

/**************************************************************************
函数功能：将单精度浮点数转换为4字节数据并存储在指定地址
输入参数：target - 目标单精度数据
          buf - 要写入的数组
          beg - 指定从数组的哪个元素开始写入
输出参数：无
**************************************************************************/
void Float2Byte(float *target, unsigned char *buf, unsigned char beg) {
    unsigned char *point = (unsigned char*)target; // 获取float的字节地址
    for (unsigned char i = 0; i < 4; i++) {
        buf[beg + i] = point[i];
    }
}

/**************************************************************************
函数功能：将要发送的通道的单精度浮点数数据写入发送缓冲区
输入参数：data - 通道数据
          channel - 选择的通道（1-10）
输出参数：无
**************************************************************************/
void DataScope_Get_Channel_Data(float data, unsigned char channel) {
    if (channel < 1 || channel > 10) return; // 通道号必须在1到10之间
    Float2Byte(&data, DataScope_OutPut_Buffer, 1 + (channel - 1) * 4);
}

/**************************************************************************
函数功能：生成DataScope v1.0可以正确识别的帧格式
输入参数：channel_number - 要发送的通道数量
输出参数：返回发送缓冲区的数据数量
**************************************************************************/
unsigned char DataScope_Data_Generate(unsigned char channel_number) {
    if (channel_number < 1 || channel_number > 10) return 0; // 通道号必须在1到10之间

    DataScope_OutPut_Buffer[0] = '$'; // 帧头
    DataScope_OutPut_Buffer[4 * channel_number + 1] = 4 * channel_number + 1;

    return 4 * channel_number + 2;
}
