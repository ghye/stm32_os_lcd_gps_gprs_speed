#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "core.h"
#include "driv_systick.h"
#include "driv_gps.h"

#if  (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_))

#if defined (CAR_DB44_V1_0_20130315_)

#define GPS_PWR_OFF()	GPIO_SetBits(GPIOE, GPIO_Pin_1)
#define GPS_PWR_ON()	GPIO_ResetBits(GPIOE, GPIO_Pin_1)

#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

#define bd_reset_disable()	GPIO_SetBits(GPIOB, GPIO_Pin_14)	/*北斗二的复位引脚*/
#define bd_reset_enable()	GPIO_ResetBits(GPIOB, GPIO_Pin_14)	/*北斗二的复位引脚*/
#define GPS_PWR_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define GPS_PWR_ON()		GPIO_ResetBits(GPIOB, GPIO_Pin_8)

#elif defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_)

#define GPS_PWR_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define GPS_PWR_ON()		GPIO_ResetBits(GPIOB, GPIO_Pin_8)

#elif defined(DouLunJi_CAR_TRUCK_1_3_140303_)

#define GPS_PWR_OFF()		GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define GPS_PWR_ON()		GPIO_ResetBits(GPIOB, GPIO_Pin_8)

#endif

void driv_gps_power_reset(void)
{
	#if (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_))
	
	GPS_PWR_OFF();
	os_task_delayms(2000);
	GPS_PWR_ON();

	#elif defined(DouLunJi_CAR_GBC_V1_2_130511_)

	bd_reset_enable();
	os_task_delayms(2000);
	bd_reset_disable();

	#endif
}

void driv_gps_init(void)
{
	uint64_t i;
	GPIO_InitTypeDef GPIO_InitStructure;

	#if defined (CAR_DB44_V1_0_20130315_)
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPS_PWR_OFF();
	i = ticks;
	i += 2000/TICK_MS;
	GPS_PWR_ON();

	#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPS_PWR_ON();
	bd_reset_disable();

	#elif defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_)

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPS_PWR_ON();

	#elif defined(DouLunJi_CAR_TRUCK_1_3_140303_)

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPS_PWR_ON();

	#endif
}

#endif
