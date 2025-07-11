#include "crc8.h"




#define CRC8_POLY     0x31   // CRC-8 ����ʽ
#define CRC8_INIT     0xFF   // ��ʼֵ

// ֱ�Ӽ��� CRC-8 ֵ
uint8_t calculate_crc8(const uint8_t *data, size_t length) 
{
    uint8_t crc = CRC8_INIT;

    for (size_t i = 0; i < length; ++i) 
	{
        crc ^= data[i];  						// ����ǰ�ֽ����� CRC

        for (int j = 0; j < 8; ++j) 
		{
            if (crc & 0x80) 					// ������λ�� 1
			{  
                crc = (crc << 1) ^ CRC8_POLY;
            } 
			else 
			{
                crc <<= 1;
            }
        }
    }

    return crc;
}








