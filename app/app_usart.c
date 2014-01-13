#include<stdio.h>

#include "stm32f10x.h"
#include "driv_usart.h"
#include "app_usart.h"

void app_usart_init(void)
{
	com_para_t com_para;

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

