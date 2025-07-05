#include "serial.h"
#include "stdio.h"
#include "cmd_list.h"
#include <stdlib.h>
#include <string.h>


int cmd_Help(int argc, char *argv[], char port)
{
	int i = 0;
	const cmd_struct_t *cmd;
	port_printf(port, "*********cmd list*********\r\n");
	do
	{
		cmd = &g_cmd_list[i];

		port_printf(port, "%s\t\t:\t%s\r\n", cmd->cmd, cmd->cmd_explain);
		i++;

	} while (g_cmd_list[i].cmd != NULL);
	port_printf(port, "*********list end*********\r\n");

	return 0;
}

int cmd_Echo(int argc, char *argv[], char port)
{
	//	int i = 0;
	if (argc != 2)
	{
		port_printf(port, "usage:%s [1|0]\r\n", argv[0]);
	}

	if (argv[1][0] == '1')
	{
		console_Echo(1);
	}
	else
	{
		console_Echo(0);
	}

	return 0;
}

void printfAllSate(char port, int check)
{
	//port_printf(port, "<STATE> %d %d %d %d %f %f %f %d %d %d %f %d %d %d %d %d %d %d %d %d %d %s\r\n", alpos_inst.idle, alpos_inst.autoState, RelaySate.CurRes, RelaySate.Probe, alpos_inst.scanStartVol, alpos_inst.scanStopVol, alpos_inst.scanStepVol, alpos_inst.sampleAvgNum, alpos_inst.lineAvgNum, alpos_inst.sampleSpeed, PowertAmp_inst.voltageSet, PowertAmp_inst.onoff, RelaySate.OutPut, alpos_inst.scanStepDelaySet, alpos_inst.autoread, alpos_inst.scanSate, alpos_inst.scanStartDelay, RelaySate.CurSide, RelaySate.Ref_10M, RelaySate.MainRef, trigger, MAIN_VERSION);
}
int cmd_sysinfo(int argc, char *argv[], char port)
{
	return 0;
}


//int version(int argc, char *argv[], char port)
//{
//	if (argc > 1)
//	{
//		port_printf(port, "<VERSION> %s\r\n", VERSION);
//	}
//	return 0;
//}

//int debug_info(int argc, char *argv[], char port)
//{
//	if(argc != 2)
//	{
//		ERRO_PA;
//		return  -1;
//	}		
//	
//	if(atoi(argv[1]) == 1)
//		setDebugInfoPrint(ON);
//	else if(atoi(argv[1]) == 0)
//		setDebugInfoPrint(OFF);
//	else
//	{
//		ERRO_PA;
//		return -1;
//	}
//	return 0;
//}

//int debug_power(int argc, char *argv[], char port)
//{
//	if(argc != 2)
//	{
//		ERRO_PA;
//		return  -1;
//	}		
//	
//	if(atoi(argv[1]) == 1)
//		useDebugMode = 1;
//	else if(atoi(argv[1]) == 0)
//		useDebugMode = 0;
//	else
//	{
//		ERRO_PA;
//		return -1;
//	}
//	port_printf(0,"ok\r\n");
//	return 0;
//}



const cmd_struct_t g_cmd_list[] =
{
		{"echo",        cmd_Echo,    "test cmd funtion"},
		{"help",        cmd_Help,    "test cmd funtion"},
		{NULL, NULL, NULL},
};
