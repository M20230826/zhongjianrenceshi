#include "stmflash.h"
#include "delay.h"

#define FLASH_SAVE_ADDR  0X0800E000 // 设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

// 解锁STM32的FLASH
void STMFLASH_Unlock(void) {
    FLASH->KEYR = FLASH_KEY1; // 写入解锁序列
    FLASH->KEYR = FLASH_KEY2;
}

// FLASH上锁
void STMFLASH_Lock(void) {
    FLASH->CR |= 1 << 7; // 上锁
}

// 获取FLASH状态
u8 STMFLASH_GetStatus(void) {  
    u32 res = FLASH->SR; 
    if (res & (1 << 0)) return 1; // 忙
    else if (res & (1 << 2)) return 2; // 编程错误
    else if (res & (1 << 4)) return 3; // 写保护错误
    return 0; // 操作完成
}

// 等待操作完成
// time: 要延时的长短
// 返回值: 状态
u8 STMFLASH_WaitDone(u16 time) {
    u8 res;
    do {
        res = STMFLASH_GetStatus();
        if (res != 1) break; // 非忙，无需等待，直接退出
        delay_us(1);
        time--;
    } while (time);
    if (time == 0) res = 0xff; // TIMEOUT
    return res;
}

// 擦除页
// paddr: 页地址
// 返回值: 执行情况
u8 STMFLASH_ErasePage(u32 paddr) {
    u8 res = 0;
    res = STMFLASH_WaitDone(0X5FFF); // 等待上次操作结束,>20ms    
    if (res == 0) { 
        FLASH->CR |= 1 << 1; // 页擦除
        FLASH->AR = paddr; // 设置页地址 
        FLASH->CR |= 1 << 6; // 开始擦除      
        res = STMFLASH_WaitDone(0X5FFF); // 等待操作结束,>20ms  
        if (res != 1) { // 非忙
            FLASH->CR &= ~(1 << 1); // 清除页擦除标志
        }
    }
    return res;
}

// 在FLASH指定地址写入半字
// faddr: 指定地址(此地址必须为2的倍数!!)
// dat: 要写入的数据
// 返回值: 写入的情况
u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat) {
    u8 res;        
    res = STMFLASH_WaitDone(0XFF);   
    if (res == 0) { // OK
        FLASH->CR |= 1 << 0; // 编程使能
        *(vu16*)faddr = dat; // 写入数据
        res = STMFLASH_WaitDone(0XFF); // 等待操作完成
        if (res != 1) { // 操作成功
            FLASH->CR &= ~(1 << 0); // 清除PG位
        }
    } 
    return res;
} 

// 读取指定地址的半字(16位数据) 
// faddr: 读取地址 
// 返回值: 对应数据
u16 STMFLASH_ReadHalfWord(u32 faddr) {
    return *(vu16*)faddr; 
}

#if STM32_FLASH_WREN // 如果启用了写   
// 不检查的写入
// WriteAddr: 起始地址
// pBuffer: 数据指针
// NumToWrite: 半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite) {            
    for (u16 i = 0; i < NumToWrite; i++) {
        STMFLASH_WriteHalfWord(WriteAddr, pBuffer[i]);
        WriteAddr += 2; // 地址增加2
    }  
} 

#if STM32_FLASH_SIZE < 256
#define STM_SECTOR_SIZE 1024 // 字节
#else 
#define STM_SECTOR_SIZE 2048
#endif         
u16 STMFLASH_BUF[STM_SECTOR_SIZE / 2]; // 最多是2K字节

// 从指定地址开始写入指定长度的数据
// WriteAddr: 起始地址(此地址必须为2的倍数!!)
// pBuffer: 数据指针
// NumToWrite: 半字(16位)数(就是要写入的16位数据的个数)
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite) {
    u32 secpos;    // 扇区地址
    u16 secoff;    // 扇区内偏移地址(16位字计算)
    u16 secremain; // 扇区内剩余地址(16位字计算)     
    u32 offaddr;   // 去掉0X08000000后的地址
    u16 i; // 定义变量 i
    if (WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE))) return; // 非法地址
    STMFLASH_Unlock(); // 解锁
    offaddr = WriteAddr - STM32_FLASH_BASE; // 实际偏移地址
    secpos = offaddr / STM_SECTOR_SIZE; // 扇区地址  0~127 for STM32F103RBT6
    secoff = (offaddr % STM_SECTOR_SIZE) / 2; // 在扇区内的偏移(2个字节为基本单位)
    secremain = STM_SECTOR_SIZE / 2 - secoff; // 扇区剩余空间大小   
    if (NumToWrite <= secremain) secremain = NumToWrite; // 不大于该扇区范围
    while (1) {    
        STMFLASH_Read(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2); // 读出整个扇区的内容
        for (i = 0; i < secremain; i++) { // 校验数据
            if (STMFLASH_BUF[secoff + i] != 0XFFFF) break; // 需要擦除      
        }
        if (i < secremain) { // 需要擦除
            STMFLASH_ErasePage(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE); // 擦除这个扇区
            for (i = 0; i < secremain; i++) { // 复制
                STMFLASH_BUF[i + secoff] = pBuffer[i];      
            }
            STMFLASH_Write_NoCheck(secpos * STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2); // 写入整个扇区  
        } else {
            STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain); // 写已经擦除过的，直接写入扇区剩余区间。                  
        }
        if (NumToWrite == secremain) break; // 写入结束
        else { // 写入未结束
            secpos++; // 扇区地址加1
            secoff = 0; // 偏移位置为0     
            pBuffer += secremain;
            NumToWrite -= secremain; // 减去已经写入的数据
            secremain = STM_SECTOR_SIZE / 2; // 重置扇区剩余空间大小
        }
    }
    STMFLASH_Lock(); // 上锁
} 

#endif

// 从指定地址开始读出指定长度的数据
// ReadAddr: 起始地址
// pBuffer: 数据指针
// NumToRead: 半字(16位)数
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead) {
    for (u16 i = 0; i < NumToRead; i++) {
        pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);
        ReadAddr += 2; // 地址增加2
    }
}

// 测试写入函数
void Test_Write(u32 WriteAddr, u16 WriteData) {
    STMFLASH_Write(WriteAddr, &WriteData, 1);
}

// 自定义的Flash读写函数
void Flash_Read(void) {
    // 实现你的Flash读取逻辑
}

void Flash_Write(void) {
    // 实现你的Flash写入逻辑
}