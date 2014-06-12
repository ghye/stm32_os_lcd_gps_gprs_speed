#ifndef __PROJ_CONF_H__
#define __PROJ_CONF_H__

#define SVERSION	"V1.0.0"

#if defined (CAR_DB44_V1_0_20130315)	/*STM32F105�����DB44����*/
#define CAR_DB44_V1_0_20130315_
#define FUN			"car"
#define HVERSION	"H1.0.20130315"
#elif defined (DouLunJi_CAR_GBC_V1_2_130511) /*STM32F103RCT6 ���ط��������Ķ��ֻ�һ�����GPRS�� ���ᴫ����*/
#define DouLunJi_CAR_GBC_V1_2_130511_
#define FUN			"car"
#define HVERSION	"H1.2.130511"
#elif defined(DouLunJi_AIS_BASE_STATION_V1_0_130513)	/*STM32F103RFT6Ģ��ͷ����ʵ�ֵĶ��ֻ������������������zigbee��ǰ�����*/
#define DouLunJi_AIS_BASE_STATION_V1_0_130513_
#define FUN			"aisBaseStation"
#define HVERSION	"H1.0.130513"
#elif defined(DouLunJi_CAR_TRUCK_1_3_140303)
#define DouLunJi_CAR_TRUCK_1_3_140303_
#define FUN			"car"
#define HVERSION	"H1.3.140303"
#elif defined(CAR_TRUCK_1_5_140325)
#define CAR_TRUCK_1_5_140325_
#define FUN			"car"
#define HVERSION	"H1.5.140325"
#elif defined(HC_CONTROLER)	/*������ͷ����ֵ��ϵͳ�Ŀ�����*/
#define HC_CONTROLER_
#define FUN			"hc_control"
#define HVERSION	"H1.0"
#else
#error "Please define project!!"
#endif

#endif
