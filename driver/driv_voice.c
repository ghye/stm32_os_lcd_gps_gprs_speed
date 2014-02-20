#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include "driv_voice.h"
#include "core.h"
#include "driv_systick.h"

#if defined (CAR_DB44_V1_0_20130315_)

#define BUSY_PIN	GPIO_Pin_9
#define DATA_PIN		GPIO_Pin_10
#define REST_PIN		GPIO_Pin_11
#define OTP_VOICE_PORT	GPIOE

static void driv_voice_tim_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	uint16_t CCR1_Val = 25000;

	TIM_DeInit(TIM1);  
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 50000;
	TIM_TimeBaseStructure.TIM_Prescaler = 4;//0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	/*TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;*/
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	  
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);	

	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	TIM_ITConfig(TIM1, TIM_IT_CC2, ENABLE);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM1, DISABLE);
}

void otp_voice_init(void)
{
	extern volatile unsigned char voice_update_cnt;
	
	uint64_t t;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);
	
	// Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = BUSY_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OTP_VOICE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = DATA_PIN;
	GPIO_Init(OTP_VOICE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = REST_PIN;
	GPIO_Init(OTP_VOICE_PORT, &GPIO_InitStructure);

	driv_voice_tim_init();

	GPIO_SetBits(OTP_VOICE_PORT, REST_PIN);
	t = ticks;
	t += 1000/TICK_MS;
	while (ticks < t) ;
	GPIO_ResetBits(OTP_VOICE_PORT, REST_PIN);

	voice_update_cnt = 1;
	TIM_Cmd(TIM1, ENABLE);
	t = ticks;
	t += 2000/TICK_MS;
	while (ticks < t) ;
	voice_update_cnt = 1;
	TIM_Cmd(TIM1, ENABLE);
}

static bool driv_voice_busy(void)
{
	if (GPIO_ReadInputDataBit(OTP_VOICE_PORT, BUSY_PIN))
		return false;
	return true;
}

static void driv_voice_rest(void)
{
	extern volatile unsigned int Timer1;
	
	GPIO_SetBits(OTP_VOICE_PORT, REST_PIN);
	os_task_delayms(3);
	GPIO_ResetBits(OTP_VOICE_PORT, REST_PIN);
	os_task_delayms(3);
}

void driv_voice_play(enum e_otp_voice_pluses w)
{
	extern volatile unsigned char voice_update_cnt;

	do {
		while(driv_voice_busy()) 
			os_task_delayms(1);
		
		driv_voice_rest();
		
		voice_update_cnt = w;
		TIM_Cmd(TIM1, ENABLE);
	} while(0);
}

#endif
