#include "print.h"

#include <stm32f10x_usart.h>

#define PRINT_USART UART5

int32_t fputc(int32_t ch, FILE *f)
{
	USART_SendData(PRINT_USART, (u8) ch);


	while(!(USART_GetFlagStatus(PRINT_USART, USART_FLAG_TXE) == SET))
	{
	}

	return ch;
}


int32_t fgetc(FILE *f)
{
	while(!(USART_GetFlagStatus(PRINT_USART, USART_FLAG_RXNE) == SET))
	{
	}

	return (USART_ReceiveData(PRINT_USART));
}
