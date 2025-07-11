#include "crc8.h"




#define CRC8_POLY     0x31   // CRC-8 多项式
#define CRC8_INIT     0xFF   // 初始值

// 直接计算 CRC-8 值
uint8_t calculate_crc8(const uint8_t *data, size_t length) 
{
    uint8_t crc = CRC8_INIT;

    for (size_t i = 0; i < length; ++i) 
	{
        crc ^= data[i];  						// 将当前字节异或进 CRC

        for (int j = 0; j < 8; ++j) 
		{
            if (crc & 0x80) 					// 如果最高位是 1
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








