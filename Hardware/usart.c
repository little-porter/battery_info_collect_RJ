#include "usart.h"
#include "flash_bsp.h"


extern float R;
extern float V;
extern uint8_t cc_flag;


UART_RX_ST uart_rx_st;
UART_RX_ST uart1_rx_st;

uint8_t SendIndex;

MoudBusType SetData;
MoudBusType SetMultData[100];
FloatType _R;
FloatType _V;
USART_TX_STRUCT uart_tx_struct;
USART_TX_STRUCT uart1_tx_struct;
uint8_t inference_module_id_set_flag;

static const unsigned char aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const unsigned char aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

uint16_t usMBCRC16( unsigned char * pucFrame, uint16_t usLen )
{
    unsigned char           ucCRCHi = 0xFF;
    unsigned char           ucCRCLo = 0xFF;
    int             iIndex;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( unsigned char )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( uint16_t )( ucCRCHi << 8 | ucCRCLo );
}

uint16_t getcrc16(const void *s, int n)
{
		uint16_t c = 0xffff;
		for(int k=0; k<n; k++)
		{
			uint16_t b = (((uint8_t *)s)[k]);
			for(char i=0; i<8; i++)
			{
				c = ((b^c)&1) ? (c>>1)^0xA001 : (c>>1);
				b>>=1;
			}
		}
		return (c<<8)|(c>>8);
}

void modbus_dataAdd(uint8_t* buff,uint8_t count)
{
	uart_tx_struct.buflen[uart_tx_struct.all_Count] = count;
	uart_tx_struct.SendIndex[uart_tx_struct.all_Count] = 0;
	for(uint8_t i = 0; i < count; i++)
	{
		uart_tx_struct.bufDate[uart_tx_struct.all_Count][i] = buff[i];
	}
	
	uart_tx_struct.all_Count += 1;
	uart_tx_struct.all_Count %= 20;
}

void modbus_dataAdd_uart1(uint8_t* buff,uint8_t count)
{
	uart1_tx_struct.buflen[uart1_tx_struct.all_Count] = count;
	uart1_tx_struct.SendIndex[uart1_tx_struct.all_Count] = 0;
	for(uint8_t i = 0; i < count; i++)
	{
		uart1_tx_struct.bufDate[uart1_tx_struct.all_Count][i] = buff[i];
	}
	
	uart1_tx_struct.all_Count += 1;
	uart1_tx_struct.all_Count %= UART_TX_BUF_FRAM_NUM;
}

void modbus_send(void)
{
	USART_SendData(USART2, uart_tx_struct.bufDate[uart_tx_struct.now_Count][0]);
	uart_tx_struct.SendIndex[uart_tx_struct.now_Count]++;
	uart_tx_struct.buflen[uart_tx_struct.now_Count]--;
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
}

void modbus_send_uart1(void)
{
	USART_SendData(USART1, uart1_tx_struct.bufDate[uart1_tx_struct.now_Count][0]);
	uart1_tx_struct.SendIndex[uart1_tx_struct.now_Count]++;
	uart1_tx_struct.buflen[uart1_tx_struct.now_Count]--;
//	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}


void uart1_data_send(void)
{
	if(uart1_tx_struct.all_Count != uart1_tx_struct.now_Count)
	{
			
		uint8_t i = 0;	
		while(1)
		{
			
			if(USART_GetFlagStatus(USART1,USART_FLAG_TXE))
			{

				USART_SendData( USART1,uart1_tx_struct.bufDate[uart1_tx_struct.now_Count][i++]);
			}
			if(i >= uart1_tx_struct.buflen[uart1_tx_struct.now_Count])
			{
				break;
			}
		}
		
		uart1_tx_struct.now_Count++;
		uart1_tx_struct.now_Count%=UART_TX_BUF_FRAM_NUM;
	}

}


void inference_module_id_set(void)
{
	static uint8_t times =0;
	uint8_t buf[6]={0};
	
	times++;
	times%=20;
	
	if(times) return;
	if(!inference_module_id_set_flag) return;
	
	buf[0] = 0x22;
	memcpy(&buf[1],&BDBuf[4],4);
	buf[5] = 0xdd;
	modbus_dataAdd(buf,6);
}

void ModBusDeal(uint8_t *UsartRecBuff,uint16_t len)
{
	uint8_t      FuncCode;
	MoudBusType  StartAddress;
	MoudBusType  RegNum;
	uint8_t      ByteNum;
	uint16_t     crc16;
	uint8_t      AddressErrorFlag = 0,DataErrorFlag = 0;
	uint8_t      UsartSendBuff[100];
	MoudBusType  UsartSendBuffTmp[100];
	uint8_t      SendCount;
	
	
	FuncCode = UsartRecBuff[1];
	if(FuncCode == READSWITCHREG)
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
	}
	else if(FuncCode == READDATAREG)//03
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
		
					
		RegNum.buff[0] = UsartRecBuff[5];
		RegNum.buff[1] = UsartRecBuff[4];
		
		if(((REG_INPUT_START + REG_INPUT_NREGS) - StartAddress.data) < RegNum.data)
		{
			AddressErrorFlag = 1;
		}
		
		if(AddressErrorFlag == 0)
		{
			UsartSendBuff[0] = SlaveAddress;
			UsartSendBuff[1] = FuncCode;
			UsartSendBuff[2] = RegNum.data * 2;
			for(uint8_t i = 0; i < RegNum.data; i++)
			{
				UsartSendBuffTmp[i].buff[0] = usRegInputBuf[StartAddress.data - REG_INPUT_START + i] >> 8;    //high
				UsartSendBuffTmp[i].buff[1] = usRegInputBuf[StartAddress.data - REG_INPUT_START + i] & 0x00ff;//low
			}
			memcpy(&UsartSendBuff[3],&UsartSendBuffTmp[0].buff[0],RegNum.data * 2);
			
			crc16 = usMBCRC16(UsartSendBuff,3+(RegNum.data * 2));
			
			UsartSendBuff[3+(RegNum.data*2)] = crc16 >> 8;
			UsartSendBuff[4+(RegNum.data*2)] = crc16 & 0x00ff;
			
			SendCount = (RegNum.data*2) + 5;
			modbus_dataAdd(UsartSendBuff,SendCount);
		}
		
	}
	else if(FuncCode == SETSWITCHREG)
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
	}
	else if(FuncCode == SETMULTSWITCHREG)
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
	}
	else if(FuncCode == SETDATAREG)//06
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
		
		SetData.buff[0] = UsartRecBuff[5];
		SetData.buff[1] = UsartRecBuff[4];
		
		if((REG_HOLDING_START > StartAddress.data) || ((REG_HOLDING_START + REG_HOLDING_NREGS) < StartAddress.data))
		{
			AddressErrorFlag = 1;
		}
		
		if(SetData.data < 0)
		{
			DataErrorFlag = 1; 
		}
		if((DataErrorFlag == 0) && (DataErrorFlag == 0))
		{
			usRegHoldingBuf[StartAddress.data - REG_HOLDING_START] = SetData.data;
			memcpy(&usRegInputBuf[10],usRegHoldingBuf,8);
			SendCount = len;
			
			for(uint8_t i = 0;i < len; i++)
			{
				UsartSendBuff[i] = UsartRecBuff[i];
			}
			
			cc_flag = 1;
			modbus_dataAdd(UsartSendBuff,SendCount);
		}
	}
	else if(FuncCode == SETMULTDATAREG)//10
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
		
		RegNum.buff[0] = UsartRecBuff[5];
		RegNum.buff[1] = UsartRecBuff[4];
		
		if((REG_HOLDING_START > StartAddress.data) || ((REG_HOLDING_START + REG_HOLDING_NREGS) < StartAddress.data) || (((REG_HOLDING_START + REG_HOLDING_NREGS) - StartAddress.data) < RegNum.data))
		{
			AddressErrorFlag = 1;
		}
		
		ByteNum = UsartRecBuff[6];
		for(uint8_t i = 0; i < ByteNum / 2; i++)
		{
			SetMultData[i].buff[1] = UsartRecBuff[7+(i*2)];
			SetMultData[i].buff[0] = UsartRecBuff[8+(i*2)];
			if(SetMultData[i].data < 0)
			{
				DataErrorFlag = 1;
			}
		}
		
		if((DataErrorFlag == 0) && (DataErrorFlag == 0))
		{
			for(uint8_t i = 0; i < ByteNum / 2; i++)
			{
				usRegHoldingBuf[StartAddress.data - REG_HOLDING_START + i] = SetMultData[i].data;
			}
			
			memcpy(&usRegInputBuf[10],usRegHoldingBuf,16);
			
			UsartSendBuff[0] = SlaveAddress;
			UsartSendBuff[1] = FuncCode;
			UsartSendBuff[2] = StartAddress.buff[1];
			UsartSendBuff[3] = StartAddress.buff[0];
			
			for(uint8_t i = 0; i < ByteNum / 2; i++)
			{
				UsartSendBuff[(i*2)+4] = SetMultData[i].buff[1];
				UsartSendBuff[(i*2)+5] = SetMultData[i].buff[0];
			}
			
			crc16 = getcrc16(UsartSendBuff,3 + ByteNum);
			
			UsartSendBuff[4+ByteNum] = crc16 >> 8;
			UsartSendBuff[5+ByteNum] = crc16 & 0x00ff;
			
			SendCount = 6+ByteNum;
			cc_flag = 1;
			modbus_dataAdd(UsartSendBuff,SendCount);
		}
	}
	else
	{
		UsartSendBuff[0] = SlaveAddress;
		UsartSendBuff[1] = 0x82;
		UsartSendBuff[2] = 0x01;
		
		crc16 = getcrc16(UsartSendBuff,3);
		
		UsartSendBuff[3] = crc16 >> 8;
		UsartSendBuff[4] = crc16 & 0x00ff;
		
		SendCount = 5;
		modbus_dataAdd(UsartSendBuff,SendCount);
	}
	
	
	if(DataErrorFlag == 1)
	{
		UsartSendBuff[0] = SlaveAddress;
		UsartSendBuff[1] = 0x82;
		UsartSendBuff[2] = ERRORDATA;
		
		crc16 = getcrc16(UsartSendBuff,3);
		
		UsartSendBuff[3] = crc16 >> 8;
		UsartSendBuff[4] = crc16 & 0x00ff;
		
		SendCount = 5;
		modbus_dataAdd(UsartSendBuff,SendCount);
	}
	if(AddressErrorFlag == 1)
	{
		UsartSendBuff[0] = SlaveAddress;
		UsartSendBuff[1] = 0x82;
		UsartSendBuff[2] = ERRORADDRESS;
		
		crc16 = getcrc16(UsartSendBuff,3);
		
		UsartSendBuff[3] = crc16 >> 8;
		UsartSendBuff[4] = crc16 & 0x00ff;
		
		SendCount = 5;
		modbus_dataAdd(UsartSendBuff,SendCount);
	}	
	
	modbus_send();
}



void ModBusDeal_uart1(uint8_t *UsartRecBuff,uint16_t len)
{
	uint8_t      FuncCode;
	MoudBusType  StartAddress;
	MoudBusType  RegNum;
	uint8_t      ByteNum;
	uint16_t     crc16;
	uint8_t      AddressErrorFlag = 0,DataErrorFlag = 0;
	uint8_t      UsartSendBuff[100];
	MoudBusType  UsartSendBuffTmp[100];
	uint8_t      SendCount;
	
	
	FuncCode = UsartRecBuff[1];
	if(FuncCode == READSWITCHREG)
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
	}
	else if(FuncCode == READDATAREG)//03
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
		
					
		RegNum.buff[0] = UsartRecBuff[5];
		RegNum.buff[1] = UsartRecBuff[4];
		
		if(((REG_INPUT_START + REG_INPUT_NREGS) - StartAddress.data) < RegNum.data)
		{
			AddressErrorFlag = 1;
		}
		
		if(AddressErrorFlag == 0)
		{
			UsartSendBuff[0] = SlaveAddress;
			UsartSendBuff[1] = FuncCode;
			UsartSendBuff[2] = RegNum.data * 2;
			for(uint8_t i = 0; i < RegNum.data; i++)
			{
				UsartSendBuffTmp[i].buff[0] = BDBuf[StartAddress.data - REG_INPUT_START + i] >> 8;    //high
				UsartSendBuffTmp[i].buff[1] = BDBuf[StartAddress.data - REG_INPUT_START + i] & 0x00ff;//low
			}
			memcpy(&UsartSendBuff[3],&UsartSendBuffTmp[0].buff[0],RegNum.data * 2);
			
			crc16 = usMBCRC16(UsartSendBuff,3+(RegNum.data * 2));
			
			UsartSendBuff[4+(RegNum.data*2)] = crc16 >> 8;
			UsartSendBuff[3+(RegNum.data*2)] = crc16 & 0x00ff;
			
			SendCount = (RegNum.data*2) + 5;
			modbus_dataAdd_uart1(UsartSendBuff,SendCount);
		}
		
	}
	else if(FuncCode == SETSWITCHREG)
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
	}
	else if(FuncCode == SETMULTSWITCHREG)
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
	}
	else if(FuncCode == SETDATAREG)//06
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
		
		SetData.buff[0] = UsartRecBuff[5];
		SetData.buff[1] = UsartRecBuff[4];
		
		if((REG_HOLDING_START > StartAddress.data) || ((REG_HOLDING_START + REG_HOLDING_NREGS) < StartAddress.data))
		{
			AddressErrorFlag = 1;
		}
		
		if(SetData.data < 0)
		{
			DataErrorFlag = 1; 
		}
		if((DataErrorFlag == 0) && (DataErrorFlag == 0))
		{
			usRegHoldingBuf[StartAddress.data - REG_HOLDING_START] = SetData.data;
			memcpy(&usRegInputBuf[10],usRegHoldingBuf,8);
			SendCount = len;
			
			for(uint8_t i = 0;i < len; i++)
			{
				UsartSendBuff[i] = UsartRecBuff[i];
			}
			
			cc_flag = 1;
			modbus_dataAdd_uart1(UsartSendBuff,SendCount);
		}
	}
	else if(FuncCode == SETMULTDATAREG)//10
	{
		StartAddress.buff[0] = UsartRecBuff[3];
		StartAddress.buff[1] = UsartRecBuff[2];
		
		RegNum.buff[0] = UsartRecBuff[5];
		RegNum.buff[1] = UsartRecBuff[4];
		
		if((REG_HOLDING_START > StartAddress.data) || ((REG_HOLDING_START + REG_HOLDING_NREGS) < StartAddress.data) || (((REG_HOLDING_START + REG_HOLDING_NREGS) - StartAddress.data) < RegNum.data))
		{
			AddressErrorFlag = 1;
		}
		
		ByteNum = UsartRecBuff[6];
		for(uint8_t i = 0; i < ByteNum / 2; i++)
		{
			SetMultData[i].buff[1] = UsartRecBuff[7+(i*2)];
			SetMultData[i].buff[0] = UsartRecBuff[8+(i*2)];
			if(SetMultData[i].data < 0)
			{
				DataErrorFlag = 1;
			}
		}
		
		if((DataErrorFlag == 0) && (DataErrorFlag == 0))
		{
			for(uint8_t i = 0; i < ByteNum / 2; i++)
			{
				BDBuf[StartAddress.data - REG_HOLDING_START + i] = SetMultData[i].data;
			}
			
//			memcpy(&usRegInputBuf[0],usRegHoldingBuf,sizeof(usRegHoldingBuf));
			
			UsartSendBuff[0] = SlaveAddress;
			UsartSendBuff[1] = FuncCode;
			UsartSendBuff[2] = StartAddress.buff[1];
			UsartSendBuff[3] = StartAddress.buff[0];
			UsartSendBuff[4] = UsartRecBuff[4];
			UsartSendBuff[5] = UsartRecBuff[5];
//			for(uint8_t i = 0; i < ByteNum / 2; i++)
//			{
//				UsartSendBuff[(i*2)+4] = SetMultData[i].buff[1];
//				UsartSendBuff[(i*2)+5] = SetMultData[i].buff[0];
//			}
//			
			crc16 = getcrc16(UsartSendBuff,6);
			
			UsartSendBuff[6] = crc16 >> 8;
			UsartSendBuff[7] = crc16 & 0x00ff;
			
			SendCount = 8;
//			cc_flag = 1;
			modbus_dataAdd_uart1(UsartSendBuff,SendCount);
			inference_module_id_set_flag = 1;
		}
	}
	else
	{
		UsartSendBuff[0] = SlaveAddress;
		UsartSendBuff[1] = 0x82;
		UsartSendBuff[2] = 0x01;
		
		crc16 = getcrc16(UsartSendBuff,3);
		
		UsartSendBuff[3] = crc16 >> 8;
		UsartSendBuff[4] = crc16 & 0x00ff;
		
		SendCount = 5;
		modbus_dataAdd_uart1(UsartSendBuff,SendCount);
	}
	
	
	
	if(DataErrorFlag == 1)
	{
		UsartSendBuff[0] = SlaveAddress;
		UsartSendBuff[1] = 0x82;
		UsartSendBuff[2] = ERRORDATA;
		
		crc16 = getcrc16(UsartSendBuff,3);
		
		UsartSendBuff[3] = crc16 >> 8;
		UsartSendBuff[4] = crc16 & 0x00ff;
		
		SendCount = 5;
		modbus_dataAdd_uart1(UsartSendBuff,SendCount);
	}
	if(AddressErrorFlag == 1)
	{
		UsartSendBuff[0] = SlaveAddress;
		UsartSendBuff[1] = 0x82;
		UsartSendBuff[2] = ERRORADDRESS;
		
		crc16 = getcrc16(UsartSendBuff,3);
		
		UsartSendBuff[3] = crc16 >> 8;
		UsartSendBuff[4] = crc16 & 0x00ff;
		
		SendCount = 5;
		modbus_dataAdd_uart1(UsartSendBuff,SendCount);
	}	
	
//	modbus_send_uart1();
}



void ota_rce_deal(uint8_t *data,uint16_t len)
{
	
	if((data[0] != 0xaa) || (data[1] != 0xaa) || (data[len-1] != 0x55) || (data[len-2] != 0x55) || (len!= 43)) return;
	uint8_t bagFlag = data[FRAME_DATA_START_IDX + 5];
	if(bagFlag != 0x01) return;
		
	bootloader_st.ota_startFlag = 1;
	memcpy(bootloader_st.order,data,len);
	
	otaData_save();
	//todo÷ÿ∆Ù
	sys_restart();
}
void device_set_ack_re(uint8_t *data,uint16_t len)
{

    if((len == 6) && (data[0] == 0x22) && (data[len - 1] == 0xdd))
    {
       inference_module_id_set_flag=0;
    }
}
