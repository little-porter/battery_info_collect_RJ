#include "iap.h"
#include <stdint.h>
#include <string.h>



#include "app_info.h"

#define FACTORY_APP		0
#define IAP_APP			1
#define APP_TYPE		IAP_APP
#define APP_MAX_SIZE	0xA000				//40kb
#define REG_IAP_START_FLAG 0x0003
#define REG_IAP_VERSION_AA 0x3003
#define REG_IAP_VERSION_BB 0x3004
#define REG_IAP_VERSION_CC 0x3005
#define IAP_OUT_TIME_MS	   10000


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
	iap_run_app_t next_run_app;
	iap_app_bin_info_t run_app_bin;
	iap_app_bin_info_t factory_bin;
	iap_app_bin_info_t app1_bin;
	iap_app_bin_info_t app2_bin;
	
	uint32_t run_app_addr;
	uint32_t next_app_addr;
	
	iap_process_t process;
	uint32_t upgrade_size;
	uint16_t upgrade_crc;
	iap_falg_t    iap_flag;
	uint16_t  iap_status;
	
	uint32_t bin_size;
	uint16_t bin_crc;
	uint16_t info_crc;
}iap_info_t;

iap_info_t iap_info;

#define  IAP_TIMEOUT		10000
uint16_t   iap_delay_time = IAP_TIMEOUT;
uint8_t    app_is_executable = 0;

void iap_info_init(void)
{
	iap_info.run_app = IAP_RUN_FACTORY;
	iap_info.factory_bin.version = (app_version_t *)factory_version;
	iap_info.app1_bin.version = (app_version_t *)app1_version;
	iap_info.app2_bin.version = (app_version_t *)app2_version;
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
	uint16_t cal_size = sizeof(iap_run_app_t) + 3 * sizeof(iap_app_bin_info_t) + sizeof(iap_process_t);
	if(iap_info.info_crc == iap_crc_calculate((uint8_t *)&iap_info,cal_size)){
		
	}else{
		iap_info_init();	//初始化iap信息
		iap_info_save();
	}
}

void iap_init(void)
{
	iap_info_read();
	iap_delay_time = IAP_TIMEOUT;
	
	if(iap_info.run_app == IAP_RUN_FACTORY){
		iap_info.run_app_addr = FACTORY_APP_START_ADDR;
		iap_info.next_run_app = IAP_RUN_APP1;
		iap_info.next_app_addr = APP1_START_ADDR;
	}else if(iap_info.run_app == IAP_RUN_APP1){
		iap_info.run_app_addr = APP1_START_ADDR;
		iap_info.next_run_app = IAP_RUN_APP2;
		iap_info.next_app_addr = APP2_START_ADDR;
	}else if(iap_info.run_app == IAP_RUN_APP2){
		iap_info.run_app_addr = APP2_START_ADDR;
		iap_info.next_run_app = IAP_RUN_APP1;
		iap_info.next_app_addr = APP1_START_ADDR;
	}else{;}
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

	jumpAdderss = *(__IO uint32_t *)(app_entre_addr + 4);
	app_main = (pAppFunction)jumpAdderss;
	__enable_irq();
	app_main();	
}

void iap_task_callback(void *param)
{
	if(iap_info.run_app == IAP_RUN_FACTORY){
		iap_delay_time = IAP_TIMEOUT;
	}else{
		if(iap_delay_time > 0){
			iap_delay_time--;
		}else{;}
	}
	
	if(iap_delay_time == 0){
		if(iap_info.run_app == IAP_RUN_APP1){
			iap_jump_app(APP1_START_ADDR);
		}else if(iap_info.run_app == IAP_RUN_APP2){
			iap_jump_app(APP2_START_ADDR);
		}else{
			iap_info.run_app = IAP_RUN_FACTORY;
		}
	}else{;}
}

void iap_next_block_erase(void)
{
	if(iap_info.run_app == IAP_RUN_FACTORY){
		flash_erase_app1_block();
	}else if(iap_info.run_app == IAP_RUN_APP1){
		flash_erase_app2_block();
	}else if(iap_info.run_app == IAP_RUN_APP2){
		flash_erase_app1_block();
	}else{;}
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
	app_version_t *new_app_version = (app_version_t *)(pdata + APP_INFO_OFFSET);
	if(new_app_version->aa < iap_info.run_app_bin.version->aa){
		iap_info.iap_status = IAP_STATUS_VERSION_ERROR;
		return ERROR;
	}else if(new_app_version->bb < iap_info.run_app_bin.version->bb){
		iap_info.iap_status = IAP_STATUS_VERSION_ERROR;
		return ERROR;
	}else if(new_app_version->cc <= iap_info.run_app_bin.version->cc){
		iap_info.iap_status = IAP_STATUS_VERSION_ERROR;
		return ERROR;
	}else{
		return SUCCESS;	
	}
	
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
	if(iap_read_bytes() > 1024){
		read_bytes = iap_read_data(read_data,1024,100);
		if(ERROR == iap_app_name_check(read_data,1024)){
			status = ERROR;
			return status;
		}else{;}
			
		if(ERROR == iap_app_version_check(read_data,1024)){
			status = ERROR;
			return status;
		}else{;}
			
		flash_write(iap_info.next_app_addr+iap_info.bin_size,(uint16_t *)read_data,read_bytes/2);
		iap_info.bin_size = iap_info.bin_size + read_bytes;
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


ErrorStatus iap_upgrade(void)
{
	ErrorStatus status = SUCCESS;
	uint8_t read_data[1024] = {0};
	uint16_t read_bytes = 0;

	read_bytes = iap_read_data(read_data,1024,100);
	if(read_bytes == 0)  return status;
	if((iap_info.bin_size+read_bytes) < APP_MAX_SIZE){
		flash_write(iap_info.next_app_addr+iap_info.bin_size,(uint16_t *)read_data,read_bytes/2);
		iap_info.bin_size = iap_info.bin_size + read_bytes;
	}else{
		status = ERROR;
	}
	
	return status;
}

ErrorStatus iap_bin_crc(void)
{
	ErrorStatus status = SUCCESS;
	uint8_t *data = (uint8_t *)(iap_info.next_app_addr);
	
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
					iap_fifo.pos = 0;
					iap_fifo.tail = 0;
					iap_next_block_erase();
				}else{
					iap_info.process = IAP_PROCESS_ERROR;
				}
			}else{;}
			break;
		case IAP_PROCESS_HEAD_CHECK:
			if(iap_head_check()){
				iap_info.process = IAP_PROCESS_UPGRADE;
			}else{;}
			break;
		case IAP_PROCESS_UPGRADE:
			iap_upgrade();
			if(iap_finish_check()){
				iap_info.process = IAP_PROCESS_FINISH;
			}else{;}
			break;
		case IAP_PROCESS_FINISH:
			if(iap_bin_crc()){
				iap_info.run_app = iap_info.next_run_app;
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


void iap_task(void)
{
#if(APP_TYPE == FACTORY_APP)
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
#endif	
	/*iap升级过程*/
	
}

void iap_msg_deal_handler(uint8_t *data,uint16_t length)
{
	if(data[2] != 0x02)  return;
	uint16_t data_num = data[6]<<8 | data[7];
	if(data[3] == 0x00){
		iap_info.iap_flag = IAP_FLAG_START;
		iap_info.upgrade_size = (data[8]<<24) | (data[9]<<16) | (data[10]<<8) |(data[11]<<0) ;
		return;
	}else if(data[3] == 0x11){
		iap_info.iap_flag = IAP_FLAG_FINISH;
		iap_info.upgrade_crc = (data[8]<<8) | data[9];
		return;
	}else if(data[3] == 0x10){
		iap_info.iap_flag = IAP_FLAG_IDLE;
	}else{
		return;
	}	
	iap_write_data(&data[8],data_num,500);		//将文件发送到iap
}






