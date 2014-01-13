#include <misc.h>
#include "stm32f10x_flash.h"
#include "stm32f10x_rcc.h"

#include "core.h"

#include "app_usart.h"
#include "app_gprs.h"
#include "app_gps.h"
#include "app_lcd.h"
#include "app_rtc.h"
#include "app_key.h"
#include "app_voice.h"
#include "app_beep.h"
#include "board_init.h"
#include "driv_systick.h"
#include "app_rfid.h"

void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
#ifdef VECT_TAB_RAM
	// Set the Vector Tab base at location at 0x20000000
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else
	// Set the Vector Tab base at location at 0x80000000
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
	/* Configure the NVIC Preemption Priority Bits[配置优先级组] */
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void board_init(void)
{
	uint64_t i;
	
	NVIC_Config();
	//SystemInit();	/*外部是12MHz振荡器，所以应该是108MHz系统频率*/

	driv_systick_init();
	app_usart_init();

	app_speed_init();

	app_rtc_init();
	
	app_gprs_init();

	app_gps_init();

	app_key_init();

	app_beep_init();

	i = ticks;
	i += 1*HZ;
	while (i > ticks) ;

	app_lcd_init();

	i = ticks;
	i += 5*HZ;
	while (i > ticks) ;

	app_voice_init();

	app_rfid_init();
}

