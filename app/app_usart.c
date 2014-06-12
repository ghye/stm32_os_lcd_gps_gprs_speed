#include<stdio.h>

#include "stm32f10x.h"
#include "driv_usart.h"
#include "app_usart.h"

void app_usart_init(void)
{
	com_para_t com_para;

	#if defined (CAR_DB44_V1_0_20130315_)
	
	/*for gprs*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPRS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);
	
	/*for gps*/
	com_para.baudrate = 9600;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	/*for speed*/
	com_para.baudrate = 9600;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_SPEED_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	#elif defined (DouLunJi_CAR_GBC_V1_2_130511_)

	/*for gprs*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPRS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	/*for gps*/
	com_para.baudrate = 9600;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	/*for ZGB*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_ZGB_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);	

	#elif defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_)

	/*for gprs*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPRS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	/*for gps*/
	com_para.baudrate = 9600;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);	

	/*for ZGB*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_ZGB_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);	

	#elif defined(DouLunJi_CAR_TRUCK_1_3_140303_)

	/*for gprs*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPRS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	/*for gps*/
	com_para.baudrate = 9600;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);	

	/*for ZGB*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_ZGB_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);	

	#elif defined(CAR_TRUCK_1_5_140325_)

	/*for gprs*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPRS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	/*for gps*/
	com_para.baudrate = 9600;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_GPS_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);	

	/*for ZGB*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_ZGB_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);	

	/*for CAM*/
	com_para.baudrate = 38400;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_CAM_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);	

	#elif defined(HC_CONTROLER_)

	/*for network*/
	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_NET_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_ZGB1_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	com_para.baudrate = 115200;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_ZGB2_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	com_para.baudrate = 2400;
	com_para.data_9bit = false;
	com_para.parity = 0;
	com_para.port = USART_WB_NUM;
	com_para.twoStopBits = false;
	com_init(&com_para);

	#endif
}

void app_usart2_send(uint8_t id, uint8_t val)
{
	uint8_t buf[8];

	sprintf((void *)buf, "%s%d:", "T", id);
	com_send_string(2, buf);
	com_send_hex(2, val);
	com_send_string(2, "  ");
}

void app_usart2_send_msg(void *msg)
{
	com_send_message(2, msg);
}

void app_usart2_gpio(void)
{
	#include "stm32f10x_gpio.h"
	
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2) == RESET)
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	else
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

