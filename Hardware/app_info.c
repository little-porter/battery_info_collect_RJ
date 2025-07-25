#include "app_info.h"


#define FACTORY_APP_START_ADDR		0x08000000			
#define FACTORY_APP_SIZE			0x7800				//30kb
#define APP1_START_ADDR				(FACTORY_APP_START_ADDR + FACTORY_APP_SIZE)		
#define APP1_SIZE					0xA000				//40kb
#define APP2_START_ADDR				(APP1_START_ADDR + APP1_SIZE)		
#define APP2_SIZE					0xA000				//40kb

#define IAP_INFO_START_ADDR			(APP2_START_ADDR + APP2_SIZE)
#define IAP_INFO_SIZE				0x800				//2kb
#define USER_INFO_START_ADDR		(IAP_INFO_START_ADDR + IAP_INFO_SIZE)
#define USER_INFO_SIZE				0x800				//2kb

#define APP_INFO_OFFSET				0x300

const char *app_name = "电池信息采集软件";

const app_info_t app_info __attribute__((at(FLASH_BASE+APP_INFO_OFFSET))) = {
	.version = {0,0,0,25,7,2025},
	.name = "电池信息采集软件",
	.describe = "天微电子——锂电",
};


const app_info_t *factory_app_info = (app_info_t *)(FACTORY_APP_START_ADDR + APP_INFO_OFFSET);
const app_info_t *app1_info		   = (app_info_t *)(APP1_START_ADDR + APP_INFO_OFFSET);
const app_info_t *app2_info		   = (app_info_t *)(APP2_START_ADDR + APP_INFO_OFFSET);

const app_version_t *factory_version = (app_version_t *)(FACTORY_APP_START_ADDR + APP_INFO_OFFSET);
const app_version_t *app1_version = (app_version_t *)(APP1_START_ADDR + APP_INFO_OFFSET);
const app_version_t *app2_version = (app_version_t *)(APP2_START_ADDR + APP_INFO_OFFSET);







