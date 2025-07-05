
#ifndef __FIFO_H__
#define __FIFO_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "stdbool.h"

typedef struct _fifo_t
{
    volatile int writePos;
    volatile int readPos;
    volatile int bufferSize;
    uint8_t *buffer;
} fifo_t;

// 初始化fifo结构
int Fifo_Init(fifo_t *fifo, uint8_t *buffer, uint16_t size);

// 压入一个字节
void Fifo_Push(fifo_t *fifo, uint8_t data);

// 弹出一个字节
uint8_t Fifo_Pop(fifo_t *fifo);

// 刷掉所有数据
void Fifo_Flush(fifo_t *fifo);

// 判断是否空
uint8_t IsFifoEmpty(fifo_t *fifo);

// 判断是否满
uint8_t IsFifoFull(fifo_t *fifo);

// 获取剩余空间
uint16_t Fifo_GetFreeSize(fifo_t *fifo);
// 获取已存储数据量
uint16_t Fifo_GetUsedSize(fifo_t *fifo);

// 压入一个buffer数据
// int Fifo_PushBuffer(fifo_t* fifo,uint8_t* buffer,int len);

// 取出一个buffer数据
// int Fifo_PullBuffer(fifo_t* fifo,uint8_t* buffer,int len);

#endif
