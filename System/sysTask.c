#include "sysTask.h"



typedef void (*sysTaskFunc)(void *param);
#define SYSTASK_FIFO_SIZE	20	

typedef enum _sysTask_flag
{
	SYSTASK_CANCEL = 0,
	SYSTASK_REGISTER = 1,
}sysTask_flag_t;

typedef struct _sysTask_event
{
	sysTaskFunc task;
	void        *param;	
	sysTask_flag_t flag;
}sysTask_event_t;

typedef struct _sysTask_fifo
{
	sysTask_event_t event[SYSTASK_FIFO_SIZE];
	uint16_t    	pos;
	uint16_t    	tail;
}sysTask_fifo_t;

/*private*/
private sysTask_fifo_t sysTask_fifo;



void sysTask_init(void)
{
	sysTask_fifo.pos = 0;
	sysTask_fifo.tail = 0;
}


void sysTask_publish(void *task,void *param)
{
	sysTask_event_t event;
	event.task = (sysTaskFunc)task;
	event.param = param;
	event.flag = SYSTASK_REGISTER;
	
	sysTask_fifo.event[sysTask_fifo.tail] = event;
	sysTask_fifo.tail++;
	sysTask_fifo.tail %= SYSTASK_FIFO_SIZE;
}


void sysTask_execute_handler(void)
{
	while(sysTask_fifo.pos != sysTask_fifo.tail)
	{
		if(sysTask_fifo.event[sysTask_fifo.pos].flag == SYSTASK_REGISTER){
			sysTask_fifo.event[sysTask_fifo.pos].flag = SYSTASK_CANCEL;
		}else{
			sysTask_fifo.pos++;
			sysTask_fifo.pos %= SYSTASK_FIFO_SIZE;
			continue;
		}
		sysTask_fifo.event[sysTask_fifo.pos].task(sysTask_fifo.event[sysTask_fifo.pos].param);
		sysTask_fifo.pos++;
		sysTask_fifo.pos %= SYSTASK_FIFO_SIZE;
	}
}







