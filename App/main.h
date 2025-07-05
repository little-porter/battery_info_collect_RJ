#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f30x.h"

/* ----------------------- Defines ------------------------------------------*/
//输入寄存器起始地址
#define REG_INPUT_START       0x0100
//输入寄存器数量
#define REG_INPUT_NREGS       18
//保持寄存器起始地址
#define REG_HOLDING_START     0x0100
//保持寄存器数量
#define REG_HOLDING_NREGS     8

//线圈起始地址
#define REG_COILS_START       0x0001
//线圈数量
#define REG_COILS_SIZE        16

//开关寄存器起始地址
#define REG_DISCRETE_START    0x0001
//开关寄存器数量
#define REG_DISCRETE_SIZE     16

/* Private variables ---------------------------------------------------------*/
//输入寄存器内容
extern  uint16_t usRegInputBuf[REG_INPUT_NREGS];//读04
//输入寄存器起始地址
extern uint16_t usRegInputStart;

//保持寄存器内容
extern uint16_t usRegHoldingBuf[REG_HOLDING_NREGS];//读03，写06，16
//保持寄存器起始地址
extern uint16_t usRegHoldingStart;

//线圈状态
extern uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8];//读01，写05，15
//开关输入状态
extern uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8];//02

//标定用
extern uint16_t BDBuf[REG_INPUT_NREGS];//读04,H2, CO, SMOKE, R , V , TEMP , Humidity , Pa      //数值寄存器区

double movingAverageFilter(double newData, double *buff, int *index, double *sm,uint8_t *buffman)  ;

#endif
