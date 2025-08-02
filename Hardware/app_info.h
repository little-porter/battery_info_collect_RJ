#ifndef __APP_INFO_H
#define __APP_INFO_H

#include "system.h"


#define FACTORY_APP_START_ADDR		0x08000000			
#define FACTORY_APP_SIZE			0x7800				//30kb
#define APP_START_ADDR				(FACTORY_APP_START_ADDR + FACTORY_APP_SIZE)		
#define APP_SIZE					0xA000				//40kb
#define CACHE_START_ADDR			(APP_START_ADDR + APP_SIZE)		
#define CACHE_SIZE					0xA000				//40kb

#define IAP_INFO_START_ADDR			(CACHE_START_ADDR + CACHE_SIZE)		//0x0801B800//
#define IAP_INFO_SIZE				0x800				//2kb
#define USER_INFO_START_ADDR		(IAP_INFO_START_ADDR + IAP_INFO_SIZE)
#define USER_INFO_SIZE				0x800				//2kb

#define APP_INFO_OFFSET				0x300

//#pragma pack(1)     // 设置为1字节对齐，实现最紧密排列
typedef struct _app_version
{
	uint16_t aa;
	uint16_t bb;
	uint16_t cc;
	uint16_t day;
	uint16_t month;
	uint16_t year;
}app_version_t;

typedef struct _app_info
{
	app_version_t version;
	char name[100];
	char describe[100];
}app_info_t;
//#pragma pack()     // 设置为1字节对齐，实现最紧密排列


extern const char *app_name;
extern const app_info_t *factory_app_info;
extern const app_info_t *app_info;
extern const app_info_t *cache_info;

extern const app_version_t *factory_version;
extern const app_version_t *app_version;
extern const app_version_t *cache_version;

#endif

