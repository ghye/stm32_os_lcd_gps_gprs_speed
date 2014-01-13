#ifndef __DRIV_USART_H__
#define __DRIV_USART_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct{
	uint8_t port;		/*1->uart1*/
	uint8_t parity;	/*0:NO, 1:ODD, 2:EVEN*/
	bool data_9bit;	/*0:8bit, 1:9bit*/	
	bool twoStopBits;	/*0:0nestopbits, 1:twostopbits*/	
	uint32_t baudrate;	/*115200*/
}com_para_t;

void com_init(com_para_t *para);
void com_send_char(uint8_t uart_num, uint8_t data );
void com_send_nchar(uint8_t uart_num, void *buf, uint32_t len );
void com_send_string(uint8_t uart_num, uint8_t *msg);
void com_send_hex(uint8_t uart_num, uint8_t data);
void com_send_message(uint8_t uart_num, uint8_t *msg);

#endif

