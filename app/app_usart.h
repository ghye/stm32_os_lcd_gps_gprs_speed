#ifndef __APP_USART_H__
#define __APP_USART_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

#if defined (CAR_DB44_V1_0_20130315_)

#define USART_GPRS_NUM		2
#define USART_GPS_NUM		5
#define USART_SPEED_NUM	1

#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

#define USART_GPRS_NUM		1
#define USART_GPS_NUM		2

#endif

void app_usart_init(void);
void app_usart2_send(uint8_t id, uint8_t val);
void app_usart2_send_msg(void *msg);

#endif

