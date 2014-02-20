#include <misc.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_exti.h>

#include "core.h"
#include "driv_key.h"
#include "app_key.h"
#include "app_beep.h"
#include "driv_systick.h"

#if defined (CAR_DB44_V1_0_20130315_)

struct dkey_st dkey[KEY_NUM];

void driv_key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	dkey[0].keyname = E_KEY_NAME_OK;
	dkey[0].status = E_DKEY_NOPRESS;
	dkey[1].keyname = E_KEY_NAME_LEFT;
	dkey[1].status = E_DKEY_NOPRESS;
	dkey[2].keyname = E_KEY_NAME_RIGHT;
	dkey[2].status = E_DKEY_NOPRESS;
	dkey[3].keyname = E_KEY_NAME_BACK;
	dkey[3].status = E_DKEY_NOPRESS;
	
#if 1
#else
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	/* Connect Key Button EXTI Line to Key Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);

	/* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Connect Key Button EXTI Line to Key Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);

	/* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Connect Key Button EXTI Line to Key Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource8);

	/* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Connect Key Button EXTI Line to Key Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);

	/* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Configure Key Button GPIO Pin as input floating (Key Button EXTI Line) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Enable the EXTI9_5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

void driv_key_process(enum key_name kname)
{
/*	extern enum key_name key;
	
	switch (kname) {
	case E_KEY_NAME_OK:
	case E_KEY_NAME_LEFT:
	case E_KEY_NAME_RIGHT:
	case E_KEY_NAME_BACK:
		break;
	default:
		break;
	}

	key = kname;*/
}


/*ispress: 该按键是否被按下状态*/
static void driv_key_status(enum key_name kname, bool ispress)
{
	extern volatile uint64_t ticks;

	uint8_t i;

	for (i=0; i<KEY_NUM; i++) {
		if (dkey[i].keyname == kname)
			break;
	}

	if (i >= KEY_NUM)
		return;

	switch (dkey[i].status) {
	case E_DKEY_NOPRESS:
		if (true == ispress) {
			dkey[i].stime = ticks;
			dkey[i].status = E_DKEY_DEBOUNCE;
		}
		break;
	case E_DKEY_DEBOUNCE:
		if (true == ispress) {
			dkey[i].stime = ticks;
			dkey[i].status = E_DKEY_SHORT_PRESS;

			app_beep_open();
		} else {
			dkey[i].status = E_DKEY_NOPRESS;
		}
		break;
	case E_DKEY_SHORT_PRESS:
		if (true == ispress) {
			if (ticks > dkey[i].stime + 1000/TICK_MS) {
				dkey[i].stime = ticks;
				dkey[i].status = E_DKEY_LONG_PRESS	;
			}
		} else {
			app_key_set_status(dkey[i].keyname, E_KEY_SINGLE_CLICK);
			dkey[i].status = E_DKEY_NOPRESS;
		}

		app_beep_close();
		break;
	case E_DKEY_LONG_PRESS:
		if (true == ispress) {

		} else {
			app_key_set_status(dkey[i].keyname, E_KEY_LONG_PRESS);
			dkey[i].status = E_DKEY_NOPRESS;
		}
		break;
	default:
		break;
	}
}

static void driv_key_status_process(enum key_name kname)
{
	if (kname & E_KEY_NAME_BACK) {
		driv_key_status(E_KEY_NAME_BACK, true);
	} else {
		driv_key_status(E_KEY_NAME_BACK, false);
	}

	if (kname & E_KEY_NAME_RIGHT) {
		driv_key_status(E_KEY_NAME_RIGHT, true);
	} else {
		driv_key_status(E_KEY_NAME_RIGHT, false);
	}

	if (kname & E_KEY_NAME_LEFT) {
		driv_key_status(E_KEY_NAME_LEFT, true);
	} else {
		driv_key_status(E_KEY_NAME_LEFT, false);
	}

	if (kname & E_KEY_NAME_OK) {
		driv_key_status(E_KEY_NAME_OK, true);
	} else {
		driv_key_status(E_KEY_NAME_OK, false);
	}
}

void driv_key_tick_process(void)
{
	#define KEY_INT_MS 20
	extern enum key_name key;
	static uint64_t ltick = 0;
	uint64_t lt;
	
	enum key_name lkey;

	lt = ticks;

	if (TICK_MS < KEY_INT_MS) {
		if (lt - ltick < KEY_INT_MS / TICK_MS)
			return;
		else
			ltick = lt;
	}

	lkey = E_KEY_NAME_ERROR;

	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) == Bit_RESET)
		lkey |= E_KEY_NAME_BACK;
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == Bit_RESET)
		lkey |= E_KEY_NAME_RIGHT;
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == Bit_RESET)
		lkey |= E_KEY_NAME_LEFT;
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) == Bit_RESET)
		lkey |= E_KEY_NAME_OK;

	driv_key_status_process(lkey);
	
	if (E_KEY_NAME_ERROR != lkey)
		key = lkey;
}

#endif
