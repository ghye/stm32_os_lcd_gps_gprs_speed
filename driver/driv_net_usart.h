#ifndef __DRIV_NET_USART_H__
#define __DRIV_NET_USART_H__

void driv_net_usart_init(void);
void driv_net_usart_tx_dma_init(void);
int driv_net_usart_tx_dma_write_data(unsigned char *buf, int len);
void driv_net_usart_tx_dma_reload(void);
void driv_net_usart_tx_dma_reload_normal(void);
void driv_net_usart_tx_dma_enable(void);
void driv_net_usart_tx_dma_disable(void);

#endif
