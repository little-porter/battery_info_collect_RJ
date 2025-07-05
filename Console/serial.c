#include "stm32f30x.h"
#include "serial.h"
#include "fifo.h"
#include "console.h"

serial_dev_t g_SerialDev1; // UART ����

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
 * ˵����  Serial_Read
 *         ��ȡ�ӿ�
 * ������  devp  : deviceʵ��
 * ������  buffer: װ�����õ�buffer
 * ������  len   : ���������ݳ���
 * ������  *ppos : ��ȡƫ������ָ�����ͣ������Ҫ�Լӿ��ں������ۼӸñ���
 * ���أ� ����0�ɹ���������ʵ�ʶ�ȡ���ȣ� ����ֵ��ʧ��
 */
int Serial_Read(serial_dev_t *serial, char *buffer, int len)
{
    // ����豸ʵ��
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

    // ��ȡ����
    while (readLen--)
    {
        *buffer = Fifo_Pop(&serial->rxFifo);
        buffer++;
    }

    // ���ض�д����
    return len;
}

/****
 * ˵����  Serial_Write
 *         д��ӿ�
 * ������  devp  : deviceʵ��
 * ������  buffer: װ�����õ�buffer
 * ������  len   : ���ݳ���
 * ������  *ppos : д��ƫ������ָ�����ͣ������Ҫ�Լӿ��ں������ۼӸñ���
 * ���أ� ����0�ɹ���������ʵ��д�볤�ȣ� ����ֵ��ʧ��
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

    // д������
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
    // ���ض�д����
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
