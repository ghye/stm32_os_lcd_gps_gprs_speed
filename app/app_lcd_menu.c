#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "public.h"
#include "driv_lcd.h"
#include "app_rtc.h"
#include "app_lcd_memu.h"
#include "app_network_data.h"
#include "app_rfid.h"

#if defined (CAR_DB44_V1_0_20130315_)

uint8_t logo_gps[32] = {
	0x00, 0x00, 0x70, 0x08, 0x54, 0x14, 0x7A, 0x22, 0x11, 0x44, 0x20, 0x88, 0x10, 0x50, 0x08, 0x20, 
	0x04, 0x10, 0x0A, 0x08, 0x11, 0x08, 0x22, 0x88, 0x44, 0x78, 0x28, 0x04, 0x10, 0x02, 0x00, 0x00
};
uint8_t logo_gprs_y[32] = {
	0x00, 0x00, 0x03, 0xFE, 0x02, 0x02, 0x02, 0x02, 0x7F, 0xC2, 0x40, 0x42, 0x4E, 0x42, 0x4E, 0x7E, 
	0x4E, 0x60, 0x40, 0x60, 0x7F, 0xF8, 0x04, 0x00, 0x04, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00
};
uint8_t logo_gprs_n[32] = {
	0x00, 0x00, 0x03, 0xFE, 0x02, 0x02, 0x12, 0x12, 0x7F, 0xE2, 0x44, 0x42, 0x42, 0xC2, 0x41, 0x7E, 
	0x42, 0xE0, 0x44, 0x60, 0x7F, 0xF8, 0x14, 0x10, 0x04, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00
};
uint8_t logo_gprs_signal_level[6][32] = {
	0x00, 0x00, 0x7F, 0xC0, 0x20, 0x80, 0x11, 0x00, 0x0A, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 
	0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x7F, 0xC0, 0x20, 0x80, 0x11, 0x00, 0x0A, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 
	0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x7F, 0xC0, 0x20, 0x80, 0x11, 0x00, 0x0A, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 
	0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x7F, 0xC0, 0x20, 0x80, 0x11, 0x00, 0x0A, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 
	0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x7F, 0xC0, 0x20, 0x80, 0x11, 0x00, 0x0A, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 
	0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x7F, 0xC0, 0x20, 0x80, 0x11, 0x00, 0x0A, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 
	0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void app_lcd_main_insert_time(void)
{
	uint8_t buf[64];
	
	sprintf((void *)buf, "%.4d/%.2d/%.2d %.2d:%.2d:%.2d", calendar.year, calendar.month, calendar.mday, calendar.hour, calendar.min, calendar.sec);
	driv_lcd_disp_dma(buf, strlen((void *)buf), 1, 0);
}

/*
	参数:
		satellites:GPS卫星个数
*/
static void app_lcd_main_insert_gps_status(uint8_t satellites)
{
	uint8_t buf[8];

	sprintf((void *)buf, "%.2d", satellites);
	driv_lcd_disp_dots_dma(logo_gps, 16, 16, 0, 0);
	driv_lcd_disp_dma(buf, strlen((void *)buf), 0, 1);
}

static void app_lcd_main_insert_gprs_status(bool net_ok)
{
	uint8_t *p;

	if (net_ok == true)
		p = logo_gprs_y;
	else
		p = logo_gprs_n;

	driv_lcd_disp_dots_dma(p, 16, 16, 0, 32);
}

/*
	参数:
			level:	0 ~ 5
*/
static void app_lcd_main_insert_gprs_signal(uint8_t level)
{
	uint8_t *p;

	if (level > 5)
		level = 5;
	p = logo_gprs_signal_level[level];
	
	driv_lcd_disp_dots_dma(p, 16, 16, 0, 48);
}

static void app_lcd_main_insert_speed(uint8_t *str)
{
	#define SPEED_STYLE "车速 %sKM/H"
	uint8_t buf[64];

	sprintf((void *)buf, SPEED_STYLE, str);
	driv_lcd_disp_dma(buf, strlen((void *)buf), 0, 4);
}

static void app_lcd_main_insert_driver_code(uint8_t *str)
{
	#define DRIVER_CODE_STYLE "司机 %s"
	uint8_t buf[64];

	sprintf((void *)buf, DRIVER_CODE_STYLE, str);
	driv_lcd_disp_dma(buf, strlen((void *)buf), 0, 4);
}

static void app_lcd_menu_l1_insert_text_notice(void)
{
	driv_lcd_disp_dma(app_network_text_notice.text, strlen((void *)(app_network_text_notice.text)), 1, 0);
}

static uint16_t app_lcd_menu_l1_insert_yn_question(void)
{
	uint16_t index;
	char buf[YN_Q_LEN];

	app_lcd_menu_load_yn_question(buf, &index);
	driv_lcd_disp_dma(buf, strlen((void *)buf), 0, 0);
	driv_lcd_disp_dma("确定", 4, 1, 1);
	driv_lcd_disp_dma("取消", 4, 1, 7);

	return index;
}

static void app_lec_menu_insert_rfid_serial_num(void)
{
	uint8_t buf[64];

	sprintf((void *)buf, "%s", "卡号");
	driv_lcd_disp_dma(buf, strlen((void *)buf), 2, 0);
	sprintf((void *)buf, "%.10u", g_rfid_serial_num);
	driv_lcd_disp_dma(buf, strlen((void *)buf), 2, 2);
}




/*
	参数:
			p:	问题的文本
			index:	问题索引标识
*/
void app_lcd_menu_insert_yn_question(const char *p, uint16_t index)
{
	uint32_t tail;

	tail = app_network_yn_questions.tail;
	app_network_yn_questions.app_lcd_yn_question[tail].index = index;
	strcpy((void *)app_network_yn_questions.app_lcd_yn_question[tail].text, p);
	tail++;
	if (tail >= YN_Q_MAX)
		tail = 0;
	app_network_yn_questions.tail = tail;
}

bool app_lcd_menu_is_have_yn_question(void)
{
	uint32_t head;
	uint32_t tail;

	head = app_network_yn_questions.head;
	tail = app_network_yn_questions.tail;
	
	if (head == tail)
		return false;

	return true;
}

/*
	参数:
			p:	问题的文本
			index:	问题索引标识
	注意: 调用此函数应该先判断app_lcd_menu_is_have_yn_question()
*/
void app_lcd_menu_load_yn_question(char *p, uint16_t *index)
{
	uint32_t head;
	uint32_t tail;

	head = app_network_yn_questions.head;
	tail = app_network_yn_questions.tail;
	
	*p = '\0';

	*index = app_network_yn_questions.app_lcd_yn_question[tail].index;
	strcpy(p, (void *)app_network_yn_questions.app_lcd_yn_question[head].text);
	head++;
	if (head >= YN_Q_MAX)
		head = 0;
	app_network_yn_questions.head = head;
}

void app_lcd_menu_init(void)
{
	app_network_yn_questions.head = 0;
	app_network_yn_questions.tail = 0;
}

void app_lcd_main_menu_1(void)
{
	app_lcd_main_insert_gps_status(0);
	app_lcd_main_insert_gprs_status(false);
	app_lcd_main_insert_gprs_signal(0);
	app_lcd_main_insert_speed("0");
	app_lcd_main_insert_time();

	driv_lcd_tx_reload_dma();
}

void app_lcd_main_menu_1_flash(void)
{
	app_lcd_main_insert_speed("0");
	app_lcd_main_insert_time();

	driv_lcd_tx_reload_dma();
}

void app_lcd_main_menu_2(void)
{
	app_lcd_main_insert_gps_status(0);
	app_lcd_main_insert_gprs_status(false);
	app_lcd_main_insert_gprs_signal(0);
	app_lcd_main_insert_driver_code("000000");
	app_lcd_main_insert_time();

	driv_lcd_tx_reload_dma();
}

void app_lcd_main_menu_2_flash(void)
{
	app_lcd_main_insert_time();

	driv_lcd_tx_reload_dma();
}

void app_lcd_main_menu_3(void)
{
	app_lcd_main_insert_gps_status(0);
	app_lcd_main_insert_gprs_status(false);
	app_lcd_main_insert_gprs_signal(0);
	app_lcd_main_insert_driver_code("000000");
	app_lcd_menu_l1_insert_text_notice();

	app_lec_menu_insert_rfid_serial_num();
	
	driv_lcd_tx_reload_dma();
}

void app_lcd_main_menu_3_flash(void)
{
	app_lcd_menu_l1_insert_text_notice();

	driv_lcd_tx_reload_dma();
}


void app_lcd_menu_l1_yn_question(void)
{
	app_lcd_menu_l1_insert_yn_question();
	
	driv_lcd_tx_reload_dma();
}

#endif
