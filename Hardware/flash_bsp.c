#include "string.h"
#include "flash_bsp.h"


BOOTLOADER_ST bootloader_st;

void writeData_to_flash(uint32_t address,uint16_t *data,uint16_t len)
{
	FLASH_Unlock();
	/* write data_8 to the corresponding address */
	
	FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR | FLASH_FLAG_EOP);
    for (int i = 0; i < len; i++)
    {

        if (FLASH_COMPLETE == FLASH_ProgramHalfWord(address, data[i]))
        {
            address = address+2;
        }
        else
        {
            while (1);
        }
    }

	FLASH_Lock();
}

void readData_from_flash(uint32_t address,uint8_t *data,uint16_t len)
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
void userDataErase(void)
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
void otaDataErase(void)
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


void otaData_save(void)
{
	otaDataErase();
	writeData_to_flash(FLASH_OTA_DATA_ADDR,(uint16_t *)&bootloader_st,(sizeof(bootloader_st) + 1) / 2);
}

void sys_restart(void)
{
	__set_MSP(*(__IO uint32_t *)FLASH_BOOTLOADER_START_ADDR);
	SCB->VTOR = FLASH_BOOTLOADER_START_ADDR;
	SCB->AIRCR = (0x05FA << 16) | (1 << 2);
}

