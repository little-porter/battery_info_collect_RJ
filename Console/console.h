
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "stdio.h"
#include "serial.h"
#include "stdarg.h"

////////////////////////////�����궨��/////////////////////////////////////

#ifdef DEBUG
#define debug(...) _debug(__VA_ARGS__)
#else
#define debug(...)
#endif

////////////////////////////�����궨��/////////////////////////////////////

#define CONSOLE_SERIAL_DEV &g_SerialDev1

#define CONSOLE_DEV_NAME "serial"

#define CONSOLE_HEAD "root#:"

#define CONSOLE_LINE_BUFFER_SIZE 512
#define MAX_ARG_COUNT 20

////////////////////////////�������Ͷ���/////////////////////////////////////
typedef int (*cmd_proc_t)(int argc, char *argv[], char port);
typedef struct CMD_STRUCT cmd_struct_t;
struct CMD_STRUCT
{
    char *cmd;           // ��������
    cmd_proc_t cmd_proc; // �������
    char *cmd_explain;   // ����˵��
};

extern uint8_t ble_console_port;
extern uint8_t ble_console_mode;
extern uint8_t debug_printf;
extern uint8_t debug_printferr;
extern uint8_t echoEnable;
////////////////////////////��������/////////////////////////////////////

void console_Init(void);
void console_Echo(int status);

int console_process_uart(void);
int console_process_socket(void);
int console_getch(void);
 int error(const char *fmt, ...);
// int warning(const char *fmt, ...);
// int info(const char *fmt, ...);
// int _debug(const char *fmt, ...);
// int at_cmd_info(const char *fmt, ...);
int uart_printf(char *fmt, ...);
int port_printf(char port, char *fmt, ...);

#endif

////////////////////////����û����/////////////////////////////////
