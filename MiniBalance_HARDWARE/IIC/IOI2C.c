#include "ioi2c.h"
#include "sys.h"
#include "delay.h"

/**************************************************************************
Function: IIC pin initialization
Input   : none
Output  : none
Description:
 - This function initializes the GPIO pins for I2C communication on GPIOB.
**************************************************************************/
void IIC_Init(void)
{			
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // Enable clock for GPIOB
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;	// Pins for I2C
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // Output push-pull mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					      // Initialize GPIOB for I2C 
}

/**************************************************************************
Function: Simulate IIC start signal
Input   : none
Output  : 1 if successful, 0 if failed
Description:
 - This function simulates the I2C start signal.
**************************************************************************/
int IIC_Start(void)
{
    SDA_OUT();     // Set SDA as output
    IIC_SDA = 1;
    if (!READ_SDA) return 0;	
    IIC_SCL = 1;
    delay_us(1);
    IIC_SDA = 0; // START: when CLK is high, DATA changes from high to low 
    if (READ_SDA) return 0;
    delay_us(1);
    IIC_SCL = 0; // Clock down to prepare for I2C communication
    return 1;
}

/**************************************************************************
Function: Analog IIC end signal
Input   : none
Output  : none
Description:
 - This function simulates the I2C stop signal.
**************************************************************************/
void IIC_Stop(void)
{
    SDA_OUT(); // Set SDA as output
    IIC_SCL = 0;
    IIC_SDA = 0; // STOP: when CLK is high, DATA changes from low to high
    delay_us(1);
    IIC_SCL = 1; 
    IIC_SDA = 1; // Release I2C stop signal
    delay_us(1);							   	
}

/**************************************************************************
Function: IIC wait for the response signal
Input   : none
Output  : 0 if no response received, 1 if response received
Description:
 - This function waits for the I2C response signal.
**************************************************************************/
int IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    SDA_IN();      // Set SDA as input  
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
    IIC_SCL = 0; // Clock down to prepare for communication	   
    return 1;  
}

/**************************************************************************
Function: IIC response
Input   : none
Output  : none
Description:
 - This function sends an acknowledgment signal during I2C communication.
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
Function: IIC don't reply
Input   : none
Output  : none
Description:
 - This function sends a non-acknowledgment signal during I2C communication.
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
Function: IIC sends a byte
Input   : txd - Byte data to be sent
Output  : none
Description:
 - This function sends a byte of data during I2C communication.
**************************************************************************/
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    SDA_OUT(); 	    
    IIC_SCL = 0; // Start clock signal
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
Function: IIC write data to register
Input   : addr - Device address, reg - Register address, len - Number of bytes, data - Data to be written
Output  : 0 if write successful, 1 if failed
Description:
 - This function writes data to a register of a device using I2C communication.
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
Function: IIC read register data
Input   : addr - Device address, reg - Register address, len - Number of bytes, *buf - Data read out
Output  : 0 if read successful, 1 if failed
Description:
 - This function reads data from a register of a device using I2C communication.
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
Function: IIC reads a byte
Input   : ack (Send response signal or not: 1 - Send, 0 - Do not send)
Output  : receive (Data read)
Description:
 - This function reads a byte of data during I2C communication.
**************************************************************************/
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN(); // Set SDA as input
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
        IIC_Ack(); // Send ACK
    else
        IIC_NAck(); // Send NACK
    return receive;
}


/**************************************************************************
Function: IIC ReadOneByte
Input   : I2C_Addr - Device I2C address, addr - Register address
Output  : res - Data read
Description:
 - This function reads a byte of data from a specific register of a specific device.
**************************************************************************/
unsigned char I2C_ReadOneByte(unsigned char I2C_Addr, unsigned char addr)
{
    unsigned char res = 0;
    
    IIC_Start();    
    IIC_Send_Byte(I2C_Addr);	   // Send device address
    res++;
    IIC_Wait_Ack();
    IIC_Send_Byte(addr); res++;  // Send register address
    IIC_Wait_Ack();	  
    IIC_Start();
    IIC_Send_Byte(I2C_Addr + 1); res++;          // Change to read mode			   
    IIC_Wait_Ack();
    res = IIC_Read_Byte(0);	   
    IIC_Stop(); // Stop and release the bus

    return res;
}

/**************************************************************************
Function: IICreadBytes
Input   : dev - Target device I2C address, reg - Register address, length - Number of bytes, *data - Data read out
Output  : count - Number of bytes read out - 1
Description:
 - This function reads multiple bytes of data from a specific register of a specific device.
**************************************************************************/
u8 IICreadBytes(u8 dev, u8 reg, u8 length, u8 *data){
    u8 count = 0;
    
    IIC_Start();
    IIC_Send_Byte(dev);	   // Send device address
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);   // Send register address
    IIC_Wait_Ack();	  
    IIC_Start();
    IIC_Send_Byte(dev + 1);  // Change to read mode	
    IIC_Wait_Ack();
    
    for(count = 0; count < length; count++){
        if(count != length - 1)   
            data[count] = IIC_Read_Byte(1);  // Read with ACK
        else                  
            data[count] = IIC_Read_Byte(0);  // Read last byte with NACK
    }
    IIC_Stop(); // Stop and release the bus
    return count;
}

/**************************************************************************
Function: IICwriteBytes
Input   : dev - Target device I2C address, reg - Register address, length - Number of bytes, *data - Data to be written
Output  : 1 if successful, 0 if failed
Description:
 - This function writes multiple bytes of data to a specific register of a specific device.
**************************************************************************/
u8 IICwriteBytes(u8 dev, u8 reg, u8 length, u8* data){
    
    u8 count = 0;
    IIC_Start();
    IIC_Send_Byte(dev);	   // Send device address
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);   // Send register address
    IIC_Wait_Ack();	  
    for(count = 0; count < length; count++){
        IIC_Send_Byte(data[count]); 
        IIC_Wait_Ack(); 
     }
    IIC_Stop(); // Stop and release the bus

    return 1; //status == 0;
}

/**************************************************************************
Function: IICreadByte
Input   : dev - Target device I2C address, reg - Register address, *data - Data read out
Output  : 1 if successful, 0 if failed
Description:
 - This function reads a byte of data from a specific register of a specific device.
**************************************************************************/
u8 IICreadByte(u8 dev, u8 reg, u8 *data){
    *data = I2C_ReadOneByte(dev, reg);
    return 1;
}

/**************************************************************************
Function: IICwriteByte
Input   : dev - Target device I2C address, reg - Register address, data - Data to be written
Output  : 1
Description:
 - This function writes a byte of data to a specific register of a specific device.
**************************************************************************/
unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data){
    return IICwriteBytes(dev, reg, 1, &data);
}

/**************************************************************************
Function: IICwriteBits
Input   : dev - Target device I2C address, reg - Register address, bitStart - Start bit of target byte, length - Number of bits to change, data - Value of the bits to be changed
Output  : 1 if success, 0 if fail
Description:
 - This function reads, modifies, and writes multiple bits in a byte of a specific register of a specific device.
**************************************************************************/
u8 IICwriteBits(u8 dev, u8 reg, u8 bitStart, u8 length, u8 data)
{
    u8 b;
    if (IICreadByte(dev, reg, &b) != 0) {
        u8 mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // Shift data into correct position
        data &= mask; // Zero all non-important bits in data
        b &= ~(mask); // Zero all important bits in existing byte
        b |= data; // Combine data with existing byte
        
        // Write the modified byte back to the register
        if (IICwriteByte(dev, reg, b) != 0) {
            return 1; // Success
        }
    }
    return 0; // Fail
}

/**************************************************************************
Function: IICwriteBit
Input   : dev - Target device I2C address, reg - Register address, bitNum - Bit number to change, value - Value to write to the bit
Output  : 1 if success, 0 if fail
Description:
 - This function reads, modifies, and writes a single bit in a byte of a specific register of a specific device.
**************************************************************************/
u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 value)
{
    u8 b;
    if (IICreadByte(dev, reg, &b) != 0) {
        u8 mask = 1 << bitNum; // Create a mask for the bit to change
        if (value != 0) {
            b |= mask; // Set the bit
        } else {
            b &= ~mask; // Clear the bit
        }
        
        // Write the modified byte back to the register
        if (IICwriteByte(dev, reg, b) != 0) {
            return 1; // Success
        }
    }
    return 0; // Fail
}
