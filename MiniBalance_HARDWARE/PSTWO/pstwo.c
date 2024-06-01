#include "pstwo.h"

/*********************************************************
 * Defines and Global Variables
 *********************************************************/
#define DELAY_TIME  delay_us(5)  // 定义延时宏

u16 Handkey;  // 按键值读取,临时存储
u8 Comd[2] = {0x01, 0x42};  // 开始命令,请求数据
u8 Data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // 数据存储数组
u16 MASK[] = {
    PSB_SELECT,
    PSB_L3,
    PSB_R3,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
};  // 按键值与按键名

/*********************************************************
 * Function: PS2_Init
 * Description: Initialize the PS2 controller GPIO pins
 * Input: None
 * Output: None
 *********************************************************/
void PS2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 启用DI, DO, CS, CLK时钟
    RCC_APB2PeriphClockCmd(PS2_DI_GPIO_CLK | PS2_DO_GPIO_CLK | PS2_CS_GPIO_CLK | PS2_CLK_GPIO_CLK, ENABLE);

    // 配置DI引脚为上拉输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = PS2_DI_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(PS2_DI_GPIO_PORT, &GPIO_InitStructure);

    // 配置DO引脚为推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = PS2_DO_GPIO_PIN;
    GPIO_Init(PS2_DO_GPIO_PORT, &GPIO_InitStructure);

    // 配置CS引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = PS2_CS_GPIO_PIN;
    GPIO_Init(PS2_CS_GPIO_PORT, &GPIO_InitStructure);

    // 配置CLK引脚为推挽输出
    GPIO_InitStructure.GPIO_Pin = PS2_CLK_GPIO_PIN;
    GPIO_Init(PS2_CLK_GPIO_PORT, &GPIO_InitStructure);
}
/*********************************************************
 * Function: PS2_Cmd
 * Description: Send a command to the PS2 controller
 * Input: u8 CMD - The command to send
 * Output: None
 *********************************************************/
void PS2_Cmd(u8 CMD)
{
    volatile u16 ref = 0x01;
    Data[1] = 0;

    for(ref = 0x01; ref < 0x0100; ref <<= 1)
    {
        if(ref & CMD)
        {
            DO_H;  // 输出高电平
        }
        else
        {
            DO_L;  // 输出低电平
        }

        CLK_H;  // 时钟拉高
        DELAY_TIME;
        CLK_L;  // 时钟拉低
        DELAY_TIME;
        CLK_H;

        if(DI)
        {
            Data[1] |= ref;
        }
    }
    delay_us(16);
}

/*********************************************************
 * Function: PS2_RedLight
 * Description: Check if the PS2 controller is in red light mode
 * Input: None
 * Output: u8 - 0 if in red light mode, 1 otherwise
 *********************************************************/
u8 PS2_RedLight(void)
{
    CS_L;
    PS2_Cmd(Comd[0]);  // 发送开始命令
    PS2_Cmd(Comd[1]);  // 请求数据命令
    CS_H;

    return (Data[1] == 0x73) ? 0 : 1;  // 红灯模式返回0,其他模式返回1
}
/*********************************************************
 * Function: PS2_ReadData
 * Description: Read data from the PS2 controller
 * Input: None
 * Output: None
 *********************************************************/
void PS2_ReadData(void)
{
    CS_L;
    PS2_Cmd(Comd[0]);  // 发送开始命令
    PS2_Cmd(Comd[1]);  // 请求数据命令

    for(u8 byte = 2; byte < 9; byte++)  // 开始接收数据
    {
        volatile u16 ref = 0x01;
        for(ref = 0x01; ref < 0x100; ref <<= 1)
        {
            CLK_H;
            DELAY_TIME;
            CLK_L;
            DELAY_TIME;
            CLK_H;

            if(DI)
            {
                Data[byte] |= ref;
            }
        }
        delay_us(16);
    }
    CS_H;
}

/*********************************************************
 * Function: PS2_DataKey
 * Description: Process the data read from the PS2 controller, only handling button parts
 * Input: None
 * Output: u8 - The index of the pressed button, 0 if no button is pressed
 *********************************************************/
u8 PS2_DataKey(void)
{
    PS2_ClearData();
    PS2_ReadData();

    Handkey = (Data[4] << 8) | Data[3];  // 这是16个按键,按下为0,未按下为1

    for(u8 index = 0; index < 16; index++)
    {	    
        if((Handkey & (1 << (MASK[index] - 1))) == 0)
        {
            return index + 1;
        }
    }
    return 0;  // 没有任何按键按下
}

/*********************************************************
 * Function: PS2_AnologData
 * Description: Get the analog value of a specific button
 * Input: u8 button - The button index
 * Output: u8 - The analog value of the button
 *********************************************************/
u8 PS2_AnologData(u8 button)
{
    return Data[button];
}

/*********************************************************
 * Function: PS2_ClearData
 * Description: Clear the data buffer
 * Input: None
 * Output: None
 *********************************************************/
void PS2_ClearData(void)
{
    for(u8 i = 0; i < 9; i++)
    {
        Data[i] = 0x00;
    }
}
/*********************************************************
 * Function: PS2_Vibration
 * Description: Control the vibration of the PS2 controller
 * Input: u8 motor1 - Right small motor (0x00 off, others on)
 *        u8 motor2 - Left large motor (0x40~0xFF on, higher value means stronger vibration)
 * Output: None
 *********************************************************/
void PS2_Vibration(u8 motor1, u8 motor2)
{
    CS_L;
    delay_us(16);
    PS2_Cmd(0x01);  // 发送开始命令
    PS2_Cmd(0x42);  // 请求数据命令
    PS2_Cmd(0x00);
    PS2_Cmd(motor1);
    PS2_Cmd(motor2);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    CS_H;
    delay_us(16);
}

/*********************************************************
 * Function: PS2_ShortPoll
 * Description: Send a short poll command to the PS2 controller
 * Input: None
 * Output: None
 *********************************************************/
void PS2_ShortPoll(void)
{
    CS_L;
    delay_us(16);
    PS2_Cmd(0x01);  
    PS2_Cmd(0x42);  
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    CS_H;
    delay_us(16);	
}

/*********************************************************
 * Function: PS2_EnterConfing
 * Description: Enter configuration mode
 * Input: None
 * Output: None
 *********************************************************/
void PS2_EnterConfing(void)
{
    CS_L;
    delay_us(16);
    PS2_Cmd(0x01);  
    PS2_Cmd(0x43);  
    PS2_Cmd(0x00);
    PS2_Cmd(0x01);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    CS_H;
    delay_us(16);
}

/*********************************************************
 * Function: PS2_TurnOnAnalogMode
 * Description: Turn on analog mode
 * Input: None
 * Output: None
 *********************************************************/
void PS2_TurnOnAnalogMode(void)
{
    CS_L;
    PS2_Cmd(0x01);  
    PS2_Cmd(0x44);  
    PS2_Cmd(0x00);
    PS2_Cmd(0x01);  // analog=0x01; digital=0x00
    PS2_Cmd(0x03);  // 0x03 lock mode, 0xEE unlock mode
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    CS_H;
    delay_us(16);
}

/*********************************************************
 * Function: PS2_VibrationMode
 * Description: Enable vibration mode
 * Input: None
 * Output: None
 *********************************************************/
void PS2_VibrationMode(void)
{
    CS_L;
    delay_us(16);
    PS2_Cmd(0x01);  
    PS2_Cmd(0x4D);  
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x01);
    CS_H;
    delay_us(16);	
}

/*********************************************************
 * Function: PS2_ExitConfing
 * Description: Exit configuration mode
 * Input: None
 * Output: None
 *********************************************************/
void PS2_ExitConfing(void)
{
    CS_L;
    delay_us(16);
    PS2_Cmd(0x01);  
    PS2_Cmd(0x43);  
    PS2_Cmd(0x00);
    PS2_Cmd(0x00);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    PS2_Cmd(0x5A);
    CS_H;
    delay_us(16);
}

/*********************************************************
 * Function: PS2_SetInit
 * Description: Initialize the PS2 controller settings
 * Input: None
 * Output: None
 *********************************************************/
void PS2_SetInit(void)
{
    PS2_ShortPoll();
    PS2_ShortPoll();
    PS2_ShortPoll();
    PS2_EnterConfing();  // 进入配置模式
    PS2_TurnOnAnalogMode();  // 打开模拟模式
    // PS2_VibrationMode();  // 开启振动模式
    PS2_ExitConfing();  // 退出配置模式
}
/*********************************************************
 * Function: PS2_Read
 * Description: Read the control data from the PS2 controller
 * Input: None
 * Output: None
 *********************************************************/
void PS2_Read(void)
{
    static int Start;

    // 读取按键值
    PS2_KEY = PS2_DataKey(); 

    // 读取左侧摇杆X轴方向的模拟量
    PS2_LX = PS2_AnologData(PSS_LX); 

    // 读取左侧摇杆Y轴方向的模拟量
    PS2_LY = PS2_AnologData(PSS_LY);

    // 读取右侧摇杆X轴方向的模拟量
    PS2_RX = PS2_AnologData(PSS_RX);

    // 读取右侧摇杆Y轴方向的模拟量
    PS2_RY = PS2_AnologData(PSS_RY);  

    if(PS2_KEY == 4 && PS2_Mode == 1) 
    {
        // 当手柄上的Start键被按下
        Start = 1; 
    }

    // 当Start键被按下后，需要推下左侧前进杆，才能正式PS2控制小车
    if(Start && (PS2_LY < 118) && PS2_Mode == 1)
    {
        PS2_Flag = 1;
    }
}
