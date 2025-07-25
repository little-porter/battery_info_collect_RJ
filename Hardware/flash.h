#ifndef __FLASH_H__
#define __FLASH_H__


#include "stm32f30x.h"


#define FLASH_PAGE_SIZE		0x800


#define APP_INFO_SIZE	1024

#define APP_BLOCK1_ADDR	0x08005000
#define APP_BLOCK1_SIZE 0xA000
#define APP_BLOCK2_ADDR 0x0800F000
#define APP_BLOCK2_SIZE 0xA000
#define APP_OFFSET		0x400


#define FLASH_USER_DATA_ADDR	0x08019000

#define FLASH_OTA_DATA_ADDR		0x0801A800			//106k
#define FLASH_OTA_DATA_SIZE		0x800				//108k



ErrorStatus flash_erase_app_block(void);
ErrorStatus flash_erase_cache_block(void);
void flash_write(uint32_t address,uint16_t *data,uint16_t len);
void flash_read(uint32_t address,uint8_t *data,uint16_t len);
void flash_erase_user_data(void);
void flash_erase_ota_data(void);
//ErrorStatus flash_erase_ota_info_block(void);



#endif

