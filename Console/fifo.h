
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

// ��ʼ��fifo�ṹ
int Fifo_Init(fifo_t *fifo, uint8_t *buffer, uint16_t size);

// ѹ��һ���ֽ�
void Fifo_Push(fifo_t *fifo, uint8_t data);

// ����һ���ֽ�
uint8_t Fifo_Pop(fifo_t *fifo);

// ˢ����������
void Fifo_Flush(fifo_t *fifo);

// �ж��Ƿ��
uint8_t IsFifoEmpty(fifo_t *fifo);

// �ж��Ƿ���
uint8_t IsFifoFull(fifo_t *fifo);

// ��ȡʣ��ռ�
uint16_t Fifo_GetFreeSize(fifo_t *fifo);
// ��ȡ�Ѵ洢������
uint16_t Fifo_GetUsedSize(fifo_t *fifo);

// ѹ��һ��buffer����
// int Fifo_PushBuffer(fifo_t* fifo,uint8_t* buffer,int len);

// ȡ��һ��buffer����
// int Fifo_PullBuffer(fifo_t* fifo,uint8_t* buffer,int len);

#endif
