#include "public.h"
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#if defined(HC_CONTROLER_)

#define COIL1_PIN		GPIO_Pin_5	/* IO1 */
#define COIL2_PIN		GPIO_Pin_4	/* IO2 */
#define COIL3_PIN		GPIO_Pin_1	/* IO5 */
#define COIL4_PIN		GPIO_Pin_0	/* IO6 */
#define COIL1_PORT		GPIOC
#define COIL2_PORT		GPIOC
#define COIL3_PORT		GPIOC
#define COIL4_PORT		GPIOC

#define COIL1_EXTI_PORT		GPIO_PortSourceGPIOC
#define COIL1_EXTI_PIN		GPIO_PinSource5
#define COIL1_EXTI_LINE		EXTI_Line5
#define COIL1_EXTI_CHN		EXTI9_5_IRQn
#define COIL2_EXTI_PORT		GPIO_PortSourceGPIOC
#define COIL2_EXTI_PIN		GPIO_PinSource4
#define COIL2_EXTI_LINE		EXTI_Line4
#define COIL2_EXTI_CHN		EXTI4_IRQn
#define COIL3_EXTI_PORT		GPIO_PortSourceGPIOC
#define COIL3_EXTI_PIN		GPIO_PinSource1
#define COIL3_EXTI_LINE		EXTI_Line1
#define COIL3_EXTI_CHN		EXTI1_IRQn
#define COIL4_EXTI_PORT		GPIO_PortSourceGPIOC
#define COIL4_EXTI_PIN		GPIO_PinSource0
#define COIL4_EXTI_LINE		EXTI_Line0
#define COIL4_EXTI_CHN		EXTI0_IRQn

void driv_coil1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = COIL1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(COIL1_PORT, &GPIO_InitStructure);

	#if 1
	GPIO_EXTILineConfig(COIL1_EXTI_PORT, COIL1_EXTI_PIN);
	EXTI_InitStructure.EXTI_Line = COIL1_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = COIL1_EXTI_CHN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	#endif
}

void driv_coil2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = COIL2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(COIL2_PORT, &GPIO_InitStructure);

	#if 1
	GPIO_EXTILineConfig(COIL2_EXTI_PORT, COIL2_EXTI_PIN);
	EXTI_InitStructure.EXTI_Line = COIL2_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = COIL2_EXTI_CHN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	#endif
}

void driv_coil3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = COIL3_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(COIL3_PORT, &GPIO_InitStructure);

	#if 1
	GPIO_EXTILineConfig(COIL3_EXTI_PORT, COIL3_EXTI_PIN);
	EXTI_InitStructure.EXTI_Line = COIL3_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = COIL3_EXTI_CHN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	#endif
}

void driv_coil4_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = COIL4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(COIL4_PORT, &GPIO_InitStructure);

	#if 1
	GPIO_EXTILineConfig(COIL4_EXTI_PORT, COIL4_EXTI_PIN);
	EXTI_InitStructure.EXTI_Line = COIL4_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = COIL4_EXTI_CHN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	#endif
}

char driv_coil1_iolevel(void)
{
	char v;
	
	if (Bit_SET == GPIO_ReadInputDataBit(COIL1_PORT, COIL1_PIN))
		v = 1;
	else
		v = 0;

	return v;
}

char driv_coil2_iolevel(void)
{
	char v;
	
	if (Bit_SET == GPIO_ReadInputDataBit(COIL2_PORT, COIL2_PIN))
		v = 1;
	else
		v = 0;

	return v;
}

char driv_coil3_iolevel(void)
{
	char v;
	
	if (Bit_SET == GPIO_ReadInputDataBit(COIL3_PORT, COIL3_PIN))
		v = 1;
	else
		v = 0;

	return v;
}

char driv_coil4_iolevel(void)
{
	char v;
	
	if (Bit_SET == GPIO_ReadInputDataBit(COIL4_PORT, COIL4_PIN))
		v = 1;
	else
		v = 0;

	return v;
}

#endif
