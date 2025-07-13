#include "system.h"

/*pravate*/
private volatile uint32_t sysTime = 0;


void system_incTick(void)
{
	sysTime++;
}

uint32_t system_tick_get(void)
{
	return sysTime;
}

bool system_timer_triggered(uint32_t start,uint32_t time)
{
	uint32_t nowTime = 0,space = 0;
	nowTime = system_tick_get();				//获取当前时间
	
	/*计算时间间隔*/
	if(nowTime < start){
		space = 0xFFFFFFFF - start + nowTime;
	}else{
		space = nowTime - start;
	}
	
	if(space > time){
		return true;
	}else
	{
		return false;
	}	
}

