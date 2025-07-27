#include "app_info.h"



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








