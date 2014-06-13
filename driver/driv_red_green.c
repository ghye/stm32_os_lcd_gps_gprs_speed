#include "public.h"
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "driv_red_green.h"

#if defined(HC_CONTROLER_)

#define RED1_PIN	GPIO_Pin_7
#define RED1_PORT	GPIOA
#define GRN1_PIN	GPIO_Pin_8
#define GRN1_PORT	GPIOA
#define RED2_PIN	GPIO_Pin_5
#define RED2_PORT	GPIOA
#define GRN2_PIN	GPIO_Pin_6
#define GRN2_PORT	GPIOA

#define RG_OPEN(port, pin)	GPIO_SetBits(port, pin)
#define RG_CLOSE(port, pin)	GPIO_ResetBits(port, pin)

void driv_red_green1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RED1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(RED1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GRN1_PIN;
	GPIO_Init(GRN1_PORT, &GPIO_InitStructure);

	driv_red_green1_ropen();
}

void driv_red_green2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RED2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(RED2_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GRN2_PIN;
	GPIO_Init(GRN2_PORT, &GPIO_InitStructure);

	driv_red_green2_ropen();
}

void driv_red_green1_ropen(void)
{
	RG_OPEN(RED1_PORT, RED1_PIN);
	RG_CLOSE(GRN1_PORT, GRN1_PIN);
}

void driv_red_green1_rclose(void)
{
	RG_CLOSE(RED1_PORT, RED1_PIN);
}

void driv_red_green1_gopen(void)
{
	RG_OPEN(GRN1_PORT, GRN1_PIN);
	RG_CLOSE(RED1_PORT, RED1_PIN);
}

void driv_red_green1_gclose(void)
{
	RG_CLOSE(GRN1_PORT, GRN1_PIN);
}

void driv_red_green2_ropen(void)
{
	RG_OPEN(RED2_PORT, RED2_PIN);
	RG_CLOSE(GRN2_PORT, GRN2_PIN);
}

void driv_red_green2_rclose(void)
{
	RG_CLOSE(RED2_PORT, RED2_PIN);
}

void driv_red_green2_gopen(void)
{
	RG_OPEN(GRN2_PORT, GRN2_PIN);
	RG_CLOSE(RED2_PORT, RED2_PIN);
}

void driv_red_green2_gclose(void)
{
	RG_CLOSE(GRN2_PORT, GRN2_PIN);
}

#endif
