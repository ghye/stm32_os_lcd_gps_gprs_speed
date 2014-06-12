#include <stm32f10x_tim.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

#include "driv_voice.h"
#include "core.h"
#include "driv_systick.h"
#include "app_usart.h"

#if defined (CAR_DB44_V1_0_20130315_) || defined(HC_CONTROLER_)

#define TYPE_OTP		1
#define TYPE_NV040B		2

#define TYPE_VOICE_DEV	TYPE_NV040B

#if (TYPE_VOICE_DEV == TYPE_OTP)
#define BUSY_PIN	GPIO_Pin_9
#define DATA_PIN		GPIO_Pin_10
#define REST_PIN		GPIO_Pin_11
#define OTP_VOICE_PORT	GPIOE
#elif (TYPE_VOICE_DEV == TYPE_NV040B)
#define BUSY_PIN	GPIO_Pin_8
#define DATA_PIN	GPIO_Pin_5
#define DATA2_PIN	GPIO_Pin_6
#define REST_PIN		GPIO_Pin_7
#define OTP_VOICE_PORT	GPIOB
#else
#error "please define voice device!"
#endif

void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	                   //ʹ�ܶ�ʱ��3ʱ��
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);

	  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);                        //Timer3������ӳ��  TIM3_CH2->PB5    

	/*���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;        //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);           //��ʼ��GPIO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);           //��ʼ��GPIO

	/*��ʼ��TIM3*/
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim,TIM3ʱ��Ϊ72MHZ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	/*��ʼ��TIM3 Channel2 PWMģʽ*/	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3

	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);

	//TIM_CtrlPWMOutputs(TIM1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, DISABLE);
}

static void driv_voice_tim_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	#if (TYPE_VOICE_DEV == TYPE_OTP)
	
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

	#elif (TYPE_VOICE_DEV == TYPE_NV040B)

	uint16_t CCR1_Val = 25000;
	/*TIM3_PWM_Init(4999,4);
	return;*/

	TIM_DeInit(TIM3);  
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 50000;
	TIM_TimeBaseStructure.TIM_Prescaler = 4;//0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	/*TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;*/
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	  
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);	

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);

	//TIM_CtrlPWMOutputs(TIM1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, DISABLE);

	#endif
}

void otp_voice_init(void)
{
	extern volatile unsigned char voice_update_cnt;
	
	uint64_t t;
	GPIO_InitTypeDef GPIO_InitStructure;

	#if (TYPE_VOICE_DEV == TYPE_OTP)
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

	#elif (TYPE_VOICE_DEV == TYPE_NV040B)

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	// Configure USART Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = BUSY_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OTP_VOICE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = DATA_PIN;
	GPIO_Init(OTP_VOICE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = DATA2_PIN;	/* ��������û����128�Σ���ʹ��DATA1�Ϳ�����,����ģʽ����ΪGPIO_Mode_Out_PP */
	GPIO_Init(OTP_VOICE_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(OTP_VOICE_PORT, DATA2_PIN);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = REST_PIN;
	GPIO_Init(OTP_VOICE_PORT, &GPIO_InitStructure);

	driv_voice_tim_init();

	GPIO_SetBits(OTP_VOICE_PORT, REST_PIN);
	t = ticks;
	t += 1000/TICK_MS;
	while (ticks < t) ;
	GPIO_ResetBits(OTP_VOICE_PORT, REST_PIN);

	voice_update_cnt = 2;
	TIM_Cmd(TIM3, ENABLE);
	t = ticks;
	t += 2000/TICK_MS;
	while (ticks < t) ;
	voice_update_cnt = 3;
	TIM_Cmd(TIM3, ENABLE);
	#endif
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
	#if (TYPE_VOICE_DEV == TYPE_OTP)
	os_task_delayms(3);
	#elif (TYPE_VOICE_DEV == TYPE_NV040B)
	os_task_delayms(1);
	#endif
	GPIO_ResetBits(OTP_VOICE_PORT, REST_PIN);
	#if (TYPE_VOICE_DEV == TYPE_OTP)
	os_task_delayms(3);
	#elif (TYPE_VOICE_DEV == TYPE_NV040B)
	os_task_delayms(6);
	#endif
}

void driv_voice_play(enum e_otp_voice_pluses w)
{
	extern volatile unsigned char voice_update_cnt;

	do {
		while(driv_voice_busy()) 
			os_task_delayms(1);
		driv_voice_rest();
		voice_update_cnt = w;
		#if (TYPE_VOICE_DEV == TYPE_OTP)
		TIM_Cmd(TIM1, ENABLE);
		#elif (TYPE_VOICE_DEV == TYPE_NV040B)
		TIM_Cmd(TIM3, ENABLE);
		#endif
	} while(0);
}

#endif
