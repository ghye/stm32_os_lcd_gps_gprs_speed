#ifndef __PROJ_CONF_H__
#define __PROJ_CONF_H__

#if defined (CAR_DB44_V1_0_20130315)	/*STM32F105�����DB44����*/
#define CAR_DB44_V1_0_20130315_
#elif defined (DouLunJi_CAR_GBC_V1_2_130511) /*STM32F103RCT6 ���ط��������Ķ��ֻ�һ�����GPRS�� ���ᴫ����*/
#define DouLunJi_CAR_GBC_V1_2_130511_
#elif defined(DouLunJi_AIS_BASE_STATION_V1_0_130513)	/*STM32F103RFT6Ģ��ͷ����ʵ�ֵĶ��ֻ������������������zigbee��ǰ�����*/
#define DouLunJi_AIS_BASE_STATION_V1_0_130513_
#elif defined(DouLunJi_CAR_TRUCK_1_3_140303)
#define DouLunJi_CAR_TRUCK_1_3_140303_
#else
#error "Please define project!!"
#endif

#endif
