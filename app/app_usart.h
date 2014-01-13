#ifndef __APP_USART_H__
#define __APP_USART_H__

#include <stdint.h>
#include <stdbool.h>

#define USART_GPRS_NUM		2
#define USART_GPS_NUM		5
#define USART_SPEED_NUM	1

void app_usart_init(void);
void app_usart2_send(uint8_t id, uint8_t val);
void app_usart2_send_msg(void *msg);

#endif

