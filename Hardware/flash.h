#ifndef __FLASH_H__
#define __FLASH_H__

#include "system.h"


#define GAS_PARAM_SAVE_ADDR		0x0801F800



void flash_pages_erase(uint32_t addr,uint16_t page_num);
uint8_t flash_write_data(uint32_t addr,uint8_t *data,uint16_t len);
void flash_read_data(uint32_t addr,uint8_t *data,uint16_t len);

#endif

