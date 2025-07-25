#ifndef __APP_INFO_H
#define __APP_INFO_H

#include "system.h"

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

#endif

