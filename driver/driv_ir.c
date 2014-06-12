#include "public.h"
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "driv_ir.h"

#if defined(HC_CONTROLER_)

#define IR1_PIN		GPIO_Pin_3	/* IO3 */
#define IR2_PIN		GPIO_Pin_2	/* IO4 */
#define IR1_PORT		GPIOC
#define IR2_PORT		GPIOC

void driv_ir1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IR1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IR1_PORT, &GPIO_InitStructure);
}

void driv_ir2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IR2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IR2_PORT, &GPIO_InitStructure);
}

char driv_ir1_iolevel(void)
{
	char v;
	
	if (Bit_SET == GPIO_ReadInputDataBit(IR1_PORT, IR1_PIN))
		v = 1;
	else
		v = 0;

	return v;
}

char driv_ir2_iolevel(void)
{
	char v;
	
	if (Bit_SET == GPIO_ReadInputDataBit(IR2_PORT, IR2_PIN))
		v = 1;
	else
		v = 0;

	return v;
}

#endif
