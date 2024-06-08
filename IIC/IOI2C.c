#include "ioi2c.h"
#include "sys.h"
#include "delay.h"

/**************************************************************************
函数名: IIC_Init
描述  : IIC引脚初始化
输入  : 无
输出  : 无
说明  :
 - 该函数初始化GPIOB上的I2C通信引脚。
**************************************************************************/
void IIC_Init(void)
{			
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOB时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;	// I2C引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					      // 初始化GPIOB用于I2C
}

/**************************************************************************
函数名: IIC_Start
描述  : 模拟IIC起始信号
输入  : 无
输出  : 1表示成功，0表示失败
说明  :
 - 该函数模拟I2C起始信号。
**************************************************************************/
int IIC_Start(void)
{
    SDA_OUT();     // 设置SDA为输出
    IIC_SDA = 1;
    if (!READ_SDA) return 0;	
    IIC_SCL = 1;
    delay_us(1);
    IIC_SDA = 0; // START: 当CLK为高时，DATA由高变低
    if (READ_SDA) return 0;
    delay_us(1);
    IIC_SCL = 0; // 拉低时钟准备进行I2C通信
    return 1;
}

/**************************************************************************
函数名: IIC_Stop
描述  : 模拟IIC停止信号
输入  : 无
输出  : 无
说明  :
 - 该函数模拟I2C停止信号。
**************************************************************************/
void IIC_Stop(void)
{
    SDA_OUT(); // 设置SDA为输出
    IIC_SCL = 0;
    IIC_SDA = 0; // STOP: 当CLK为高时，DATA由低变高
    delay_us(1);
    IIC_SCL = 1; 
    IIC_SDA = 1; // 释放I2C停止信号
    delay_us(1);							   	
}

/**************************************************************************
函数名: IIC_Wait_Ack
描述  : IIC等待应答信号
输入  : 无
输出  : 0表示未收到应答，1表示收到应答
说明  :
 - 该函数等待I2C应答信号。
**************************************************************************/
int IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    SDA_IN();      // 设置SDA为输入  
    IIC_SDA = 1;
    delay_us(1);	   
    IIC_SCL = 1;
    delay_us(1);	 
    while (READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 50)
        {
            IIC_Stop();
            return 0;
        }
        delay_us(1);
    }
    IIC_SCL = 0; // 拉低时钟准备进行通信	   
    return 1;  
}
/**************************************************************************
函数名: IIC_Ack
描述  : IIC发送应答信号
输入  : 无
输出  : 无
说明  :
 - 该函数在I2C通信过程中发送应答信号。
**************************************************************************/
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    delay_us(1);
    IIC_SCL = 1;
    delay_us(1);
    IIC_SCL = 0;
}

/**************************************************************************
函数名: IIC_NAck
描述  : IIC发送非应答信号
输入  : 无
输出  : 无
说明  :
 - 该函数在I2C通信过程中发送非应答信号。
**************************************************************************/
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    delay_us(1);
    IIC_SCL = 1;
    delay_us(1);
    IIC_SCL = 0;
}

/**************************************************************************
函数名: IIC_Send_Byte
描述  : IIC发送一个字节
输入  : txd - 要发送的字节数据
输出  : 无
说明  :
 - 该函数在I2C通信过程中发送一个字节的数据。
**************************************************************************/
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    SDA_OUT(); 	    
    IIC_SCL = 0; // 开始时钟信号
    for (t = 0; t < 8; t++)
    {              
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1; 	  
        delay_us(1);   
        IIC_SCL = 1;
        delay_us(1); 
        IIC_SCL = 0;	
        delay_us(1);
    }	 
} 
/**************************************************************************
函数名: i2cWrite
描述  : IIC写数据到寄存器
输入  : addr - 设备地址, reg - 寄存器地址, len - 字节数, data - 要写入的数据
输出  : 0表示写入成功，1表示失败
说明  :
 - 该函数使用I2C通信将数据写入设备的寄存器。
**************************************************************************/
int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
    int i;
    if (!IIC_Start())
        return 1;
    IIC_Send_Byte(addr << 1);
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    for (i = 0; i < len; i++) {
        IIC_Send_Byte(data[i]);
        if (!IIC_Wait_Ack()) {
            IIC_Stop();
            return 0;
        }
    }
    IIC_Stop();
    return 0;
}

/**************************************************************************
函数名: i2cRead
描述  : IIC读取寄存器数据
输入  : addr - 设备地址, reg - 寄存器地址, len - 字节数, *buf - 读取的数据
输出  : 0表示读取成功，1表示失败
说明  :
 - 该函数使用I2C通信从设备的寄存器读取数据。
**************************************************************************/
int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    if (!IIC_Start())
        return 1;
    IIC_Send_Byte(addr << 1);
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte((addr << 1) + 1);
    IIC_Wait_Ack();
    while (len) {
        if (len == 1)
            *buf = IIC_Read_Byte(0);
        else
            *buf = IIC_Read_Byte(1);
        buf++;
        len--;
    }
    IIC_Stop();
    return 0;
}

/**************************************************************************
函数名: IIC_Read_Byte
描述  : IIC读取一个字节
输入  : ack (发送应答信号与否: 1 - 发送, 0 - 不发送)
输出  : receive (读取的数据)
说明  :
 - 该函数在I2C通信过程中读取一个字节的数据。
**************************************************************************/
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN(); // 设置SDA为输入
    for (i = 0; i < 8; i++)
    {
        IIC_SCL = 0;
        delay_us(2);
        IIC_SCL = 1;
        receive <<= 1;
        if (READ_SDA)
            receive++;
        delay_us(2);
    }
    if (ack)
        IIC_Ack(); // 发送ACK
    else
        IIC_NAck(); // 发送NACK
    return receive;
}
/**************************************************************************
函数名: I2C_ReadOneByte
描述  : IIC读取一个字节
输入  : I2C_Addr - 设备I2C地址, addr - 寄存器地址
输出  : res - 读取的数据
说明  :
 - 该函数从特定设备的特定寄存器读取一个字节的数据。
**************************************************************************/
unsigned char I2C_ReadOneByte(unsigned char I2C_Addr, unsigned char addr)
{
    unsigned char res = 0;
    
    IIC_Start();    
    IIC_Send_Byte(I2C_Addr);	   // 发送设备地址
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);        // 发送寄存器地址
    IIC_Wait_Ack();	  
    IIC_Start();
    IIC_Send_Byte(I2C_Addr + 1); // 切换到读模式			   
    IIC_Wait_Ack();
    res = IIC_Read_Byte(0);	   
    IIC_Stop(); // 停止并释放总线

    return res;
}

/**************************************************************************
函数名: IICreadBytes
描述  : IIC读取多个字节
输入  : dev - 目标设备I2C地址, reg - 寄存器地址, length - 字节数, *data - 读取的数据
输出  : count - 读取的字节数
说明  :
 - 该函数从特定设备的特定寄存器读取多个字节的数据。
**************************************************************************/
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data)
{
    u8 count = 0;
    
    IIC_Start();
    IIC_Send_Byte(dev);	   // 发送设备地址
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);   // 发送寄存器地址
    IIC_Wait_Ack();	  
    IIC_Start();
    IIC_Send_Byte(dev + 1);  // 切换到读模式	
    IIC_Wait_Ack();
    
    for(count = 0; count < length; count++){
        if(count != length - 1)   
            data[count] = IIC_Read_Byte(1);  // 读取并发送ACK
        else                  
            data[count] = IIC_Read_Byte(0);  // 读取最后一个字节并发送NACK
    }
    IIC_Stop(); // 停止并释放总线
    return count;
}
/**************************************************************************
函数名: IICwriteBytes
描述  : IIC写入多个字节
输入  : dev - 目标设备I2C地址, reg - 寄存器地址, length - 字节数, *data - 要写入的数据
输出  : 1表示成功，0表示失败
说明  :
 - 该函数将多个字节的数据写入特定设备的特定寄存器。
**************************************************************************/
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data)
{
    u8 count = 0;
    IIC_Start();
    IIC_Send_Byte(dev);	   // 发送设备地址
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);   // 发送寄存器地址
    IIC_Wait_Ack();
    for(count = 0; count < length; count++){
        IIC_Send_Byte(data[count]); 
        IIC_Wait_Ack(); 
    }
    IIC_Stop(); // 停止并释放总线

    return 1; //status == 0;
}

/**************************************************************************
函数名: IICreadByte
描述  : IIC读取一个字节
输入  : dev - 目标设备I2C地址, reg - 寄存器地址, *data - 读取的数据
输出  : 1表示成功，0表示失败
说明  :
 - 该函数从特定设备的特定寄存器读取一个字节的数据。
**************************************************************************/
u8 IICreadByte(u8 dev, u8 reg, u8 *data)
{
    *data = I2C_ReadOneByte(dev, reg);
    return 1;
}

/**************************************************************************
函数名: IICwriteByte
描述  : IIC写入一个字节
输入  : dev - 目标设备I2C地址, reg - 寄存器地址, data - 要写入的数据
输出  : 1表示成功
说明  :
 - 该函数将一个字节的数据写入特定设备的特定寄存器。
**************************************************************************/
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
{
    return IICwriteBytes(dev, reg, 1, &data);
}
/**************************************************************************
函数名: IICwriteBit
描述  : IIC写入一个位
输入  : dev - 目标设备I2C地址, reg - 寄存器地址, bitNum - 位位置, data - 要写入的位值
输出  : 1表示成功
说明  :
 - 该函数将一个位的数据写入特定设备的特定寄存器。
**************************************************************************/
unsigned char IICwriteBit(unsigned char dev, unsigned char reg, unsigned char bitNum, unsigned char data)
{
    unsigned char b;
    IICreadByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return IICwriteByte(dev, reg, b);
}

/**************************************************************************
函数名: IICreadBit
描述  : IIC读取一个位
输入  : dev - 目标设备I2C地址, reg - 寄存器地址, bitNum - 位位置, *data - 读取的位值
输出  : 1表示成功
说明  :
 - 该函数从特定设备的特定寄存器读取一个位的数据。
**************************************************************************/
unsigned char IICreadBit(unsigned char dev, unsigned char reg, unsigned char bitNum, unsigned char *data)
{
    unsigned char b;
    IICreadByte(dev, reg, &b);
    *data = b & (1 << bitNum);
    return 1;
}

/**************************************************************************
函数名: IICwriteBits
描述  : IIC写入多个位
输入  : dev - 目标设备I2C地址, reg - 寄存器地址, bitStart - 起始位位置, length - 位长度, data - 要写入的位值
输出  : 1表示成功
说明  :
 - 该函数将多个位的数据写入特定设备的特定寄存器。
**************************************************************************/
unsigned char IICwriteBits(unsigned char dev, unsigned char reg, unsigned char bitStart, unsigned char length, unsigned char data)
{
    unsigned char b, mask;
    IICreadByte(dev, reg, &b); // 读取寄存器当前值
    mask = ((1 << length) - 1) << (bitStart - length + 1); // 创建掩码，保留需要修改的位
    data <<= (bitStart - length + 1); // 将数据移到正确位置
    data &= mask; // 清零数据中不重要的位
    b &= ~(mask); // 清零寄存器中重要的位
    b |= data; // 将新的数据合并到寄存器中
    return IICwriteByte(dev, reg, b); // 写回寄存器
}

/**************************************************************************
函数名: IICreadBits
描述  : IIC读取多个位
输入  : dev - 目标设备I2C地址, reg - 寄存器地址, bitStart - 起始位位置, length - 位长度, *data - 读取的位值
输出  : 1表示成功
说明  :
 - 该函数从特定设备的特定寄存器读取多个位的数据。
**************************************************************************/
unsigned char IICreadBits(unsigned char dev, unsigned char reg, unsigned char bitStart, unsigned char length, unsigned char *data)
{
    unsigned char b, mask;
    if (IICreadByte(dev, reg, &b) != 0) {
        mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
        return 1;
    } else {
        return 0;
    }
}
