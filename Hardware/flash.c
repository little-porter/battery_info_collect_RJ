#include "flash.h"




/**
* @brief  Ó¦ÓÃ³ÌÐò²Á³ý
* @param  None
* @retval 1succ  0fail
*/
ErrorStatus flash_erase_app_block(void)
{
	FLASH_Unlock();
	for(int i = 0; i< (APP_BLOCK1_SIZE / FLASH_PAGE_SIZE); i++)
	{
		if(FLASH_ErasePage(APP_BLOCK1_ADDR + (FLASH_PAGE_SIZE * i)) != FLASH_COMPLETE) 
		{
			FLASH_Lock();
			return ERROR;
		}
	}
	FLASH_Lock();
	return SUCCESS;
}

/**
* @brief  »º´æÇø²Á³ý
* @param  None
* @retval None
*/
ErrorStatus flash_erase_cache_block(void)
{
	FLASH_Unlock();
	for(int i = 0; i< (APP_BLOCK2_SIZE / FLASH_PAGE_SIZE); i++)
	{
		if(FLASH_ErasePage(APP_BLOCK2_ADDR + (FLASH_PAGE_SIZE * i)) != FLASH_COMPLETE) 
		{
			FLASH_Lock();
			return ERROR;
		}
	}
	FLASH_Lock();
	return SUCCESS;
}

ErrorStatus flash_erase_ota_info_block(void)
{
	FLASH_Unlock();
	for(int i = 0; i< (FLASH_OTA_DATA_SIZE / FLASH_PAGE_SIZE); i++)
	{
		if(FLASH_ErasePage(FLASH_OTA_DATA_ADDR + (FLASH_PAGE_SIZE * i)) != FLASH_COMPLETE) 
		{
			FLASH_Lock();
			return ERROR;
		}
	}
	FLASH_Lock();
	return SUCCESS;
}


void flash_write(uint32_t address,uint16_t *data,uint16_t len)
{
	FLASH_Unlock();
	/* write data_8 to the corresponding address */
	
	
	FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_EOP);
    for (int i = 0; i < len; i++)
    {
        if (FLASH_COMPLETE == FLASH_ProgramHalfWord(address, data[i])){
            address = address+2;
        }
        else
        {
            while (1);
        }
    }
	FLASH_Lock();
}
void flash_read(uint32_t address,uint8_t *data,uint16_t len)
{

	for(int i =0;i<len;i++)
	{
		data[i] = *(__IO uint8_t *)(address+i);
	}
	
}

/**
* @brief  ´æ´¢²Á³ý
* @param  erase_addr:²Á³ýµÄµØÖ·
* @retval None
*/
void flash_erase_user_data(void)
{
	FLASH_Unlock();
	
    if (FLASH_ErasePage(FLASH_USER_DATA_ADDR)!= FLASH_COMPLETE)
    {
     /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
      while (1)
      {
      }
    }
  
	FLASH_Lock();
}

/**
* @brief  ´æ´¢²Á³ý
* @param  erase_addr:²Á³ýµÄµØÖ·
* @retval None
*/
void flash_erase_ota_data(void)
{
	FLASH_Unlock();
	
    if (FLASH_ErasePage(FLASH_OTA_DATA_ADDR)!= FLASH_COMPLETE)
    {
     /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
      while (1)
      {
      }
    }
  
	FLASH_Lock();
}












