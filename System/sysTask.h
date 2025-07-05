#ifndef __SYSTASK_H__
#define __SYSTASK_H__


#include "system.h"


void sysTask_init(void);
void sysTask_publish(void *task,void *param);
void sysTask_execute_handler(void);



#endif

