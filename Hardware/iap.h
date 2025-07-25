#ifndef __IAP_H__
#define __IAP_H__


#include "stm32f30x.h"
#include "flash.h"
#include "system.h"


void iap_jump_app(void);

void iap_init(void);
void iap_task(void);
void iap_write_data(uint8_t *pdata,uint16_t num,uint16_t timeout);


void iap_flag_idle(void);
void iap_flag_start(void);
void iap_flag_finish(void);

void iap_crc_set(uint16_t crc);
void iap_msg_deal_handler(uint8_t *data,uint16_t length);


#endif

