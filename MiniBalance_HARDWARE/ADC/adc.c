
#include "adc.h"  // 包含ADC头文件

#define TSL_SI    PBout(1)   //SI引脚控制
#define TSL_CLK   PAout(1)   //CLK引脚控制

/**
  * @brief 实现微秒级延时函数
  * @param 无
  * @retval 无
  */
void Dly_us(void)
{
    int ii;    
    for(ii = 0; ii < 10; ii++);  // 微秒级延时
}

/**
  * @brief CCD数据读取函数，控制SI和CLK引脚进行数据读取
  * @param 无
  * @retval 无
  */
void RD_TSL(void) 
{
    u8 i = 0, tslp = 0;
    
    // 控制SI和CLK引脚进行数据读取
    TSL_CLK = 0;
    TSL_SI = 0; 
    Dly_us();
      
    TSL_SI = 1; 
    Dly_us();
	
    TSL_CLK = 0;
    Dly_us();
      
    TSL_CLK = 1;
    Dly_us();
	
    TSL_SI = 0;
    Dly_us(); 
    
    // 读取CCD传感器数据并存储到ADV数组中
    for(i = 0; i < 128; i++)
    { 
        TSL_CLK = 0; 
        Dly_us();  // 微秒级延时
        ADV[tslp] = (Get_Adc(2)) >> 4;
        tslp++;
        TSL_CLK = 1;
        Dly_us();
    }  
}

/**
  * @brief ADC初始化函数，配置ADC模块参数
  * @param 无
  * @retval 无
  */
void Adc_Init(void)
{    
    ADC_InitTypeDef ADC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能ADC1的时钟和GPIOB的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);	  
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   // 设置ADC时钟分频为6

    // 配置GPIOB的引脚为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	// 模拟输入模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);	

    ADC_DeInit(ADC1);  // 重置ADC1，将外设 ADC1 的寄存器重设为缺省值

    // ADC初始化设置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  // ADC工作模式独立
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;       // 关闭扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // 关闭连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 禁用外部触发转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;  // 转换的ADC通道数为1
    ADC_Init(ADC1, &ADC_InitStructure);  // 根据ADC初始化结构体参数进行ADC1初始化

    ADC_Cmd(ADC1, ENABLE);  // 启动ADC1
    ADC_ResetCalibration(ADC1);  // 重置校准
    while(ADC_GetResetCalibrationStatus(ADC1));  // 等待重置校准完成
    ADC_StartCalibration(ADC1);  // 开始校准
    while(ADC_GetCalibrationStatus(ADC1));  // 等待校准完成
}

/**
  * @brief 获取指定通道的ADC数据
  * @param ch ADC通道号
  * @retval u16类型的ADC转换值
  */
u16 Get_Adc(u8 ch)   
{
    // 配置ADC通道和采样时间后启动转换并等待转换完成
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));  // 等待转换完成

    return ADC_GetConversionValue(ADC1);  // 返回ADC转换值
}

/**
  * @brief 获取电池电压值
  * @param 无
  * @retval int类型的电池电压值
  */
int Get_battery_volt(void)   
{  
    int Volt;  // 电池电压值
    Volt = Get_Adc(Battery_Ch) * 3.3 * 11 * 100 / 4096;  // 获取ADC转换值并计算电压值
    return Volt;  // 返回电池电压值
}

/**
  * @brief CCD模块初始化函数
  * @param 无
  * @retval 无
  */
void ccd_Init(void)
{    
    ADC_InitTypeDef ADC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOA、GPIOB、GPIOC和AFIO的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    // 使能ADC1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   // 设置ADC时钟分频为6

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
                      
    // 配置SI引脚为输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	

    // 配置CLK引脚为输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	

    // 配置PB0为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	

    // 其他CCD模块初始化设置，如ADC初始化等
    // ...
}

/**
  * @brief 电子元件初始化函数
  * @param 无
  * @retval 无
  */
void ele_Init(void)
{    
    ADC_InitTypeDef ADC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOA、GPIOB的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    // 使能ADC1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   // 设置ADC时钟分频为6
                      
    // 配置相关引脚为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	

    // 其他电子元件初始化设置，如ADC初始化等
    // ...
}
