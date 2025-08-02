#ifndef __CONFIG_H__
#define __CONFIG_H__


#include "stdint.h"


#define  USE_DEBUG_MODULE    //


#ifdef USE_DEBUG_MODULE

#include "serial.h"
#include "fifo.h"
#include "console.h"

#endif



#define FLASH_BOOTLOADER_START_ADDR         0x08000000  //bootloader��ʼ��ַ
#define FLASH_BOOTLOADER_SIZE               0x0000c800  //bootloader��С 50k
#define FLASH_DATASAVE_START_ADDR           0x0800c800  //���ݴ洢����ʼ��ַ
#define FLASH_DATASAVE_SIZE                 0x00001800  //���ݴ洢����С 6k
#define FLASH_CACHE_START_ADDR              0x08027000 //�������򻺴�����ʼ��ַ
#define FLASH_CACHE_SIZE                    0x00019000  //�������򻺴�����С 100k
#define FLASH_APP_START_ADDR                (uint32_t)0x0800e000  //Ӧ�ó�����ʼ��ַ
#define FLASH_APP_SIZE                      0x00019000  //Ӧ�ó����С

typedef union
{
	int16_t data;
	uint8_t  buff[2];
}MoudBusType;

typedef union
{
	float     data;
	uint16_t  buff[2];
}FloatType;







#endif
