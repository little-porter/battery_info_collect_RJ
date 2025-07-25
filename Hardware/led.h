#ifndef __LED_H__
#define __LED_H__


#include "system.h"
#include "sysTask.h"

typedef enum _led_run_status
{
	LED_RUN_STATUS_ERROR = 0,
	LED_RUN_STATUS_WORK,
	LED_RUN_STATUS_UPGRADING,
}led_run_status_t;


void led_init(void);
void led_task(void);

void led_sys_status_set(led_run_status_t run_status);
void led_change_task(void);

#endif

