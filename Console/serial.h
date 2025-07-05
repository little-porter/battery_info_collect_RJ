

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "fifo.h"

typedef struct __serial_dev
{
    int baudrate;
    uint32_t usart_periph;
    fifo_t txFifo;
    fifo_t rxFifo;
    void (*setTX)();
    void (*setRX)();
} serial_dev_t;

#define SERIAL1_TX_BUFFER_LEN 1024
#define SERIAL1_RX_BUFFER_LEN 1024

extern serial_dev_t g_SerialDev1; // UART Êý¾Ý

int Serial_Init(void);
int Serial_Read(serial_dev_t *self, char *buffer, int len);
int Serial_Write(serial_dev_t *self, char *buffer, int len);

void Serial_ReadDataToBuffer(unsigned char ch);
void Serial_WriteDataToUartInst(void);


#endif
