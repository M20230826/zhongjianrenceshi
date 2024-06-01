#include "DataScope_DP.h"

unsigned char DataScope_OutPut_Buffer[42] = {0}; // 输出缓冲区

/**************************************************************************
Function: Convert single precision floating-point data into 4-byte data and store it in the specified address
Input   : target: Target single precision data
          buf: Array to be written
          beg: Specifies which element of the array to write from
Output  : none
**************************************************************************/
void Float2Byte(float *target, unsigned char *buf, unsigned char beg) {
    unsigned char *point = (unsigned char*)target; // 获取float的字节地址
    for (unsigned char i = 0; i < 4; i++) {
        buf[beg + i] = point[i];
    }
}

/**************************************************************************
Function: Write the single precision floating-point data of the channel to be sent to the send buffer
Input   : data: Channel data
          channel: Select channel (1-10)
Output  : none
**************************************************************************/
void DataScope_Get_Channel_Data(float data, unsigned char channel) {
    if (channel < 1 || channel > 10) return; // 通道号必须在1到10之间
    Float2Byte(&data, DataScope_OutPut_Buffer, 1 + (channel - 1) * 4);
}

/**************************************************************************
Function: Generate frame format that datascope v1.0 can recognize correctly
Input   : channel_number: Number of channels to send
Output  : Returns the number of send buffer data
**************************************************************************/
unsigned char DataScope_Data_Generate(unsigned char channel_number) {
    if (channel_number < 1 || channel_number > 10) return 0; // 通道号必须在1到10之间

    DataScope_OutPut_Buffer[0] = '$'; // 帧头
    DataScope_OutPut_Buffer[4 * channel_number + 1] = 4 * channel_number + 1;

    return 4 * channel_number + 2;
}
