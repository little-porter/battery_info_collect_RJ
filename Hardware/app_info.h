#ifndef __APP_INFO_H
#define __APP_INFO_H

#include "system.h"


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


typedef struct _app_version
{
	uint8_t aa;
	uint8_t bb;
	uint8_t cc;
	uint8_t day;
	uint8_t month;
	uint16_t year;
}app_version_t;

typedef struct _app_info
{
	app_version_t version;
	char name[100];
	char describe[100];
}app_info_t;

extern const char *app_name;
extern const app_info_t *factory_app_info;
extern const app_info_t *app1_info;
extern const app_info_t *app2_info;

extern const app_version_t *factory_version;
extern const app_version_t *app1_version;
extern const app_version_t *app2_version;

#endif

