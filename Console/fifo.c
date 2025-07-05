

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
 * 写入一个字节到fifo。
 * 当 writePos != readPos，writePos始终指向最新写入的数据，即writePos当前指向的数据是有效的
 * 该函数不判断缓冲区当前是否可写，调用该函数前，应用程序应该先检查缓冲区是否已写满
 * @return none
 */
void Fifo_Push(fifo_t *fifo, uint8_t data)
{
    fifo->buffer[Fifo_Next(fifo, fifo->writePos)] = data;
    fifo->writePos = Fifo_Next(fifo, fifo->writePos); // 指向最新的数据
}

/**
 * 从fifo读取一个字节readPos的下一个位置
 * readPos当前指向的位置始终是无效数据，可读取的第一个数据是readPos的下一个位置
 * 该函数不判断缓冲区当前是否可读，调用该函数前，应用程序应该先检查缓冲区是否为空
 * @return 读取的数据
 */
uint8_t Fifo_Pop(fifo_t *fifo)
{
    uint8_t data = fifo->buffer[Fifo_Next(fifo, fifo->readPos)]; // 可读取的第一个数据，是读取指针的下一个位置
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

    int FreeSize; // = fifo->bufferSize - 1;  //可用空间比缓冲区总量小1
    // 必须要先存下来在计算，避免直接引用在计算的过程中被中断修改导致判断错误
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
    else // writePos == readPos 缓冲区空
    {
        FreeSize = fifo->bufferSize - 1;
    }

    return FreeSize;
}

uint16_t Fifo_GetUsedSize(fifo_t *fifo)
{
    int usedLen = 0;
    // 必须要先存下来在计算，避免直接引用在计算的过程中被中断修改导致判断错误
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
    { // 需要分两段写入
        cpyLen = ((fifo->bufferSize) - writePos);
        memcpy((uint8_t *)&fifo->buffer[writePos], buffer, cpyLen);
        buffer += cpyLen;
        cpyLen = len - cpyLen;
        fifo->writePos = fifo->bufferSize - 1; // 指向最新的数据位置
        writePos = 0;                          // 指向下一个待写入位置
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
