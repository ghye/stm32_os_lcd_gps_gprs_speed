#include "public.h"
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "driv_usart.h"
#include "driv_weighbridge.h"

#if defined(HC_CONTROLER_)

#define RS485_WB_RX_E_PIN		GPIO_Pin_9
#define RS485_WB_RX_E_PORT	GPIOC

static void driv_weighbridge_gpio_init(void);

void driv_weighbridge_init(void)
{
	driv_weighbridge_gpio_init();
}

static void driv_weighbridge_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RS485_WB_RX_E_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_WB_RX_E_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(RS485_WB_RX_E_PORT, RS485_WB_RX_E_PIN);
}

#endif
