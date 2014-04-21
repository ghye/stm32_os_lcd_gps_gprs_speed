#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"

#include "public.h"

void uart4_RCC_Configuration(void);
void uart4_GPIO_Configuration(void);
void uart4_NVIC_Configuration(void);
void mloop(void);

static unsigned int cnt = 0;
static unsigned char value = 0;

void main2(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	/* System Clocks Configuration */
	uart4_RCC_Configuration();

	/* NVIC configuration */
	uart4_NVIC_Configuration();

	/* Configure the GPIO ports */
	uart4_GPIO_Configuration();


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USARTy */
	USART_Init(UART4, &USART_InitStructure);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);

	while (1)
		mloop();
}

void uart4_isr(unsigned char v)
{
	value = v;
	cnt++;
}

void mloop(void)
{
	static char c = 0;
	char buf[32];

	sprintf(buf, "%d\r\n", cnt);
	com_send_nchar(4, buf, strlen(buf));
/*	if (c == 0) {
		GPIO_SetBits(GPIOC, GPIO_Pin_10);
		GPIO_SetBits(GPIOC, GPIO_Pin_11);
		c = 1;
	} else {
		GPIO_ResetBits(GPIOC, GPIO_Pin_10);
		GPIO_ResetBits(GPIOC, GPIO_Pin_11);
		c = 0;
	}
*/	
}

void uart4_RCC_Configuration(void)
{  
/*	RCC_HCLKConfig(RCC_SYSCLK_Div1); //配置AHB(HCLK)==系统时钟/1
	RCC_PCLK2Config(RCC_HCLK_Div1); //配置APB2(高速)(PCLK2)==系统时钟/1 
	RCC_PCLK1Config(RCC_HCLK_Div2); //配置APB1(低速)(PCLK1)==系统时钟/2*/

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void uart4_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure USARTy Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure USARTy Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void uart4_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef VECT_TAB_RAM
	// Set the Vector Tab base at location at 0x20000000
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else
	// Set the Vector Tab base at location at 0x80000000
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void uart4_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	USART_Cmd(UART4, DISABLE);
	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
	
	uart4_RCC_Configuration();
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	uart4_GPIO_Configuration();

#if defined(DouLunJi_CAR_TRUCK_1_3_140303_)
	USART_InitStructure.USART_BaudRate = 115200;
#elif defined(CAR_TRUCK_1_5_140325_)
	USART_InitStructure.USART_BaudRate = 9600;
#else
#error "UART4 error"
#endif
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USARTy */
	USART_Init(UART4, &USART_InitStructure);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);
}
