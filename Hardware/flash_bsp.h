#ifndef __FLASH_BSP_H
#define __FLASH_BSP_H

#include "gxht30.h"
#include "main.h"
#include "adc.h"





#define FLASH_OTA_DATA_ADDR                 0x0800c800    //ota数据存储地址   //2k
#define FLASH_USER_DATA_ADDR                0x0800d000    //用户数据存储地址



#pragma pack(1)


typedef struct
{
    uint8_t      ota_startFlag;       //开始标志
	uint8_t      ota_writeFlag;       //写标志
	uint8_t      ota_completFlag;       //完成标志
	uint8_t      ota_errtFlag;       //故障标志
    uint16_t     contentSizePerFrame; // 每帧的文件长度
    uint32_t     fileReceiveSize;     // 已接收文件大小
	
	uint8_t      order[43];

} BOOTLOADER_ST;

#pragma pack()
extern BOOTLOADER_ST bootloader_st;


void readData_from_flash(uint32_t address,uint8_t *data,uint16_t len);
void writeData_to_flash(uint32_t address,uint16_t *data,uint16_t len);
void userDataErase(void);
void otaDataErase(void);
void otaData_save(void);
void sys_restart(void);


#endif
