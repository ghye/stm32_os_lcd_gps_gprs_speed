#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#include "core.h"
#include "driv_usart.h"
#include "app_usart.h"
#include "driv_systick.h"
#include "driv_gprs.h"

#if defined (CAR_DB44_V1_0_20130315_)

#define PORT_RST_ON GPIOD
#define M_RST GPIO_Pin_4
#define M_ON GPIO_Pin_5
#define MG323_RESET() 	GPIO_SetBits(PORT_RST_ON, M_RST) /* 硬件复位>= 10ms */
#define MG323_NORM() GPIO_ResetBits(PORT_RST_ON, M_RST)
#define MG323_POWERSWITCH() GPIO_SetBits(PORT_RST_ON, M_ON)	/*开/关切换>=1s*/
#define MG323_POWERNORM() GPIO_ResetBits(PORT_RST_ON, M_ON)

#define PORT_MG323_PWR GPIOD
#define MG323_PWR_PIN GPIO_Pin_3
#define MG323_POWER_ON() GPIO_SetBits(PORT_MG323_PWR, MG323_PWR_PIN)
#define MG323_POWER_OFF() GPIO_ResetBits(PORT_MG323_PWR, MG323_PWR_PIN)

#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

#define PORT_RST_ON		GPIOB
#define M_RST			GPIO_Pin_7
#define M_ON			GPIO_Pin_9
#define MG323_RESET() 	GPIO_SetBits(PORT_RST_ON, M_RST) /* 硬件复位>= 10ms */
#define MG323_NORM() 	GPIO_ResetBits(PORT_RST_ON, M_RST)
#define MG323_POWERSWITCH() 	GPIO_SetBits(PORT_RST_ON, M_ON)	/*开/关切换>=1s*/
#define MG323_POWERNORM() 		GPIO_ResetBits(PORT_RST_ON, M_ON)

#define PORT_MG323_PWR	GPIOB
#define MG323_PWR_PIN		GPIO_Pin_6
#define MG323_POWER_ON()	GPIO_SetBits(PORT_MG323_PWR, MG323_PWR_PIN)
#define MG323_POWER_OFF()	GPIO_ResetBits(PORT_MG323_PWR, MG323_PWR_PIN)

#endif

void driv_gprs_rx_it_disable(void)
{
	#if defined (CAR_DB44_V1_0_20130315_)
	
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

	#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	
	#endif
}

void driv_gprs_rx_it_enable(void)
{
	#if defined (CAR_DB44_V1_0_20130315_)
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	#endif
}

void driv_gprs_send_msg(void *msg, uint32_t len)
{
	com_send_nchar(USART_GPRS_NUM, msg, len);
}

void driv_gprs_power_enable(void)
{
	MG323_POWER_ON();
}

void driv_gprs_power_disable(void)
{
	MG323_POWER_OFF();
}

void driv_gprs_reset(void)
{
	MG323_RESET();
}

void driv_gprs_norm(void)
{
	MG323_NORM();
}

void driv_gprs_init(void)
{
	uint64_t i;
	GPIO_InitTypeDef GPIO_InitStructure;

	#if defined (CAR_DB44_V1_0_20130315_)
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = M_RST|M_ON;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(PORT_RST_ON, &GPIO_InitStructure);
	MG323_POWERSWITCH();
	MG323_NORM();

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MG323_PWR_PIN;
	GPIO_Init(PORT_MG323_PWR, &GPIO_InitStructure);
	
	driv_gprs_power_disable();
	i = ticks;
	i += 2000/TICK_MS;
	while (i > ticks) ;
	driv_gprs_power_enable();

	#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = M_RST|M_ON;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PORT_RST_ON, &GPIO_InitStructure);
	MG323_POWERSWITCH();
	MG323_NORM();

	GPIO_InitStructure.GPIO_Pin = MG323_PWR_PIN;
	GPIO_Init(PORT_MG323_PWR, &GPIO_InitStructure);
	
	driv_gprs_power_enable();
	
	#endif
}

