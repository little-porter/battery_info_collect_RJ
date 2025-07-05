
#ifndef __CMD_LIST_H__
#define __CMD_LIST_H__

#include "console.h"

#define ERRO_MODE           port_printf(0,"erro:please open debug mode frist\r\n")
#define ERRO_PA             port_printf(0,"erro:invalid parameter!!\r\n")

#define VERSION             "v0.00.00/20240923"

extern const cmd_struct_t g_cmd_list[];
extern uint8_t IAPMODE;
extern uint8_t NOSLEEP;
extern uint8_t poweroffCount;
void printfAllSate(char port, int check);


#endif
