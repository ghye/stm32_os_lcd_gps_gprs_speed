#ifndef __PROJ_CONF_H__
#define __PROJ_CONF_H__

#if defined (CAR_DB44_V1_0_20130315)	/*STM32F105最早的DB44主板*/
#define CAR_DB44_V1_0_20130315_
#elif defined (DouLunJi_CAR_GBC_V1_2_130511) /*STM32F103RCT6 车载分体机改造的斗轮机一体机，GPRS、 九轴传感器*/
#define DouLunJi_CAR_GBC_V1_2_130511_
#elif defined(DouLunJi_AIS_BASE_STATION_V1_0_130513)	/*STM32F103RFT6蘑菇头主板实现的斗轮机后端主机，接收来自zigbee的前端倾角*/
#define DouLunJi_AIS_BASE_STATION_V1_0_130513_
#elif defined(DouLunJi_CAR_TRUCK_1_3_140303)
#define DouLunJi_CAR_TRUCK_1_3_140303_
#else
#error "Please define project!!"
#endif

#endif
