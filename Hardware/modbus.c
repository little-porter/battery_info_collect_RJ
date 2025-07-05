#include "modbus.h"

/*private 文件内部私有*/
#define POLYNOMIAL 0xA001   // Modbus CRC-16 polynomial (低字节优先)
uint16_t crcTable[256];     // CRC-16 table

#define  REG_TABLE_LEN    20
uint16_t config_reg_table[REG_TABLE_LEN];
uint16_t data_reg_table[REG_TABLE_LEN];
uint16_t result_reg_table[REG_TABLE_LEN];
uint16_t info_reg_table[REG_TABLE_LEN];

#define  MODBUS_ADDR_IDX        0
#define  MODBUS_FUNCODE_IDX     1
#define  REG_START_ADDR_IDX     2
#define  REG_NUM_IDX            4
#define  MODBUS_CRC_IDX         6

#define  MODBUS_READ_CONFIG_REG_CMD        0x03
#define  MODBUS_READ_DATA_REG_CMD          0x04
#define  MODBUS_WRITE_CONFIG_REG_CMD       0x10

/*public 文件外部接口*/
uint8_t modbus_addr = 1;

/*生成CRC-16表*/
void modbus_generate_crcTable(void) {
    uint16_t polynomial = POLYNOMIAL;
    for (int i = 0; i < 256; i++) 
	{
        uint16_t crc = 0;
        uint16_t c = (uint16_t)i;
        for (int j = 0; j < 8; j++) 
		{
            if (((crc ^ c) & 0x0001) != 0)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
            c >>= 1;
        }
        crcTable[i] = crc;
    }
}

/*计算CRC*/
uint16_t modbus_calculate_crc(uint8_t *data,uint16_t length)
{
    uint16_t crc = 0xFFFF;

    for(uint16_t i = 0; i < length; i++){
        uint16_t index = (crc ^ data[i]) & 0xFF;
        crc = (crc >> 8) ^ crcTable[index];
    }

    return crc;
}

void modbus_reg_write(uint16_t addr,uint16_t *data,uint16_t num)
{
    uint16_t *write_reg = NULL;
    if(addr<0x1000){
        write_reg = &config_reg_table[addr];
    }else if(addr<0x2000){
        write_reg = &data_reg_table[addr-0x1000];
    }else if(addr<0x3000){
        write_reg = &result_reg_table[addr-0x2000];
    }else if(addr<0x4000){
        write_reg = &info_reg_table[addr-0x3000];
    }else{
        /* 非法地址 */
        return;
    }
    memcpy(write_reg,data,num*2);    
}






void modbus_error_ask(uint8_t cmd,uint8_t error_code)
{
    uint8_t error_msg[256] = {0};
    uint16_t pos = 0,cal_crc = 0;
    error_msg[pos++] = modbus_addr;
    error_msg[pos++] = cmd+0x80;
    error_msg[pos++] = error_code;
    cal_crc = modbus_calculate_crc(error_msg,pos+1);
    error_msg[pos++] = cal_crc>>8&0xFF;
    error_msg[pos++] = cal_crc&0xFF;
    uart_data_send(error_msg,pos);
}

void modbus_read_ack(uint8_t cmd, uint16_t addr,uint16_t num)
{
    uint8_t ack_msg[256] = {0};
    uint16_t pos = 0,cal_crc = 0;
    uint16_t *ack_reg = NULL;
    if(num > REG_TABLE_LEN){
        /* 寄存器数量错误 */
        modbus_error_ask(cmd,0x03);
        return;
    }

    if(addr<0x1000){
        ack_reg = &config_reg_table[addr];
    }else if(addr<0x2000){
        ack_reg = &data_reg_table[addr-0x1000];
    }else if(addr<0x3000){
        ack_reg = &result_reg_table[addr-0x2000];
    }else if(addr<0x4000){
        ack_reg = &info_reg_table[addr-0x3000];
    }else{
        /* 非法地址 */
        modbus_error_ask(cmd,0x02);
        return;
    }
    ack_msg[pos++] = modbus_addr;
    ack_msg[pos++] = cmd;
    ack_msg[pos++] = num*2>>8;
    ack_msg[pos++] = num*2&0xFF;
    memcpy(&ack_msg[pos],ack_reg,num*2);
    pos += num*2;
    cal_crc = modbus_calculate_crc(ack_msg,pos);
    ack_msg[pos++] = cal_crc&0xFF;
    ack_msg[pos++] = cal_crc>>8&0xFF;
    
    uart_data_send(ack_msg,pos);
}

void modbus_write_ack(uint8_t cmd, uint16_t addr,uint16_t num,uint8_t *data)
{
    uint8_t ack_msg[256] = {0};
    uint16_t pos = 0,cal_crc = 0;
    uint16_t *ack_reg = NULL;

    if(num > REG_TABLE_LEN){
        /* 寄存器数量错误 */
        modbus_error_ask(cmd,0x03);
        return;
    }


    if(addr<0x1000){
        ack_reg = &config_reg_table[addr];
    }else{
        /* 非法地址 */
        modbus_error_ask(cmd,0x02);
        return;
    }
    memcpy(ack_reg,data,num*2);

    ack_msg[pos++] = modbus_addr;
    ack_msg[pos++] = cmd;
    ack_msg[pos++] = addr>>8;
    ack_msg[pos++] = addr&0xFF;
    ack_msg[pos++] = num>>8;
    ack_msg[pos++] = num&0xFF;
    cal_crc = modbus_calculate_crc(ack_msg,pos);
    ack_msg[pos++] = cal_crc&0xFF;
    ack_msg[pos++] = cal_crc>>8&0xFF;
   

    uart_data_send(ack_msg,pos);

}


void modbus_msg_deal_handler(uint8_t *data,uint16_t length)
{
    uint16_t crc=0,cal_crc=0;
    if(data[MODBUS_ADDR_IDX] != modbus_addr) return;
    crc = data[length-2] | data[length-1]<<8;
    cal_crc = modbus_calculate_crc(data,length-2);
    if(crc != cal_crc) return;
    uint8_t cmd = data[MODBUS_FUNCODE_IDX];
    uint16_t addr = data[REG_START_ADDR_IDX]<<8 | data[REG_START_ADDR_IDX+1];
    uint16_t num = data[REG_NUM_IDX]<<8 | data[REG_NUM_IDX+1];

    switch (cmd)
    {
    case MODBUS_READ_CONFIG_REG_CMD:
    case MODBUS_READ_DATA_REG_CMD: 
        modbus_read_ack(cmd,addr,num);
        break;
    case MODBUS_WRITE_CONFIG_REG_CMD:
        modbus_write_ack(cmd,addr,num,data);
        break;
    default:
        /*功能码错误*/
        modbus_error_ask(cmd,0x01);
        break;
    }
}






