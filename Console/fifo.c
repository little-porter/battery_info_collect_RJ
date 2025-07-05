

#include "fifo.h"

#define Fifo_Next(xfifo, index) ((index + 1) >= xfifo->bufferSize ? 0 : index + 1)

int Fifo_Init(fifo_t *fifo, uint8_t *buffer, uint16_t size)
{
    fifo->writePos = 0;
    fifo->readPos = 0;
    fifo->buffer = buffer;
    fifo->bufferSize = size;
    return 0;
}

/**
 * д��һ���ֽڵ�fifo��
 * �� writePos != readPos��writePosʼ��ָ������д������ݣ���writePos��ǰָ�����������Ч��
 * �ú������жϻ�������ǰ�Ƿ��д�����øú���ǰ��Ӧ�ó���Ӧ���ȼ�黺�����Ƿ���д��
 * @return none
 */
void Fifo_Push(fifo_t *fifo, uint8_t data)
{
    fifo->buffer[Fifo_Next(fifo, fifo->writePos)] = data;
    fifo->writePos = Fifo_Next(fifo, fifo->writePos); // ָ�����µ�����
}

/**
 * ��fifo��ȡһ���ֽ�readPos����һ��λ��
 * readPos��ǰָ���λ��ʼ������Ч���ݣ��ɶ�ȡ�ĵ�һ��������readPos����һ��λ��
 * �ú������жϻ�������ǰ�Ƿ�ɶ������øú���ǰ��Ӧ�ó���Ӧ���ȼ�黺�����Ƿ�Ϊ��
 * @return ��ȡ������
 */
uint8_t Fifo_Pop(fifo_t *fifo)
{
    uint8_t data = fifo->buffer[Fifo_Next(fifo, fifo->readPos)]; // �ɶ�ȡ�ĵ�һ�����ݣ��Ƕ�ȡָ�����һ��λ��
    fifo->readPos = Fifo_Next(fifo, fifo->readPos);
    return data;
}

void Fifo_Flush(fifo_t *fifo)
{
    fifo->readPos = 0;
    fifo->writePos = 0;
}

uint8_t IsFifoEmpty(fifo_t *fifo)
{
    return (fifo->readPos == fifo->writePos);
}

uint8_t IsFifoFull(fifo_t *fifo)
{
    return (Fifo_Next(fifo, fifo->writePos) == fifo->readPos);
}

uint16_t Fifo_GetFreeSize(fifo_t *fifo)
{

    int FreeSize; // = fifo->bufferSize - 1;  //���ÿռ�Ȼ���������С1
    // ����Ҫ�ȴ������ڼ��㣬����ֱ�������ڼ���Ĺ����б��ж��޸ĵ����жϴ���
    int readPos = fifo->readPos;
    int writePos = fifo->writePos;
    if (writePos > readPos)
    {
        // FreeSize -= (writePos - readPos);
        FreeSize = fifo->bufferSize - 1 - (writePos - readPos);
    }
    else if (writePos < readPos)
    {
        FreeSize = readPos - writePos - 1;
    }
    else // writePos == readPos ��������
    {
        FreeSize = fifo->bufferSize - 1;
    }

    return FreeSize;
}

uint16_t Fifo_GetUsedSize(fifo_t *fifo)
{
    int usedLen = 0;
    // ����Ҫ�ȴ������ڼ��㣬����ֱ�������ڼ���Ĺ����б��ж��޸ĵ����жϴ���
    int readPos = fifo->readPos;
    int writePos = fifo->writePos;

    if (writePos > readPos)
    {
        usedLen = writePos - readPos;
    }
    else if (writePos < readPos)
    {
        usedLen = (fifo->bufferSize - readPos) + (writePos);
    }
    else // writePos == readPos
    {
        // usedLen = 0;
    }

    return usedLen;
}

int Fifo_PushBuffer(fifo_t *fifo, uint8_t *buffer, int len)
{
    int cpyLen = 0;
    int freeLen = Fifo_GetFreeSize(fifo);
    int writePos = fifo->writePos;

    if (freeLen == 0 || len <= 0)
    {
        return 0;
    }
    if (len > freeLen)
    {
        len = freeLen;
    }

    writePos = Fifo_Next(fifo, writePos);
    cpyLen = len;
    if (len > ((fifo->bufferSize) - (writePos)))
    { // ��Ҫ������д��
        cpyLen = ((fifo->bufferSize) - writePos);
        memcpy((uint8_t *)&fifo->buffer[writePos], buffer, cpyLen);
        buffer += cpyLen;
        cpyLen = len - cpyLen;
        fifo->writePos = fifo->bufferSize - 1; // ָ�����µ�����λ��
        writePos = 0;                          // ָ����һ����д��λ��
    }

    if (cpyLen != 0)
    {
        memcpy((uint8_t *)&fifo->buffer[writePos], buffer, cpyLen);
        fifo->writePos = (fifo->writePos + cpyLen) % fifo->bufferSize;
    }

    return len;
}

int Fifo_PullBuffer(fifo_t *fifo, uint8_t *buffer, int len)
{
    int cpyLen;
    int validLen = Fifo_GetUsedSize(fifo);
    int readPos = fifo->readPos;

    if (validLen == 0 || len <= 0)
    {
        return 0;
    }
    if (len > validLen)
    {
        len = validLen;
    }

    readPos = Fifo_Next(fifo, readPos);
    cpyLen = len;
    if (len > ((fifo->bufferSize) - readPos))
    {
        cpyLen = ((fifo->bufferSize) - readPos);
        memcpy(buffer, (uint8_t *)&fifo->buffer[readPos], cpyLen);
        buffer += cpyLen;
        cpyLen = len - cpyLen;
        fifo->readPos = fifo->bufferSize - 1;
        readPos = 0;
    }

    if (cpyLen != 0)
    {
        memcpy(buffer, (uint8_t *)&fifo->buffer[readPos], cpyLen);
        fifo->readPos = (fifo->readPos + cpyLen) % fifo->bufferSize;
    }

    return len;
}
