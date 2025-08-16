#include "app_info.h"



const char *app_name = "电池信息采集软件";

const app_info_t bsp_app_info __attribute__((at(FACTORY_APP_START_ADDR+APP_INFO_OFFSET))) = {
	.version = {2025,7,25,0,0,0},
	.name = "电池信息采集软件",
	.describe = "天微电子——锂电",
};


const app_info_t *factory_app_info = (app_info_t *)(FACTORY_APP_START_ADDR + APP_INFO_OFFSET);
const app_info_t *app_info		   = (app_info_t *)(APP_START_ADDR + APP_INFO_OFFSET);
const app_info_t *cache_info		   = (app_info_t *)(CACHE_START_ADDR + APP_INFO_OFFSET);

const app_version_t *factory_version = (app_version_t *)(FACTORY_APP_START_ADDR + APP_INFO_OFFSET);
const app_version_t *app1_version = (app_version_t *)(APP_START_ADDR + APP_INFO_OFFSET);
const app_version_t *app2_version = (app_version_t *)(CACHE_START_ADDR + APP_INFO_OFFSET);








