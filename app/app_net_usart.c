#include <stdio.h>
#include <stdlib.h>
#include "app_usart.h"
#include "alg_loopback_buf.h"
#include "app_net_usart.h"
#include "driv_net_usart.h"

#if defined(HC_CONTROLER_)

#define NET_READ_BUF_LEN	1024

static int head;
static int tail;
static char net_read_buf[NET_READ_BUF_LEN];

static void app_net_usart_send_dma(unsigned char *buf, int len);

void app_net_usart_init(void)
{
	driv_net_usart_init();
	alg_lbb_init(&tail, &head);
}

void app_net_usart_proc(char *buf, int *len, int maxlen)
{
	alg_lbb_get_single(net_read_buf, NET_READ_BUF_LEN, tail, &head, buf, len, maxlen, '#');
}

void app_net_usart_isr(unsigned char v)
{
	alg_lbb_insert(net_read_buf, NET_READ_BUF_LEN, &tail, (char)v);
}

void app_net_usart_send(char *buf, int len)
{
	#if 1
	com_send_nchar(USART_NET_NUM, buf, len);
	#else
	app_net_usart_send_dma((void *)buf, len);
	#endif
}

static void app_net_usart_send_dma(unsigned char *buf, int len)
{
	driv_net_usart_tx_dma_set(buf, len);
	driv_net_usart_tx_dma_action();
}

#endif
