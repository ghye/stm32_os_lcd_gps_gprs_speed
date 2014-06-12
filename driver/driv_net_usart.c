#include "public.h"
#include <stm32f10x.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "driv_net_usart.h"

#if defined(HC_CONTROLER_)

#define RS485_TX_E_PIN		GPIO_Pin_4
#define RS485_RX_E_PIN		GPIO_Pin_1
#define RS485_TX_E_PORT	GPIOA
#define RS485_RX_E_PORT	GPIOA

#define NET_TX_DMA_IRQ			DMA1_Channel7_IRQn
#define NET_TX_DMA_Channel		DMA1_Channel7	/*UART2_TX*/
#define NET_TX_BASE				0x40004404 		/*UART2_TX*/
#define NET_USART				USART2
#define NET_TX_DMA_FLAG		DMA1_FLAG_TC7

#define WILL_SET				0

static char isr_running;
static DMA_InitTypeDef net_txDMA_InitStructure;

void driv_net_usart_tx_dma_reload_isr(void);
void driv_net_usart_tx_dma_reload(void);
void driv_net_usart_tx_dma_enable(void);
void driv_net_usart_tx_dma_disable(void);
bool driv_net_usart_tx_dma_is_finished(void);

static void driv_net_usart_tx_dma_running_set(void);
static void driv_net_usart_tx_dma_running_reset(void);
static char driv_net_usart_tx_dma_running_is_set(void);

void driv_net_usart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RS485_TX_E_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_TX_E_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RS485_RX_E_PIN;
	GPIO_Init(RS485_RX_E_PORT, &GPIO_InitStructure);

	GPIO_SetBits(RS485_TX_E_PORT, RS485_TX_E_PIN);
	GPIO_ResetBits(RS485_RX_E_PORT, RS485_RX_E_PIN);
}

void driv_net_usart_tx_dma_set(unsigned char *buf, int len)
{

}

void driv_net_usart_tx_dma_action(void)
{

}

static void driv_net_usart_tx_dma_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = NET_TX_DMA_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//DMA_DeInit(AIS_RX_DMA_Channel);  
	DMA_Cmd(NET_TX_DMA_Channel, DISABLE);
	DMA_SetCurrDataCounter(NET_TX_DMA_Channel, WILL_SET);
	net_txDMA_InitStructure.DMA_PeripheralBaseAddr = NET_TX_BASE;
	net_txDMA_InitStructure.DMA_MemoryBaseAddr = WILL_SET;
	net_txDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	net_txDMA_InitStructure.DMA_BufferSize = WILL_SET;
	net_txDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	net_txDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	net_txDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	net_txDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	net_txDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	net_txDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	net_txDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(NET_TX_DMA_Channel, &net_txDMA_InitStructure);
	DMA_ITConfig(NET_TX_DMA_Channel, DMA_IT_TC, ENABLE);
	//USART_DMACmd(NET_USART, USART_DMAReq_Tx, ENABLE);
	//DMA_Cmd(NET_TX_DMA_Channel, ENABLE);

	driv_net_usart_tx_dma_running_reset();
}

void driv_net_usart_tx_dma_reload_isr(void)
{
	int len;
	unsigned char *p;
	
	//app_net_usart_tx_data_get(&p, &len);
	if (len <= 0) {
		driv_net_usart_tx_dma_disable();
		driv_net_usart_tx_dma_running_reset();
		return;
	}
	DMA_Cmd(NET_TX_DMA_Channel, DISABLE);
	DMA_SetCurrDataCounter(NET_TX_DMA_Channel, (uint16_t)len);
	net_txDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)p;
	net_txDMA_InitStructure.DMA_BufferSize = (uint16_t)len;
	DMA_Init(NET_TX_DMA_Channel, &net_txDMA_InitStructure);
	driv_net_usart_tx_dma_enable();
	driv_net_usart_tx_dma_running_set();
}

void driv_net_usart_tx_dma_reload(void)
{
	if (0 == driv_net_usart_tx_dma_running_is_set()) {
		driv_net_usart_tx_dma_reload_isr();
	}
}

void driv_net_usart_tx_dma_enable(void)
{
	USART_DMACmd(NET_USART, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(NET_TX_DMA_Channel, ENABLE);
}

void driv_net_usart_tx_dma_disable(void)
{
	USART_DMACmd(NET_USART, USART_DMAReq_Tx, DISABLE);
	DMA_Cmd(NET_TX_DMA_Channel, DISABLE);
}

bool driv_net_usart_tx_dma_is_finished(void)
{
	if(DMA_GetFlagStatus(NET_TX_DMA_FLAG) == SET)
	{
		return true;
	}	
	return false;
}

static void driv_net_usart_tx_dma_running_set(void)
{
	isr_running = 1;
}

static void driv_net_usart_tx_dma_running_reset(void)
{
	isr_running = 0;
}

static char driv_net_usart_tx_dma_running_is_set(void)
{
	return isr_running;
}

#endif
