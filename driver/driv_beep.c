#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "driv_beep.h"

#define DRIV_BEEP_PORT	GPIOB
#define DRIV_BEEP_PIN	GPIO_Pin_0

void driv_beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = DRIV_BEEP_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DRIV_BEEP_PORT, &GPIO_InitStructure);
}

void driv_beep_open(void)
{
	GPIO_SetBits(DRIV_BEEP_PORT, DRIV_BEEP_PIN);
}

void driv_beep_close(void)
{
	GPIO_ResetBits(DRIV_BEEP_PORT, DRIV_BEEP_PIN);
}
