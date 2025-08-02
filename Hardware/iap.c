#include "iap.h"
#include <stdint.h>
#include <string.h>



#include "app_info.h"

#define FACTORY_APP		0
#define IAP_APP			1
#define APP_TYPE		FACTORY_APP
#define APP_MAX_SIZE	0xA000				//40kb
#define REG_IAP_START_FLAG 0x0003
#define REG_IAP_VERSION_AA 0x3003
#define REG_IAP_VERSION_BB 0x3004
#define REG_IAP_VERSION_CC 0x3005
#define IAP_OUT_TIME_MS	   10000

#define IAP_INFO_CRC		0xAA55


typedef void (*pAppFunction)(void);

#define IAP_FIFO_SIZE		4096
typedef struct _iap_fifo
{
	uint8_t data[IAP_FIFO_SIZE];
	uint16_t pos;
	uint16_t tail;
}iap_fifo_t;

iap_fifo_t iap_fifo;


typedef enum _iap_process
{
	IAP_PROCESS_IDLE = 0,
	IAP_PROCESS_HEAD_CHECK,
	IAP_PROCESS_UPGRADE,
	IAP_PROCESS_FINISH,
	IAP_PROCESS_ERROR,
}iap_process_t;

typedef enum _iap_flag
{
	IAP_FLAG_IDLE = 0,
	IAP_FLAG_START = 1,
	IAP_FLAG_FINISH = 2,
}iap_falg_t;

typedef enum _iap_status{ 
	IAP_STATUS_UPGRADING = 0,
	IAP_STATUS_FINISH,
	IAP_STATUS_BIN_OVER,
	IAP_STATUS_BIN_ERROR,
	IAP_STATUS_VERSION_ERROR,
	IAP_STATUS_CRC_ERROR,
	IAP_STATUS_TIMEOUT,
}iap_status_t;


typedef enum _iap_upgrade_falg
{
	IAP_UPGRADE_FLAG_NO = 0,
	IAP_UPGRADE_FLAG_YES,
}iap_upgrade_flag_t;


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
	iap_upgrade_flag_t  upgrade_flag;
	
	uint32_t run_app_addr;
	uint32_t cache_addr;
	
	iap_process_t process;
	uint32_t upgrade_size;
	uint16_t upgrade_crc;
	iap_falg_t    iap_flag;
	uint16_t  iap_status;
	
	uint16_t erase_page;
	uint32_t bin_size;
	uint16_t bin_crc;
	uint16_t info_crc;
}iap_info_t;

iap_info_t iap_info;

#define  IAP_TIMEOUT		10
uint16_t   iap_delay_time = IAP_TIMEOUT;					//IAP程序更新过程超时时间
uint16_t   iap_jump_time  = IAP_TIMEOUT;					//IAP跳转时间
uint8_t    app_is_executable = 0;

void iap_info_init(void)
{
	iap_info.upgrade_flag = IAP_UPGRADE_FLAG_NO;
	iap_info.run_app_addr = FACTORY_APP_START_ADDR;
	iap_info.cache_addr = CACHE_START_ADDR;
	iap_info.process = IAP_PROCESS_IDLE;
	iap_info.info_crc = IAP_INFO_CRC;
}

void iap_info_save(void)
{
	flash_erase_iap_info_block();
	flash_write(IAP_INFO_START_ADDR,(uint16_t *)&iap_info,sizeof(iap_info_t));
}

void iap_info_read(void)
{
	iap_info_t *info = (iap_info_t *)(IAP_INFO_START_ADDR);
	memcpy(&iap_info,info,sizeof(iap_info_t));
//	flash_read(IAP_INFO_START_ADDR,(uint8_t *)&iap_info,(uint16_t)sizeof(iap_info_t));
	if(iap_info.info_crc == IAP_INFO_CRC){
		
	}else{
		iap_info_init();	//初始化iap信息
		iap_info_save();
	}
}

void iap_init(void)
{
	iap_info_read();
	iap_delay_time = IAP_TIMEOUT;
	iap_jump_time  = IAP_TIMEOUT;
	iap_info.process = IAP_PROCESS_IDLE;
	iap_info.iap_flag = IAP_FLAG_IDLE;
	
	modbus_reg_write(REG_IAP_VERSION_AA,(uint16_t *)(iap_info.run_app_addr+APP_INFO_OFFSET),6);
}


void iap_cache_block_erase(void)
{
	flash_erase_cache_block();	
}

uint16_t iap_read_bytes(void)
{
	uint16_t data_num;
	if(iap_fifo.pos < iap_fifo.tail){
		data_num = iap_fifo.tail - iap_fifo.pos;
	}else if(iap_fifo.pos > iap_fifo.tail){
		data_num = IAP_FIFO_SIZE - iap_fifo.pos + iap_fifo.tail;
	}else{
		data_num = 0;
	}
	return data_num;
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

ErrorStatus iap_app_name_check(uint8_t *pdata,uint16_t num)
{
	app_info_t *new_app_info = (app_info_t *)(pdata + APP_INFO_OFFSET);
	if(0 == strcmp(new_app_info->name,app_name)){
		return SUCCESS;
	}else{
		iap_info.iap_status = IAP_STATUS_BIN_ERROR;
		return ERROR;
	}
}
ErrorStatus iap_app_version_check(uint8_t *pdata,uint16_t num)
{
	app_version_t *run_app_version = (app_version_t *)(iap_info.run_app_addr + APP_INFO_OFFSET);
	app_version_t *new_app_version = (app_version_t *)(pdata + APP_INFO_OFFSET);
	
	if((run_app_version->aa == 0xFFFF) && (run_app_version->bb == 0xFFFF) && (run_app_version->cc == 0xFFFF)){
		return SUCCESS;
	}else{;}
	
	if(0 > memcmp(run_app_version,new_app_version,6)){
		return SUCCESS;	
	}else{
		iap_info.iap_status = IAP_STATUS_VERSION_ERROR;
		return ERROR;
	}
	
//	if(new_app_version->aa < run_app_version->aa){
//		iap_info.iap_status = IAP_STATUS_VERSION_ERROR;
//		return ERROR;
//	}else if(new_app_version->aa > run_app_version->aa){
//		return SUCCESS;
//	}else{;}
//		
//	if(new_app_version->bb < run_app_version->bb){
//		iap_info.iap_status = IAP_STATUS_VERSION_ERROR;
//		return ERROR;
//	}else if(new_app_version->bb > run_app_version->bb){
//		return SUCCESS;
//	}else{;}
//		
//	if(new_app_version->cc <= run_app_version->cc){
//		iap_info.iap_status = IAP_STATUS_VERSION_ERROR;
//		return ERROR;
//	}else{
//		return SUCCESS;	
//	}
	
}

ErrorStatus iap_start_check(void)
{
	ErrorStatus status = SUCCESS;
	if(iap_info.iap_flag == IAP_FLAG_START){
		return  status;
	}else{
		status = ERROR;
		return status;
	}
}

ErrorStatus iap_bin_size_check(void)
{
	ErrorStatus status = SUCCESS;
	if(iap_info.upgrade_size > APP_MAX_SIZE){
		iap_info.iap_status = IAP_STATUS_BIN_OVER;
		status = ERROR;
		return status;
	}else{
		iap_info.iap_status = IAP_STATUS_UPGRADING;
		modbus_reg_write(REG_IAP_START_FLAG,&iap_info.iap_status,1);
		return  status;
	}
}

ErrorStatus iap_head_check(void)
{
	ErrorStatus status = SUCCESS;
	uint16_t read_bytes = 0;
	uint8_t read_data[1024] = {0};
	if(iap_read_bytes() >= 1024){
		read_bytes = iap_read_data(read_data,1024,100);
		if(ERROR == iap_app_name_check(read_data,1024)){
			status = ERROR;
			return status;
		}else{;}
			
		if(ERROR == iap_app_version_check(read_data,1024)){
			status = ERROR;
			return status;
		}else{;}
		
		if((CACHE_START_ADDR+iap_info.bin_size+read_bytes) >= CACHE_START_ADDR+iap_info.erase_page*FLASH_PAGE_SIZE){
			flash_erase_one_page(CACHE_START_ADDR+iap_info.erase_page*FLASH_PAGE_SIZE);
			iap_info.erase_page++;
		}else{;}
			
		flash_write(CACHE_START_ADDR+iap_info.bin_size,(uint16_t *)read_data,(read_bytes+1)/2);
		iap_info.bin_size = iap_info.bin_size + read_bytes;
		iap_delay_time = IAP_TIMEOUT;
		return SUCCESS;
	}else{;}
	
	status = ERROR;		
	return status;
	
}

ErrorStatus iap_finish_check(void)
{
	ErrorStatus status = ERROR;
	if(iap_fifo.pos == iap_fifo.tail){
		if(iap_info.iap_flag == IAP_FLAG_FINISH){
			status = SUCCESS;
		}else{
			status = ERROR;
		}
	}else{;}
	return status;
}

ErrorStatus iap_timeout_check(void)
{
	ErrorStatus status = ERROR;
	if(iap_delay_time == 0){
		iap_info.iap_status = IAP_STATUS_TIMEOUT;
		status = SUCCESS;
		return status;
	}else{
		return status;
	}
	
}



ErrorStatus iap_upgrade(void)
{
	ErrorStatus status = SUCCESS;
	uint8_t read_data[1024] = {0};
	uint16_t read_bytes = 0;

	read_bytes = iap_read_data(read_data,1024,100);
	if(read_bytes == 0)  return status;
	if((iap_info.bin_size+read_bytes) < APP_MAX_SIZE){
		if((CACHE_START_ADDR+iap_info.bin_size+read_bytes) >= CACHE_START_ADDR+iap_info.erase_page*FLASH_PAGE_SIZE){
			flash_erase_one_page(CACHE_START_ADDR+iap_info.erase_page*FLASH_PAGE_SIZE);
			iap_info.erase_page++;
		}else{;}
		
		flash_write(iap_info.cache_addr+iap_info.bin_size,(uint16_t *)read_data,read_bytes/2);
		iap_info.bin_size = iap_info.bin_size + read_bytes;
	}else{
		status = ERROR;
	}
	
	iap_delay_time = IAP_TIMEOUT;
	return status;
}

ErrorStatus iap_bin_crc(void)
{
	ErrorStatus status = SUCCESS;
	uint8_t *data = (uint8_t *)(iap_info.cache_addr);
//	uint16_t cal_crc = 0xFFFF;
//	uint32_t offset = 0;
//	
//	for(offset = 0;(offset + 1024)<iap_info.bin_size;offset+=1024){
//		cal_crc = modbus_calculate_crc_ota(cal_crc,data+offset,1024);
//	} 
//	if(iap_info.bin_size%1024 > 0){
//		cal_crc = modbus_calculate_crc_ota(cal_crc,data+offset,iap_info.bin_size%1024);
//	}
//	iap_info.bin_crc = cal_crc;
	iap_info.bin_crc = iap_crc_calculate(data,iap_info.bin_size);
	
	if(iap_info.bin_crc == iap_info.upgrade_crc){
		iap_info.iap_status = IAP_STATUS_FINISH;
		modbus_reg_write(REG_IAP_START_FLAG,&iap_info.iap_status,1);
		status = SUCCESS;
	}else{
		iap_info.iap_status = IAP_STATUS_CRC_ERROR;
		status = ERROR;
	}
	
	return status;
}

void iap_upgrade_process(void *param)
{
	switch(iap_info.process)
	{
		case IAP_PROCESS_IDLE:
			if(iap_start_check()){
				if(iap_bin_size_check()){
					iap_info.process = IAP_PROCESS_HEAD_CHECK;
					iap_info.bin_size = 0;
					iap_info.bin_crc = 0;
					iap_info.erase_page = 0;
					iap_fifo.pos = 0;
					iap_fifo.tail = 0;
					iap_delay_time = IAP_TIMEOUT;
//					iap_cache_block_erase();
				}else{
					iap_info.process = IAP_PROCESS_ERROR;
				}
			}else{;}
			break;
		case IAP_PROCESS_HEAD_CHECK:
			if(iap_head_check()){
				iap_info.process = IAP_PROCESS_UPGRADE;
			}else{;}
			if(iap_timeout_check()){
				iap_info.process = IAP_PROCESS_ERROR;
			}else{;}				
			break;
		case IAP_PROCESS_UPGRADE:
			iap_upgrade();
			if(iap_finish_check()){
				iap_info.process = IAP_PROCESS_FINISH;
			}else{;}
				
			if(iap_timeout_check()){
				iap_info.process = IAP_PROCESS_ERROR;
			}else{;}
			break;
		case IAP_PROCESS_FINISH:
			if(iap_bin_crc()){
				iap_info.upgrade_flag = IAP_UPGRADE_FLAG_YES;
				iap_info_save();
				/*重启设备*/
				NVIC_SystemReset();
			}else{
				iap_info.process = IAP_PROCESS_ERROR;
			}
		case IAP_PROCESS_ERROR:
			iap_info.process = IAP_PROCESS_IDLE;
			iap_info.bin_size = 0;
			iap_info.bin_crc = 0;
			iap_fifo.pos = 0;
			iap_fifo.tail = 0;
			modbus_reg_write(REG_IAP_START_FLAG,&iap_info.iap_status,1);
			break;
		default:
			iap_info.bin_size = 0;
			iap_info.bin_crc = 0;
			iap_info.process = IAP_PROCESS_IDLE;
			break;	
	}
}


void iap_jump_app(uint32_t app_entre_addr)
{
	uint32_t jumpAdderss = *(__IO uint32_t *)(app_entre_addr+4);
	
	pAppFunction app_main;
	
	// 关闭初始化的外设
	/* NVIC */
	__disable_irq();
	SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

	__set_MSP(*(__IO uint32_t *)(app_entre_addr));
	SCB->VTOR = app_entre_addr;

//	jumpAdderss = *(__IO uint32_t *)(app_entre_addr + 4);
	app_main = (pAppFunction)jumpAdderss;
	__enable_irq();
	app_main();	
}

void iap_task_callback(void *param)
{
	if(iap_delay_time > 0){
		iap_delay_time--;
	}else{;}
			
#if(APP_TYPE == FACTORY_APP)			//出厂APP才需要跳转	
	if(iap_info.run_app_addr == FACTORY_APP_START_ADDR){
		iap_jump_time = IAP_TIMEOUT;
	}else{
		if(iap_jump_time > 0){
			iap_jump_time--;
		}else{;}
	}
		
		
	if(iap_info.process ==  IAP_PROCESS_IDLE && iap_jump_time == 0){
	if(iap_app_name_check((uint8_t *)APP_START_ADDR,1024)){
		iap_jump_app(APP_START_ADDR);
	}else{
		iap_info.run_app_addr = FACTORY_APP_START_ADDR;
	}
	}else{;}
#endif	
}

ErrorStatus iap_app_update(void)
{
	ErrorStatus status = ERROR;
	const uint16_t write_size = 1024;
	static uint32_t complete_bin = 0;
	static uint32_t app_area = (uint32_t)APP_START_ADDR;
	static uint16_t *cache_area = (uint16_t *)CACHE_START_ADDR;
	static uint16_t page_num = 0;
	uint8_t reply = 3;
	
	if((APP_START_ADDR+complete_bin+write_size) >= (APP_START_ADDR+FLASH_PAGE_SIZE*page_num)){
//		flash_erase_one_page(APP_START_ADDR+FLASH_PAGE_SIZE*page_num);
		while((ERROR == flash_erase_one_page(APP_START_ADDR+FLASH_PAGE_SIZE*page_num)) && (reply--));
		page_num++;
	}
	
//	if(complete_bin == 0){
//		while((ERROR == flash_erase_app_block()) && (reply--));
////		flash_erase_app_block();
//	}else{;}
	
	if((complete_bin+write_size) < iap_info.bin_size)
	{
		flash_write(app_area,(uint16_t *)((__IO uint16_t *)cache_area),write_size/2);

		app_area += write_size;
		cache_area += (write_size/2);
		complete_bin += write_size;
	}else if(complete_bin != iap_info.bin_size){
		flash_write(app_area,cache_area,((iap_info.bin_size - complete_bin)+1)/2);
		app_area += (iap_info.bin_size - complete_bin);
		cache_area += ((iap_info.bin_size - complete_bin)+1)/2;
		complete_bin += (iap_info.bin_size - complete_bin);
	}else{
		iap_info.upgrade_flag = IAP_UPGRADE_FLAG_NO;
		iap_info.run_app_addr = APP_START_ADDR;
		iap_info_save();
	}
	
	
	iap_jump_time = IAP_TIMEOUT;
	return status;
}


void iap_task(void)
{

	static uint32_t start_time = 0,time = 1000;
	static uint8_t  start_flag = 0;
	
	/*iap跳转过程，仅factory_APP存在*/
	if(start_flag == 0){
		start_time = system_tick_get();
		start_flag = 1;
	}
	
	if(system_timer_triggered(start_time,time)){
		start_flag = 0;
		sysTask_publish(iap_task_callback,NULL);
	}

	/*iap升级过程*/
	if(iap_info.upgrade_flag == IAP_UPGRADE_FLAG_YES){
#if(APP_TYPE == FACTORY_APP)
		iap_app_update();								//存在更新进行数据搬运乒乓搬运
#else
		NVIC_SystemReset();
#endif	
	}else{
		iap_upgrade_process(NULL);						//进行更行流程
	}
}

void iap_msg_deal_handler(uint8_t *data,uint16_t length)
{
	if(data[2] != 0x02)  return;
	uint16_t data_num = data[6]<<8 | data[7];
	if(data[3] == 0x00){
		iap_info.iap_flag = IAP_FLAG_START;
		iap_info.upgrade_size = (data[8]<<0) | (data[9]<<8) | (data[10]<<16) |(data[11]<<24) ;
		return;
	}else if(data[3] == 0x11){
		iap_info.iap_flag = IAP_FLAG_FINISH;
		iap_info.upgrade_crc = (data[8]<<0) | (data[9]<<8);
		return;
	}else if(data[3] == 0x10){
		iap_info.iap_flag = IAP_FLAG_IDLE;
	}else{
		return;
	}	
	iap_write_data(&data[8],data_num,500);		//将文件发送到iap
}






