#ifndef __IAP_H__
#define __IAP_H__


#include "stm32f30x.h"
#include "flash.h"
#include "system.h"

#include "modbus.h"

#define iap_crc_calculate(a,b)	modbus_calculate_crc(a,b)

void iap_init(void);
void iap_task(void);

void iap_msg_deal_handler(uint8_t *data,uint16_t length);


#endif

