#ifndef _USART_H_
#define _USART_H_

#include "stdint.h"
#include "string.h"
#include "stm32f30x.h"
#include "main.h"
#include "adc.h"
#include "config.h"

#define SlaveAddress  1

#define READSWITCHREG      0x01
#define READDATAREG        0x03

#define SETSWITCHREG       0x05
#define SETMULTSWITCHREG   0x0F

#define SETDATAREG         0x06
#define SETMULTDATAREG     0x10

#define ERRORFUN           0x01
#define ERRORADDRESS       0x02
#define ERRORDATA          0x03

#define UART_RX_BUF_LEN    100

#define UART_TX_BUF_FRAM_NUM    20



//�̶�֡ͷ
#define  FRAME_HEAD_IDX          0  
#define  FRAME_XY_VERSION_IDX    2  //Э��汾
#define  FRAME_SORUCE_ADRESS_IDX 5  //Դ��ַ
#define  FRAME_TARGET_ADRESS_IDX 9  //Ŀ�ĵ�ַ
#define  FRAME_DEV_TYPE_IDX      13 //�豸����
#define  FRAME_DEV_VERSION_IDX   16 //�豸�汾
#define  FRAME_NUM_IDX           21 //֡���
#define  FRAME_ORDER_IDX         25 //��������
#define  FRAME_TYPE_IDX          23 //֡����
#define  FRAME_DATALEN_IDX       29 //���ݳ���
#define  FRAME_DATA_START_IDX    31  
#define  FRAME_LEN_MIN           35 //֡��С���ȣ�֡�̶��ֶ���ռ���ȣ�
#define  FRAME_LEN_MAX           1068 //֡��󳤶�
typedef struct ST_UartSendInfo
{
    uint8_t bufDate[UART_TX_BUF_FRAM_NUM][100]; // ��������
    uint8_t buflen[UART_TX_BUF_FRAM_NUM];
    uint8_t all_Count;			 // ��������
    uint8_t now_Count;			 // ��ǰ����
	uint8_t SendIndex[20];

}USART_TX_STRUCT;

typedef struct _uart_rx_st
{
    uint8_t data[UART_RX_BUF_LEN]; // ��������
    uint8_t now_Count;			 
}UART_RX_ST;


extern uint8_t FuncCode;
extern MoudBusType StartAddress;
extern MoudBusType RegNum;
extern uint8_t ByteNum;
extern MoudBusType SetData;
extern MoudBusType SetMultData[100];
extern uint8_t SendCount;
extern uint8_t SendIndex;
extern FloatType _R;
extern FloatType _V;


extern UART_RX_ST uart_rx_st;
extern UART_RX_ST uart1_rx_st;
extern USART_TX_STRUCT uart_tx_struct;
extern USART_TX_STRUCT uart1_tx_struct;

extern uint16_t usRegInputStart;

void Usart1SendVR(void);
uint16_t usMBCRC16( unsigned char * pucFrame, uint16_t usLen );
uint16_t getcrc16(const void *s, int n);
void ModBusDeal(uint8_t *UsartRecBuff,uint16_t len);
void ModBusDeal_uart1(uint8_t *UsartRecBuff,uint16_t len);
void modbus_send(void);
void uart1_data_send(void);
void ota_rce_deal(uint8_t *data,uint16_t len);
void inference_module_id_set(void);
void device_set_ack_re(uint8_t *data,uint16_t len);
#endif
