#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
	
#include "core.h"

#include "app_gprs.h"
#include "app_usart.h"
#include "app_gps.h"
#include "app_lcd.h"
#include "app_rtc.h"
#include "app_speed.h"
#include "app_voice.h"
#include "app_rfid.h"
#include "app_hmc5883l_bmp085.h"
#include "app_watchdog.h"

void task_gprs(void *arg)
{
	while (1) {
		app_gprs_clean_seqed_msgs();
		app_gprs_form_seqed_msgs();
		app_gprs_socket();

		app_wdg_update_task_tick(0);
		app_wdg_keeplibe();
		os_task_delayms(700);
	}
}

void task2(void *arg)
{
	while (1) {
		#if defined (CAR_DB44_V1_0_20130315_)
//		app_lcd_disp();
		#endif

		app_wdg_update_task_tick(1);
		os_task_delayms(100);
	}

#if 0
	uint8_t i = 0;
	uint16_t j;
	double f;
	int8_t buf[16];
	
	while(1)
	{
		j=1+(int)(10.0*rand()/(RAND_MAX+1.0));
		j *= 100;
		i++;
		//app_usart2_gpio();
		//app_usart2_send(2, i);
		//memset((void *)&f, 0xFF, sizeof(f));
		f = 65535.89999;
		sprintf((void *)buf, "%f", f);
		app_usart2_send_msg(buf);
		//os_task_delayms(j);
		os_task_switch();
	}
#endif
}

void task3(void *arg)
{
	while (1) {
		#if defined (CAR_DB44_V1_0_20130315_)
		app_voice_play();
		#endif

		#if defined (CAR_DB44_V1_0_20130315_)
		//app_rfid_proc();
		#endif

		#if (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_))
		app_hmc5883l_bmp085();
		#endif

		app_wdg_update_task_tick(2);
		os_task_delayms(/*5000*/20);
	}
}

void task4(void *arg)
{
	while (1) {
		app_gps_parse();

		#if defined (CAR_DB44_V1_0_20130315_)
		app_speed_calc();
		#endif
		
		app_rtc_process();

		app_wdg_update_task_tick(3);
		os_task_switch();
		
		//os_task_delayms(10);
	}
	
/*	uint8_t i = 0;
	uint16_t j;
	
	while(1)
	{
		j=1+(int)(10.0*rand()/(RAND_MAX+1.0));
		j *= 100;
		i++;
		app_usart2_send(3, i);
		os_task_delayms(j);
		//os_task_switch();
	}*/
}

