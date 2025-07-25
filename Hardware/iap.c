#include "iap.h"
#include <stdint.h>
#include <string.h>

#include "modbus.h"

#include "app_info.h"



typedef void (*pAppFunction)(void);

#define IAP_FIFO_SIZE		4096
typedef struct _iap_fifo
{
	uint8_t data[IAP_FIFO_SIZE];
	uint16_t pos;
	uint16_t tail;
}iap_fifo_t;


typedef enum _iap_process
{
	IAP_PROCESS_IDLE = 0,
	IAP_PROCESS_UPGRADE = 1,
	IAP_PROCESS_FINISH = 2,
	IAP_PROCESS_BIN_LOAD = 3,
	IAP_PROCESS_ERROR = 4,
}iap_process_t;

typedef enum _iap_flag
{
	IAP_FLAG_IDLE = 0,
	IAP_FLAG_START = 1,
	IAP_FLAG_FINISH = 2,
}iap_falg_t;

typedef enum _iap_upgrade_flag
{
	IAP_NO_UPGRADE = 0,
	IAP_UPGRADE = 1,
}iap_upgrade_flag_t;

typedef struct _app_info
{
	char version[100];
	char name[100];
	char reserve[APP_INFO_SIZE-100-100];
}app_info_t;

#define get_app_info()  	((app_info_t *)APP_BLOCK1_ADDR)
#define get_app_cache_info()  ((app_info_t *)APP_BLOCK2_ADDR)
#define app_addr 			((void *)APP_BLOCK1_ADDR)
#define cache_addr			((void *)APP_BLOCK2_ADDR)	



typedef enum _iap_run_app
{
	IAP_RUN_FACTORY = 0,
	IAP_RUN_APP1,
	IAP_RUN_APP2,
}iap_run_app_t;

typedef struct _iap_app_bin_info
{
	app_version_t *version;
	uint32_t bin_size;
	uint16_t bin_crc;
}iap_app_bin_info_t;


typedef struct _iap_info
{
	iap_run_app_t run_app;
	iap_app_bin_info_t factory_bin;
	iap_app_bin_info_t app1_bin;
	iap_app_bin_info_t app2_bin;
	
	iap_upgrade_flag_t upgrade_flag;
	iap_process_t process;
	iap_falg_t    iap_flag;
	uint32_t upgrade_size;
	uint16_t upgrade_crc;
	
	uint32_t bin_size;
	uint16_t bin_crc;
	uint16_t info_crc;
}iap_info_t;

iap_info_t iap_info;
iap_fifo_t iap_fifo;
uint16_t   iap_delay_time = 10000;
uint8_t    app_is_executable = 0;


/*计算CRC*/
uint16_t iap_calculate_crc(uint8_t *pData,uint16_t length)
{
    uint16_t crc = 0xFFFF; // 初始化CRC寄存器为0xFFFF
    for (uint16_t pos = 0; pos < length; pos++) {
        crc ^= (uint16_t)pData[pos]; // XOR当前字节的数据到CRC寄存器

        // 处理8个位
        for (int i = 8; i != 0; i--) {
            // 如果CRC寄存器的最低位是1，则右移一位，并与多项式相异或；否则直接右移一位
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001; // Modbus使用的多项式：X^16 + X^15 + X^2 + 1
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}




ErrorStatus iap_bin_load(void *des,void *src)
{
	ErrorStatus status = ERROR;
	uint8_t reply = 3;
	uint32_t app_area = (uint32_t)des;
	uint16_t *app_bin = (uint16_t *)src;
	
	while((reply--) && !status){
		status = flash_erase_app_block();
//		status = flash_erase_cache_block();
	} 
	for (int i = 0; i < iap_info.bin_size / 128; i++)
	{
		flash_write((uint32_t)app_area,(uint16_t *)((__IO uint16_t *)app_bin),64);

		app_area += 128;
		app_bin += 64;
	}
	if((app_area-(uint32_t)app_addr) < iap_info.bin_size){
		flash_write((uint32_t)app_area,(uint16_t *)((__IO uint16_t *)app_bin),(iap_info.bin_size - (app_area-(uint32_t)app_addr) + 1)/2);
	}
	
	iap_info.upgrade_flag = IAP_NO_UPGRADE;
	
	return status;
}

ErrorStatus iap_bin_crc(void)
{
	ErrorStatus status = SUCCESS;
	uint16_t crc = 0;
	uint8_t *data = (uint8_t *)get_app_cache_info();
	
	
//	iap_calculate_crc(data,iap_info.bin_size);
	crc = modbus_calculate_crc(data,iap_info.bin_size);
	
	if(crc == iap_info.bin_crc){
		status = SUCCESS;
	}else{
		status = ERROR;
	}
	
	return status;
}

void iap_crc_set(uint16_t crc)
{
	iap_info.bin_crc = crc;
}


void iap_info_save(void)
{
	flash_erase_ota_data();
	flash_write(FLASH_OTA_DATA_ADDR,(uint16_t *)&iap_info,sizeof(iap_info_t));
}


void iap_info_init(void)
{
	iap_info.upgrade_flag = IAP_NO_UPGRADE;
	iap_info.process = IAP_PROCESS_IDLE;
	iap_info.iap_flag = IAP_FLAG_IDLE;
	iap_info.bin_size = 0;
	iap_info.bin_crc = 0;
	uint16_t size = sizeof(iap_upgrade_flag_t) + sizeof(iap_process_t) + sizeof(iap_falg_t) + sizeof(uint32_t)  + sizeof(uint16_t);
	iap_info.info_crc = iap_calculate_crc((uint8_t *)&iap_info,size);
	iap_info_save();
}


void iap_read_info(void)
{
	flash_read(FLASH_OTA_DATA_ADDR,(uint8_t *)&iap_info,sizeof(iap_info_t));
	uint16_t size = sizeof(iap_upgrade_flag_t) + sizeof(iap_process_t) + sizeof(iap_falg_t) + sizeof(uint32_t)  + sizeof(uint16_t);
	uint16_t crc = iap_calculate_crc((uint8_t *)&iap_info,size);
	if(crc != iap_info.info_crc){							//ota区域数据校验不通过
		iap_info_init();									//初始化ota数据
	}
	
}

void iap_init(void)
{
	app_info_t *app_info = get_app_info();					//运行APP信息
	app_info_t *app_cache_info = get_app_cache_info();		//缓存APP信息
	app_version_t *app_version = (app_version_t *)app_info;
	app_version_t *app_cache_version = (app_version_t *)app_cache_info;
	
	iap_read_info();
	if(iap_info.upgrade_flag == IAP_UPGRADE){				//更新程序
		if(0 != strcmp(app_name,app_cache_info->name)){	/*判断软件名字*/
			iap_info.upgrade_flag = IAP_NO_UPGRADE;
		}else if(!iap_bin_crc()){							/*crc校验，判断文件完整性*/
			iap_info.upgrade_flag = IAP_NO_UPGRADE;
		}else if(app_version->aa < app_cache_version->aa){	/*版本比较，升级高版本*/
			iap_info.upgrade_flag = IAP_UPGRADE;
		}else if(app_version->bb < app_cache_version->bb){
			iap_info.upgrade_flag = IAP_UPGRADE;
		}else if(app_version->cc < app_cache_version->cc){
			iap_info.upgrade_flag = IAP_UPGRADE;
		}else{
			iap_info.upgrade_flag = IAP_NO_UPGRADE;
		}

		
		if(iap_info.upgrade_flag == IAP_UPGRADE){			/*信息校验通过，进行软件升级*/
			iap_bin_load(app_info,app_cache_info);			/*软件升级*/
		}else{;}
			
		iap_info_init();									/*升级完成，初始化ota信息*/	
		NVIC_SystemReset();									/*软件复位*/
			
	}else{													//校验APP信息
		if(0 == strcmp(app_name,app_info->name)){			/*判断软件信息，成功跳转运行*/
			app_is_executable = 1;                  		//设置APP可执行
		}else{
			app_is_executable = 0;
		}
	}
}



void iap_jump_app(void)
{
//	uint32_t jumpAdderss = *(__IO uint32_t *)(APP_BLOCK1_ADDR + APP_OFFSET);
	uint32_t jumpAdderss = *(__IO uint32_t *)(APP_BLOCK1_ADDR+APP_OFFSET+4);
	pAppFunction app_main;
	
	// 关闭初始化的外设
	/* NVIC */
	__disable_irq();
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

	__set_MSP(*(__IO uint32_t *)(APP_BLOCK1_ADDR+APP_OFFSET));
	SCB->VTOR = APP_BLOCK1_ADDR+APP_OFFSET;

	jumpAdderss = *(__IO uint32_t *)(APP_BLOCK1_ADDR + APP_OFFSET + 4);
	app_main = (pAppFunction)jumpAdderss;
	__enable_irq();
	app_main();	
}

void iap_flag_idle(void)
{
	iap_info.iap_flag = IAP_FLAG_IDLE;
}


void iap_flag_start(void)
{
	iap_info.iap_flag = IAP_FLAG_START;
}

void iap_flag_finish(void)
{
	iap_info.iap_flag = IAP_FLAG_FINISH;
}


uint16_t iap_read_data(uint8_t *pdata,uint16_t num,uint16_t timeout)
{
	uint16_t data_num;
	if(iap_fifo.pos < iap_fifo.tail){
		data_num = iap_fifo.tail - iap_fifo.pos;
	}else if(iap_fifo.pos > iap_fifo.tail){
		data_num = IAP_FIFO_SIZE - iap_fifo.pos + iap_fifo.tail;
	}else{
		data_num = 0;
	}
	
	if(data_num > num){
		data_num = num;
	}else{
		data_num = data_num;
	}
	
	if((iap_fifo.pos+data_num) < IAP_FIFO_SIZE){
		memcpy(pdata,&iap_fifo.data[iap_fifo.pos],data_num);
	}else{
		memcpy(pdata,&iap_fifo.data[iap_fifo.pos],IAP_FIFO_SIZE-iap_fifo.pos);
		memcpy(pdata+IAP_FIFO_SIZE-iap_fifo.pos,&iap_fifo.data[0],data_num-(IAP_FIFO_SIZE-iap_fifo.pos));
	}
	
	iap_fifo.pos = iap_fifo.pos + data_num;
	iap_fifo.pos %= IAP_FIFO_SIZE;
	
	return data_num;		//返回读取数量
}

void iap_write_data(uint8_t *pdata,uint16_t num,uint16_t timeout)
{
	if((iap_fifo.tail+num) < IAP_FIFO_SIZE){
		memcpy(&iap_fifo.data[iap_fifo.tail],pdata,num);
	}else{
		memcpy(&iap_fifo.data[iap_fifo.tail],pdata,IAP_FIFO_SIZE-iap_fifo.tail);
		memcpy(&iap_fifo.data[0],pdata+IAP_FIFO_SIZE-iap_fifo.tail,num - (IAP_FIFO_SIZE-iap_fifo.tail));
	}
	
	iap_fifo.tail = iap_fifo.tail + num;
	iap_fifo.tail %= IAP_FIFO_SIZE;
}

ErrorStatus iap_idle_process(void)
{
	ErrorStatus status = SUCCESS;
	if(iap_info.iap_flag == IAP_FLAG_START){
		status = SUCCESS;
	}else{
		status = ERROR;
	}
	return status;
}



ErrorStatus iap_upgrade_process(void)
{
	ErrorStatus status = ERROR;
	uint8_t read_data[1024] = {0};
	uint16_t read_bytes = 0;
	
	if(iap_fifo.pos == iap_fifo.tail){
		if(iap_info.iap_flag == IAP_FLAG_FINISH){
			status = SUCCESS;
		}else{
			status = ERROR;
		}
	}
	
	read_bytes = iap_read_data(read_data,1024,100);
	if(read_bytes == 0)  return status;
	if((iap_info.bin_size+read_bytes) < APP_BLOCK2_SIZE){
		flash_write(APP_BLOCK2_ADDR+iap_info.bin_size,(uint16_t *)read_data,read_bytes/2);
		iap_info.bin_size = iap_info.bin_size + read_bytes;
	}else{
		iap_info.process = IAP_PROCESS_ERROR;
	}
	
	
	
	return status;
}

volatile uint16_t frame_num[50];
volatile uint16_t index1 = 0;

void iap_task_callback(void *param)
{
	ErrorStatus	status = SUCCESS;
	
	switch(iap_info.process)
	{
		case IAP_PROCESS_IDLE:
			status = iap_idle_process();
			if(status){
				iap_info.process = IAP_PROCESS_UPGRADE;
				flash_erase_cache_block();
			}else{;}
			break;
		case IAP_PROCESS_UPGRADE:
			status = iap_upgrade_process();
			if(status){
				iap_info.process = IAP_PROCESS_FINISH;
			}else{;}
			break;
		case IAP_PROCESS_FINISH:
			status = iap_bin_crc();
			if(!status){
				iap_info.process = IAP_PROCESS_ERROR;
			}else{
				iap_info.process = IAP_PROCESS_BIN_LOAD;
			}
			break;
		case IAP_PROCESS_BIN_LOAD:
			status = iap_bin_load(app_addr,cache_addr);
			if(status){
				iap_flag_idle();
				iap_info.process = IAP_PROCESS_IDLE;
				iap_info_save();
				NVIC_SystemReset();								/*软件复位*/
			}else{;}
			break;
		case IAP_PROCESS_ERROR:
			iap_info.upgrade_flag = IAP_NO_UPGRADE;
			iap_info.bin_size = 0;
			iap_info.bin_crc = 0;
			status = flash_erase_cache_block();
			if(status){
				iap_flag_idle();
				iap_info.process = IAP_PROCESS_IDLE;
			}else{;}
			break;
		default:
			iap_info.upgrade_flag = IAP_NO_UPGRADE;
			iap_info.bin_size = 0;
			iap_info.bin_crc = 0;
			iap_flag_idle();
			iap_info.process = IAP_PROCESS_IDLE;
			break;	
	}
}

void iap_msg_deal_handler(uint8_t *data,uint16_t length)
{
	if(data[2] != 0x02)  return;
	uint16_t data_num = data[6]<<8 | data[7];
	uint16_t crc = 0;
	frame_num[index1] =  data[4]<<8 | data[5];
	index1++;
	if(data[3] == 0x00){
		iap_flag_start();
		iap_info.upgrade_size = (data[8]<<24) | (data[9]<<16) | (data[10]<<8) |(data[11]<<0) ;
		return;
	}else if(data[3] == 0x11){
		crc = data[8] | data[9]<<8;
		iap_flag_finish();
		iap_crc_set(crc);
		iap_info.upgrade_crc = (data[8]<<8) | data[9];
		return;
	}else if(data[3] == 0x10){
	}else{
		return;
	}
	
	
		
	iap_write_data(&data[8],data_num,500);		//将文件发送到iap
	
}



void iap_task(void)
{
	if(app_is_executable){
		if(iap_delay_time == 0){
			iap_jump_app();
		}else{
			iap_task_callback(NULL);
		}
	}else{
		iap_task_callback(NULL);
	}
//	iap_task_callback(NULL);
//	static uint32_t start_time = 0,time = 5;
//	static uint8_t  start_flag = 0;
//	
//	if(start_flag == 0){
//		start_time = system_tick_get();
//		start_flag = 1;
//	}
//	
//	if(system_timer_triggered(start_time,time)){
//		start_flag = 0;
//		sysTask_publish(iap_task_callback,NULL);
//	}
}



