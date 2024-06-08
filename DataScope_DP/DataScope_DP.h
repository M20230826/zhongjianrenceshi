#ifndef __DATA_PROTOCOL_H
#define __DATA_PROTOCOL_H

extern unsigned char DataScope_OutPut_Buffer[42]; // 输出缓冲区

void DataScope_Get_Channel_Data(float data, unsigned char channel); // 获取通道数据
unsigned char DataScope_Data_Generate(unsigned char channel_number); // 生成数据帧

#endif
