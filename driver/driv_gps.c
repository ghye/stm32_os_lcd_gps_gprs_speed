#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "core.h"
#include "driv_systick.h"

#define GPS_PWR_OFF()	GPIO_SetBits(GPIOE, GPIO_Pin_1)
#define GPS_PWR_ON()	GPIO_ResetBits(GPIOE, GPIO_Pin_1)

void driv_gps_power_reset(void)
{
	GPS_PWR_OFF();
	os_task_delayms(2000);
	GPS_PWR_ON();
}

void driv_gps_init(void)
{
	uint64_t i;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPS_PWR_OFF();
	i = ticks;
	i += 2000/TICK_MS;
	GPS_PWR_ON();
}





