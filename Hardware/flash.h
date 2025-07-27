#ifndef __FLASH_H__
#define __FLASH_H__


#include "stm32f30x.h"

#include "app_info.h"

#define FLASH_PAGE_SIZE			1024*2

ErrorStatus flash_erase_app1_block(void);
ErrorStatus flash_erase_app2_block(void);
ErrorStatus flash_erase_iap_info_block(void);
void flash_write(uint32_t address,uint16_t *data,uint16_t len);
void flash_read(uint32_t address,uint8_t *data,uint16_t len);
void flash_erase_user_data(void);

//ErrorStatus flash_erase_ota_info_block(void);



#endif

