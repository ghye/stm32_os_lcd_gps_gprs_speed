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
#include "app_hmc5883l_bmp085.h"
#include "app_watchdog.h"
#include "app_zgb.h"
#include "app_vc0706.h"
#include "app_cpu_id.h"
#include "app_net_usart.h"
#include "app_ir.h"
#include "app_coil.h"
#include "app_weighbridge.h"

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

	#if (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_))
	
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	#elif defined(DouLunJi_CAR_GBC_V1_2_130511_)

	NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	#endif
}

void board_init(void)
{
	uint64_t i;

	#if 1
	app_wdg_init();
	app_wdg_start();
	#endif
	
	NVIC_Config();
	//SystemInit();	/*外部是12MHz振荡器，所以应该是108MHz系统频率*/

	#if defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_) || defined(HC_CONTROLER_)
	app_zgb_init();
	#endif

	#if defined(HC_CONTROLER_)
	app_zgb2_init();
	#endif
	
	driv_systick_init();
	app_usart_init();

	#if defined (CAR_DB44_V1_0_20130315_)
	app_speed_init();
	#endif

//	app_rtc_init();

	#if defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_)
	app_gprs_init();
	#endif

	#if defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_)
	app_gps_init();
	#endif

	#if defined (CAR_DB44_V1_0_20130315_)
	app_key_init();
	#endif

	#if defined (CAR_DB44_V1_0_20130315_)
	app_beep_init();
	#endif

/*	i = ticks;
	i += 1*HZ;
	while (i > ticks) ;
*/
	#if defined (CAR_DB44_V1_0_20130315_)
	app_lcd_init();
	#endif

/*	i = ticks;
	i += 5*HZ;
	while (i > ticks) ;
*/
	#if defined (CAR_DB44_V1_0_20130315_) || defined(HC_CONTROLER_)
	app_voice_init();
	#endif

	#if defined (CAR_DB44_V1_0_20130315_)
	app_rfid_init();
	#endif

	#if (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_))
	app_hmc5883l_bmp085_init();
	#endif

	#if defined(CAR_TRUCK_1_5_140325_)
	app_vc0706_init();
	#endif

	#if defined(HC_CONTROLER_)
	app_net_usart_init();
	#endif

	#if defined(HC_CONTROLER_)
	app_ir_init();
	#endif

	#if defined(HC_CONTROLER_)
	app_coil_init();
	#endif

	#if defined(HC_CONTROLER_)
	app_weighbridge_init();
	#endif

	app_cpu_id_init();
}

