

// ��ɫ
// echo -e "\033[30m ��ɫ�� \033[0m"
// echo -e "\033[31m ��ɫ�� \033[0m"
// echo -e "\033[32m ��ɫ�� \033[0m"
// echo -e "\033[33m ��ɫ�� \033[0m"
// echo -e "\033[34m ��ɫ�� \033[0m"
// echo -e "\033[35m ��ɫ�� \033[0m"
// echo -e "\033[36m ������ \033[0m"
// echo -e "\033[37m ��ɫ�� \033[0m"
// ���屳��ɫ
// echo -e "\033[40;37m �ڵװ��� \033[0m"
// echo -e "\033[41;37m ��װ��� \033[0m"
// echo -e "\033[42;37m �̵װ��� \033[0m"
// echo -e "\033[43;37m �Ƶװ��� \033[0m"
// echo -e "\033[44;37m ���װ��� \033[0m"
// echo -e "\033[45;37m �ϵװ��� \033[0m"
// echo -e "\033[46;37m �����װ��� \033[0m"
// echo -e "\033[47;30m �׵׺��� \033[0m"
// ����
// \33[0m �ر���������
// \33[1m ���ø�����
// \33[4m �»���
// \33[5m ��˸
// \33[7m ����
// \33[8m ����
// \33[30m �� \33[37m ����ǰ��ɫ
// \33[40m �� \33[47m ���ñ���ɫ
// \33[nA �������n��
// \33[nB �������n��
// \33[nC �������n��
// \33[nD �������n��
// \33[y;xH���ù��λ��
// \33[2J ����
// \33[K ����ӹ�굽��β������
// \33[s ������λ��
// \33[u �ָ����λ��
// \33[?25l ���ع��
// \33[?25h ��ʾ���

#include "stdio.h"
#include "serial.h"
#include "console.h"
#include "stdarg.h"
#include "string.h"

////////////////////////////ȫ�ֱ���/////////////////////////////////////
extern const cmd_struct_t g_cmd_list[];
char g_ConLineBuf[CONSOLE_LINE_BUFFER_SIZE];

////////////////////////////��������/////////////////////////////////////

int cmd_line(char *cmdline, char port);

uint8_t echoEnable = 0;

//////////////////////////�ӿں���ʵ��/////////////////////////////////////

/****
 * ˵����  console_init
 *         ��ʼ��console
 */
void console_Init()
{
    console_Echo(0);
}

/****
 * ˵����  console_Echo
 *         ���ÿ���̨�Ƿ����
 */
void console_Echo(int status)
{
    if (status)
    {
        echoEnable = 1;
    }
    else
    {
        echoEnable = 0;
    }
}

/****
 * ˵����  console_process
 *        ����ѭ����ʱ���ã����������С�
 */

int console_process_uart()
{
    static char lineBuffer[CONSOLE_LINE_BUFFER_SIZE];
    static int readCount = 0;
    char ch;
    int retval;
    while (1)
    {
        retval = Serial_Read(CONSOLE_SERIAL_DEV, &ch, 1);
        if (retval <= 0)
        {
            // û�ж������������˳���
            return 1;
        }
        // �������ˡ�
        // ������˸����
        if ((ch == 0x7f) || ch == 0x08)
        {
            if (readCount != 0)
            {
                if (echoEnable)
                {
                    uart_printf("%c %c", ch, ch);
                }
                readCount--;
            }
            continue;
        }

        // ����ǻ��з�
        if ((ch == '\r') || (ch == '\n'))
        {
            lineBuffer[readCount] = '\r';
            lineBuffer[readCount + 1] = '\n';
            lineBuffer[readCount + 2] = '\0';
            if (echoEnable)
            {
                Serial_Write(CONSOLE_SERIAL_DEV, &lineBuffer[readCount], 2);
            }
            else
            {
                uart_printf("\r\n");
            }
            // ���У������������
            retval = cmd_line(lineBuffer, 0);
            readCount = 0;
            // uart_printf(CONSOLE_HEAD);
            return retval;
        }

        // ��������������ַ�
        if (readCount < CONSOLE_LINE_BUFFER_SIZE)
        {
            lineBuffer[readCount++] = ch;
            if (echoEnable)
            {
                Serial_Write(CONSOLE_SERIAL_DEV, &ch, 1);
            }
        }
    }
}

 

int console_getch()
{
    char ch;
    int retval = 0;

    retval = Serial_Read(CONSOLE_SERIAL_DEV, &ch, 1);

    if (retval <= 0)
    {
        retval = -1;
    }
    else
    {
        retval = ch;
    }

    return retval;
}

//////////////////////////�ڲ���˽�к���/////////////////////////////////////

/****
 * ˵����  clear_argv
 *        ���verctor
 */
void clear_argv(char *argv[])
{
    int count = 0;

    while (count < MAX_ARG_COUNT)
    {
        argv[count] = NULL;
    }
}

/****
 * ˵����  find_cmd
 *        ����һ������ṹ
 * ������ cmd�����������
 * ���أ�  ����һ�����ҵ�������ṹ�壬���û�ҵ�������NULL
 */
const cmd_struct_t *find_cmd(char *cmd)
{
    int count = 0;

    while (g_cmd_list[count].cmd != NULL) // ��������ṹ��
    {
        if (strcmp(cmd, g_cmd_list[count].cmd) == 0) // �Ƚ���������
        {
            return &g_cmd_list[count];
        }
        count++;
    }

    return NULL;
}

/****
 * ˵����   get_arg_vector
 *          ���ݿ���̨�����һ�����ݣ��������������
 * ������   str�������һ������
 * ������   argv����������
 * ���أ�   ��������
 */
int get_arg_vector(char *str, char *argv[])
{
    int argc = 0;

    char *ptr_search = str;

    argv[argc] = ptr_search; // ��ֵ��һ������

    // ��������
    while ((*ptr_search != '\r') && (*ptr_search != '\n') && (*ptr_search != '\0'))
    {
        ptr_search++;
        if ((*ptr_search == ' ') || (*ptr_search == '=') || (*ptr_search == ','))
        {
            argc++;
            *ptr_search = '\0';
            ptr_search++;
            argv[argc] = ptr_search;
        }
    }
    argc++;
    argv[argc] = NULL;
    *ptr_search = '\0';

    return argc;
}

/****
 * ˵����   cmd_line
 *          �������̨�����һ��
 * ������   cmdline�������һ������
 * ���أ�   ������
 */
int cmd_line(char *cmdline, char port)
{
    int argc = 0;
    char *cmdBuffer = cmdline;
    char *(argv[MAX_ARG_COUNT]);
    const cmd_struct_t *cmd_struct;

    if ((cmdBuffer[0] == '\r') || (cmdBuffer[0] == '\n')) // �ж��ǲ���ֻ�л��з�
    {
        return -3;
    }

    argc = get_arg_vector(cmdBuffer, argv); // ����������

    if (argc == 0) // ������������
    {
        return -1;
    }

    cmd_struct = find_cmd(argv[0]); // ���ҵ�һ��������Ӧ�ĳ��򣬷�������ṹ��

    if (cmd_struct == NULL) // û���������
    {
        port_printf(port, " can not find this cmd\r\n");
        return -2;
    }

    // �����Ѿ��ҵ��ĳ���
    cmd_struct->cmd_proc(argc, argv, port);

    return 0;
}

 int error(const char *fmt, ...)
 {
     va_list argp;
     int retval;

     va_start(argp, fmt);

     printf("\033[31;5m[ERROR]\033[0m");

     printf("\033[31m");
     retval = vprintf(fmt, argp);

     printf("\033[0m");

     va_end(argp);

     return retval;
 }

// int warning(const char *fmt, ...)
// {
//     __gnuc_va_list argp;
//     int retval;

//     va_start(argp, fmt);

//     printf("\033[33;5m[Warning]\033[0m");

//     printf("\033[33m");
//     retval = vprintf(fmt, argp);

//     printf("\033[0m");

//     va_end(argp);

//     return retval;
// }

// int at_cmd_info(const char *fmt, ...)
// {
//     __gnuc_va_list argp;
//     int retval;
//     va_start(argp, fmt);
//     printf("\033[35;4m[atcmd]\033[0m");
//     // printf("\033[33m");
//     retval = vprintf(fmt, argp);
//     // printf("\033[0m");
//     va_end(argp);
//     return retval;
// }

// int info(const char *fmt, ...)
// {
//     __gnuc_va_list argp;
//     int retval;
//     va_start(argp, fmt);
//     printf("\033[35;4m[info]\033[0m");
//     // printf("\033[33m");
//     retval = vprintf(fmt, argp);
//     // printf("\033[0m");
//     va_end(argp);

//     return retval;
// }
// int _debug(const char *fmt, ...)
// {
//     __gnuc_va_list argp;
//     int retval;

//     va_start(argp, fmt);

//     printf("\033[36;4m[debug]\033[0m");

//     // printf("\033[33m");
//     retval = vprintf(fmt, argp);

//     // printf("\033[0m");

//     va_end(argp);

//     return retval;
// }

int uart_printf(char *fmt, ...)
{
    char buf[CONSOLE_LINE_BUFFER_SIZE + 1];
    int len;
    int retval;
	va_list argp;
    va_start(argp, fmt);
    retval = vsnprintf(buf, CONSOLE_LINE_BUFFER_SIZE + 1, fmt, argp);
    len = strlen(buf);
    Serial_Write(CONSOLE_SERIAL_DEV, buf, len);
    va_end(argp);
    return retval;
}


int port_printf(char port, char *fmt, ...)
{
    char buf[CONSOLE_LINE_BUFFER_SIZE + 1];
    int len;
    int retval;
    va_list argp;
    va_start(argp, fmt);
    retval = vsnprintf(buf, CONSOLE_LINE_BUFFER_SIZE + 1, fmt, argp);
    len = strlen(buf);
    Serial_Write(CONSOLE_SERIAL_DEV, buf, len);
    va_end(argp);
    return retval;
}

