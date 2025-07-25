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

extern const char *app_name;

#endif

