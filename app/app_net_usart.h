#ifndef __APP_NET_USART_H__
#define __APP_NET_USART_H__

#include "public.h"

void app_net_usart_init(void);
void app_net_usart_proc(char *buf, int *len, int maxlen);
void app_net_usart_isr(unsigned char v);
void app_net_usart_send(char *buf, int len);

#endif
