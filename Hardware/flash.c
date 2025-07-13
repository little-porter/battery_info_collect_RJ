#include "flash.h"


uint32_t flash_get_page_addr(uint16_t page)
{
	uint32_t addr = 0;
	if(page >= 128){
		addr = 0;
	}else{
		addr = 0x08000000 + 0x800*page;
	}
	
	return addr;
}


void flash_page_erase(uint16_t page)
{
	FLASH_Unlock();
	uint32_t addr = 0;
	addr = flash_get_page_addr(page);
	if(addr == 0) return;
	
    if (FLASH_ErasePage(addr)!= FLASH_COMPLETE)
    {
     /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
      while (1)
      {
      }
    }
  
	FLASH_Lock();	
}

void flash_pages_erase(uint32_t addr,uint16_t page_num)
{
	uint16_t num = 0;
	num = (addr-0x08000000)/2048 + page_num;
	if(num > 128) return;
	FLASH_Unlock();
	for(int i=0; i<page_num; i++){
		if (FLASH_ErasePage(addr)!= FLASH_COMPLETE){
		 /* Error occurred while sector erase. 
			 User can add here some code to deal with this error  */
		  while (1){;};
		}else{
			addr = addr + 0x800;
		}
	}
	FLASH_Lock();	
}



uint8_t flash_write_data(uint32_t addr,uint8_t *data,uint16_t len)
{
	uint16_t *ptr = (uint16_t *)data;
	uint16_t write_len = (len+1)/2;
	FLASH_Unlock();
	for(int i=0; i<write_len; i++){
		if (FLASH_COMPLETE != FLASH_ProgramHalfWord(addr, ptr[i])){
			return 0;
		}else{
			addr+=2;
		}
	}
	FLASH_Lock();
	return 1;
}


void flash_read_data(uint32_t addr,uint8_t *data,uint16_t len)
{
	FLASH_Unlock();
	for(int i=0; i<len; i++)
	{
		data[i] = *(__IO uint8_t *)(addr+i);
	}
	FLASH_Lock();
}





