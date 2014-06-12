
//#include "stm32f10x_lib.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h" //"stm32f10x_nvic.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "driv_usart.h"
#include "driv_usart.h"

void uart4_init(USART_InitTypeDef *USART_InitStructure);

static void usart1_init(USART_InitTypeDef *USART_InitStructure)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	USART_Cmd(USART1, DISABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	// Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		

	// USART configuration
	USART_Init(USART1, USART_InitStructure);	

			
	// Enable USART1 Receive and Transmit interrupts
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

	// Enable USART
	USART_Cmd(USART1, ENABLE);
}

static void usart2_init(USART_InitTypeDef *USART_InitStructure)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

	// Enable UART clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		

	// USART configuration
	USART_Init(USART2, USART_InitStructure);		

	// Enable USART2 Receive and Transmit interrupts
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

	// Enable USART
	USART_Cmd(USART2, ENABLE);	
}

static void usart3_init(USART_InitTypeDef *USART_InitStructure)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	
	// Enable UART clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	// Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		

	// USART configuration
	USART_Init(USART3, USART_InitStructure);		

	// Enable USART3 Receive and Transmit interrupts
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART3, USART_IT_TXE, ENABLE);


	// Enable USART
	USART_Cmd(USART3, ENABLE);		
}

void usart4_init(USART_InitTypeDef *USART_InitStructure)
{
	uart4_init(USART_InitStructure);
	
/*	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Enable UART clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

	// Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);		

	// USART configuration
	USART_Init(UART4, USART_InitStructure);		

	// Enable USART3 Receive and Transmit interrupts
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

	// Enable USART
	USART_Cmd(UART4, ENABLE);*/
}

void usart5_init(USART_InitTypeDef *USART_InitStructure)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Enable UART clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

	// Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Configure USART Rx as input floating
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);		

	// USART configuration
	USART_Init(UART5, USART_InitStructure);		

	// Enable USART3 Receive and Transmit interrupts
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART3, USART_IT_TXE, ENABLE);

	// Enable USART
	USART_Cmd(UART5, ENABLE);		
}

void com_init(com_para_t *para)
{
	//GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;	

	USART_InitStructure.USART_BaudRate = para->baudrate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	if(para->parity == 0)
		USART_InitStructure.USART_Parity = USART_Parity_No;
	else if(para->parity == 1)
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
	else if(para->parity == 2)
		USART_InitStructure.USART_Parity = USART_Parity_Even;

	if(para->twoStopBits == false)
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
	else if(para->twoStopBits == true)
		USART_InitStructure.USART_StopBits = USART_StopBits_2;
	
	if(para->data_9bit == false)
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	else if(para->data_9bit == true)
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	
	// Enable GPIO clock
	switch (para->port){
		case 1:
			usart1_init(&USART_InitStructure);
			NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //Channel;
			break;
	
		case 2:
			usart2_init(&USART_InitStructure);
			NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //Channel;
			break;

		case 3:
			usart3_init(&USART_InitStructure);
			NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //Channel;
			break;
		case 4:
			usart4_init(&USART_InitStructure);
			NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //Channel;
		case 5:
			usart5_init(&USART_InitStructure);
			NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; //Channel;
		default:
			break;		
	}

	// Enable the USARTx Interrupt
	//NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	/* �������жϾͲ���send_char???? */
	
}
void com_send_char(uint8_t uart_num, uint8_t data )
{
	switch (uart_num){
		case 1:
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){;}
			USART1->DR = data;	
			//USART_SendData(USART1, data&0xffff);
			break;
		case 2:
			while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){;}
			USART2->DR = data;
			break;
		case 3:
			while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET){;}
			USART3->DR = data;
			break;
		default:
			break;
	}
}

void com_send_nchar(uint8_t uart_num, void *buf, uint32_t len )
{
	uint32_t i;
	USART_TypeDef* USARTx;
	
	switch(uart_num){
		case 1:
			USARTx = USART1;
			break;
		case 2:
			USARTx = USART2;
			break;
		case 3:
			USARTx = USART3;			
			break;
		case 4:
			USARTx = UART4;			
			break;			
		case 5:
			USARTx = UART5;			
			break;
		default:
			return;
	}

	for(i=0; i<len; i++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET){;}
		USARTx->DR = (uint16_t) ((uint8_t *)buf)[i];	
	}
}

void com_send_string(uint8_t uart_num, uint8_t *msg)
{
	USART_TypeDef* USARTx;
	switch(uart_num){
		case 1:
			USARTx = USART1;
			break;
		case 2:
			USARTx = USART2;
			break;
		case 3:
			USARTx = USART3;			
			break;
		case 4:
			USARTx = UART4;			
			break;			
		case 5:
			USARTx = UART5;			
			break;
		default:
			return;
	}
	while(*msg != '\0'){
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET){;}
		USARTx->DR = *msg ++;
	}
}

static uint8_t hex2ascii[] = "0123456789ABCDEF";
void com_send_hex(uint8_t uart_num, uint8_t data)
{
	com_send_char(uart_num, '0');
	com_send_char(uart_num, 'x');
	com_send_char(uart_num, hex2ascii[data >> 4]);
	com_send_char(uart_num, hex2ascii[data & 0x0F]);
}

void com_send_message(uint8_t uart_num, uint8_t *msg)
{
	USART_TypeDef* USARTx;
	switch(uart_num){
		case 1:
			USARTx = USART1;
			break;
		case 2:
			USARTx = USART2;
			break;
		case 3:
			USARTx = USART3;			
			break;
		case 4:
			USARTx = UART4;			
			break;			
		case 5:
			USARTx = UART5;			
			break;
		default:
			return;
	}
	while(*msg != '\0'){
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET){;}
		USARTx->DR = *msg ++;
	}
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET){;}
	USARTx->DR = '\r';		
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET){;}
	USARTx->DR = '\n';		
}

