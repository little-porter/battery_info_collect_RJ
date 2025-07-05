#include "stm32f30x.h"
#include "serial.h"
#include "fifo.h"
#include "console.h"

serial_dev_t g_SerialDev1; // UART 数据

static uint8_t Serial1txData;
//static uint8_t Serial1rxData;

uint8_t serial1TxBuffer[SERIAL1_TX_BUFFER_LEN];
uint8_t serial1RxBuffer[SERIAL1_RX_BUFFER_LEN];

int Serial_Init(void)
{
    g_SerialDev1.usart_periph = 1;
    Fifo_Init(&g_SerialDev1.rxFifo, serial1RxBuffer, SERIAL1_RX_BUFFER_LEN);
    Fifo_Init(&g_SerialDev1.txFifo, serial1TxBuffer, SERIAL1_TX_BUFFER_LEN);
    return 0;
}

/****
 * 说明：  Serial_Read
 *         读取接口
 * 参数：  devp  : device实例
 * 参数：  buffer: 装数据用的buffer
 * 参数：  len   : 期望的数据长度
 * 参数：  *ppos : 读取偏移量，指针类型，如果需要自加可在函数中累加该变量
 * 返回： 大于0成功，并返回实际读取长度， 其他值：失败
 */
int Serial_Read(serial_dev_t *serial, char *buffer, int len)
{
    // 获得设备实体
    int vaildLen;
    int readLen = len;

    if (len == 0)
    {
        return 0;
    }

    vaildLen = Fifo_GetUsedSize(&serial->rxFifo);

    if (readLen > vaildLen)
    {
        len = readLen = vaildLen;
    }

    // 读取数据
    while (readLen--)
    {
        *buffer = Fifo_Pop(&serial->rxFifo);
        buffer++;
    }

    // 返回读写长度
    return len;
}

/****
 * 说明：  Serial_Write
 *         写入接口
 * 参数：  devp  : device实例
 * 参数：  buffer: 装数据用的buffer
 * 参数：  len   : 数据长度
 * 参数：  *ppos : 写入偏移量，指针类型，如果需要自加可在函数中累加该变量
 * 返回： 大于0成功，并返回实际写入长度， 其他值：失败
 */
int Serial_Write(serial_dev_t *serial, char *buffer, int len)
{
    int writeLen = len;
    int freeLen = 0;
    int sendLen;

    if (len == 0)
    {
        return 0;
    }

    if (serial->setTX != NULL)
    {
        serial->setTX();
        // Serial_delay(10);
    }

    // 写入数据
    while (len != 0)
    {
        freeLen = 0;
        while (freeLen == 0)
        {
            freeLen = Fifo_GetFreeSize(&serial->txFifo);
        }
        if (len > freeLen)
        {
            sendLen = freeLen;
            // return 0;
        }
        else
        {
            sendLen = len;
        }

        len -= sendLen;

        while (sendLen--)
        {
            Fifo_Push(&serial->txFifo, *buffer);
            buffer++;
        }
        // usart_interrupt_enable(serial->usart_periph, USART_INT_TBE);
    }
    // 返回读写长度
    return writeLen;
}

void Serial_ReadDataToBuffer(unsigned char ch)
{
    // Serial1rxData = ch;
    if (IsFifoFull(&g_SerialDev1.rxFifo) == false)
    {
        // Read one byte from the receive data register
        Fifo_Push(&g_SerialDev1.rxFifo, ch);
    }
}


void Serial_WriteDataToUartInst(void)
{
    if (IsFifoEmpty(&g_SerialDev1.txFifo) == false)
    {
		if(USART_GetFlagStatus(USART1,USART_FLAG_TXE))
		{
			Serial1txData = Fifo_Pop(&g_SerialDev1.txFifo);
			USART_SendData( USART1,Serial1txData);
		}
        
    }
}
