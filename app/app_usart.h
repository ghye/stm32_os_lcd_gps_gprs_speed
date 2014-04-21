#ifndef __APP_USART_H__
#define __APP_USART_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

#if defined (CAR_DB44_V1_0_20130315_)

#define USART_GPRS_NUM		2
#define USART_GPS_NUM		5
#define USART_SPEED_NUM	1

#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

#define USART_GPRS_NUM		1
#define USART_GPS_NUM		2
#define USART_ZGB_NUM		5

#elif defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_)

#define USART_GPRS_NUM		2
#define USART_GPS_NUM		3
#define USART_ZGB_NUM		1

#elif defined(DouLunJi_CAR_TRUCK_1_3_140303_)

#define USART_GPRS_NUM		1
#define USART_GPS_NUM		2
#define USART_ZGB_NUM		4 ///4 ÖØÐÂÒþÉäµ½

#elif defined(CAR_TRUCK_1_5_140325_)

#define USART_GPRS_NUM		1
#define USART_GPS_NUM		4
#define USART_ZGB_NUM		2
#define USART_CAM_NUM		3

#endif

void app_usart_init(void);
void app_usart2_send(uint8_t id, uint8_t val);
void app_usart2_send_msg(void *msg);

#endif

