

// 颜色
// echo -e "\033[30m 黑色字 \033[0m"
// echo -e "\033[31m 红色字 \033[0m"
// echo -e "\033[32m 绿色字 \033[0m"
// echo -e "\033[33m 黄色字 \033[0m"
// echo -e "\033[34m 蓝色字 \033[0m"
// echo -e "\033[35m 紫色字 \033[0m"
// echo -e "\033[36m 天蓝字 \033[0m"
// echo -e "\033[37m 白色字 \033[0m"
// 字体背景色
// echo -e "\033[40;37m 黑底白字 \033[0m"
// echo -e "\033[41;37m 红底白字 \033[0m"
// echo -e "\033[42;37m 绿底白字 \033[0m"
// echo -e "\033[43;37m 黄底白字 \033[0m"
// echo -e "\033[44;37m 蓝底白字 \033[0m"
// echo -e "\033[45;37m 紫底白字 \033[0m"
// echo -e "\033[46;37m 天蓝底白字 \033[0m"
// echo -e "\033[47;30m 白底黑字 \033[0m"
// 其他
// \33[0m 关闭所有属性
// \33[1m 设置高亮度
// \33[4m 下划线
// \33[5m 闪烁
// \33[7m 反显
// \33[8m 消隐
// \33[30m — \33[37m 设置前景色
// \33[40m — \33[47m 设置背景色
// \33[nA 光标上移n行
// \33[nB 光标下移n行
// \33[nC 光标右移n行
// \33[nD 光标左移n行
// \33[y;xH设置光标位置
// \33[2J 清屏
// \33[K 清除从光标到行尾的内容
// \33[s 保存光标位置
// \33[u 恢复光标位置
// \33[?25l 隐藏光标
// \33[?25h 显示光标

#include "stdio.h"
#include "serial.h"
#include "console.h"
#include "stdarg.h"
#include "string.h"

////////////////////////////全局变量/////////////////////////////////////
extern const cmd_struct_t g_cmd_list[];
char g_ConLineBuf[CONSOLE_LINE_BUFFER_SIZE];

////////////////////////////函数声明/////////////////////////////////////

int cmd_line(char *cmdline, char port);

uint8_t echoEnable = 0;

//////////////////////////接口函数实现/////////////////////////////////////

/****
 * 说明：  console_init
 *         初始化console
 */
void console_Init()
{
    console_Echo(0);
}

/****
 * 说明：  console_Echo
 *         设置控制台是否回显
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
 * 说明：  console_process
 *        在主循环或定时调用，处理命令行。
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
            // 没有读到数，本次退出。
            return 1;
        }
        // 读到数了。
        // 如果是退格符号
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

        // 如果是换行符
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
            // 新行，调用命令解析
            retval = cmd_line(lineBuffer, 0);
            readCount = 0;
            // uart_printf(CONSOLE_HEAD);
            return retval;
        }

        // 如果是其他正常字符
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

//////////////////////////内部用私有函数/////////////////////////////////////

/****
 * 说明：  clear_argv
 *        清空verctor
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
 * 说明：  find_cmd
 *        查找一个命令结构
 * 参数： cmd，输入的命令
 * 返回：  返回一个查找到的命令结构体，如果没找到，返回NULL
 */
const cmd_struct_t *find_cmd(char *cmd)
{
    int count = 0;

    while (g_cmd_list[count].cmd != NULL) // 查找命令结构体
    {
        if (strcmp(cmd, g_cmd_list[count].cmd) == 0) // 比较命令名字
        {
            return &g_cmd_list[count];
        }
        count++;
    }

    return NULL;
}

/****
 * 说明：   get_arg_vector
 *          根据控制台输入的一行数据，解析出命令参数
 * 参数：   str，输入的一行命令
 * 参数：   argv，参数容器
 * 返回：   返回容器
 */
int get_arg_vector(char *str, char *argv[])
{
    int argc = 0;

    char *ptr_search = str;

    argv[argc] = ptr_search; // 赋值第一个向量

    // 查找向量
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
 * 说明：   cmd_line
 *          处理控制台输入的一行
 * 参数：   cmdline，输入的一行命令
 * 返回：   处理结果
 */
int cmd_line(char *cmdline, char port)
{
    int argc = 0;
    char *cmdBuffer = cmdline;
    char *(argv[MAX_ARG_COUNT]);
    const cmd_struct_t *cmd_struct;

    if ((cmdBuffer[0] == '\r') || (cmdBuffer[0] == '\n')) // 判断是不是只有换行符
    {
        return -3;
    }

    argc = get_arg_vector(cmdBuffer, argv); // 填充参数向量

    if (argc == 0) // 解析不出参数
    {
        return -1;
    }

    cmd_struct = find_cmd(argv[0]); // 查找第一个参数对应的程序，返回命令结构体

    if (cmd_struct == NULL) // 没有这个程序
    {
        port_printf(port, " can not find this cmd\r\n");
        return -2;
    }

    // 调用已经找到的程序
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

