#include "app_network_data.h"
#include "app_voice.h"
#include "app_lcd_memu.h"
#include "app_usart.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct app_network_yn_questions_		app_network_yn_questions;
struct app_network_voices_ 			app_network_voices;
struct app_network_text_notice_			app_network_text_notice;

static void app_network_yn_questions_proc(uint8_t *buf)
{
	uint8_t i;
	uint8_t val;
	uint8_t index;
	uint8_t *p;
	uint8_t buffer[3];
	uint8_t yn[YN_Q_LEN];

	p = buf;
	buffer[2] = '\0';

	strncpy((void *)buffer, (void *)p, 2);
	index = strtol((void *)buffer, NULL, 16);

	i = 0;
	p += 2;
	while (isalnum(*p)) {
		strncpy((void *)buffer, (void *)p, 2);
		val = strtol((void *)buffer, NULL, 16);
		yn[i] = val;
		i++;
		if (i >= YN_Q_LEN - 1)
			break;

		p +=2;
	}
	yn[i] = '\0';

	app_lcd_menu_insert_yn_question((void *)yn, index);
/*	val = 1;
	com_send_nchar(USART_GPS_NUM, &val, 1);
	com_send_nchar(USART_GPS_NUM, yn, strlen(yn));
	yn[0] = '\r';
	yn[1] = '\n';
	com_send_nchar(USART_GPS_NUM, yn, 2);*/
}

static void app_network_voices_proc(uint8_t *buf)
{
	uint8_t val;
	uint8_t *p;
	uint8_t buffer[3];

	p = buf;
	buffer[2] = '\0';

	strncpy((void *)buffer, (void *)p, 2);
	val = strtol((void *)buffer, NULL, 16);
	app_network_voices.app_network_voice[app_network_voices.tail].index = val;
	app_network_voices.tail++;
	if (app_network_voices.tail >= VOICE_MAX)
		app_network_voices.tail = 0;

	p += 2;
	while (isalnum(*p)) {
		strncpy((void *)buffer, (void *)p, 2);
		val = strtol((void *)buffer, NULL, 16);
		app_voice_data_insert(&val, 1);

		p +=2;
	}
}

static void app_network_text_proc(uint8_t *buf)
{
	uint8_t i;
	uint8_t val;
	uint8_t *p;
	uint8_t buffer[3];

	p = buf;
	buffer[2] = '\0';

	strncpy((void *)buffer, (void *)p, 2);
	val = strtol((void *)buffer, NULL, 16);
	app_network_text_notice.index = val;

	i = 0;
	p += 2;
	app_network_text_notice.text[TEXT_NOTICE_LEN - 1]  = '\0';
	while (isalnum(*p)) {
		strncpy((void *)buffer, (void *)p, 2);
		val = strtol((void *)buffer, NULL, 16);
		app_network_text_notice.text[i] = val;
		i++;
		if (i >= TEXT_NOTICE_LEN - 1)
			break;

		p +=2;
	}
	app_network_text_notice.text[i] = '\0';
/*	val = 3;
	com_send_nchar(USART_GPS_NUM, &val, 1);
	com_send_nchar(USART_GPS_NUM, app_network_text_notice.text, strlen(app_network_text_notice.text));
	com_send_nchar(USART_GPS_NUM, "\r\n", 2);*/
}

void app_network_data_proc(uint8_t *buf)
{
	uint8_t *p1;

	p1 = (void *)strstr((void *)buf, "T:");
	if (p1 == NULL)
		return;
	p1 += 2;
	if (*p1 == 'A' && *(p1 + 1) == ':') {
		app_network_yn_questions_proc(p1 + 2);
	}
	else if (*p1 == 'B' && *(p1 + 1) == ':') {
		app_network_voices_proc(p1 + 2);
	}
	else if (*p1 == 'C' && *(p1 + 1) == ':') {
		app_network_text_proc(p1 + 2);
	}
}

void app_network_data_init(void)
{
	//char buf[64];
	app_network_yn_questions.head = 0;
	app_network_yn_questions.tail = 0;
	app_network_yn_questions.app_lcd_yn_question[0].text[0] = '\0';

	app_network_voices.head = 0;
	app_network_voices.tail = 0;
	app_network_voices.app_network_voice[0].index = 0;

	app_network_text_notice.index = 0;
	app_network_text_notice.text[0] = '\0';

/*	strcpy((void *)buf, "T:A:00C4E3BAC3##");
	app_network_data_proc((void *)buf);
	strcpy((void *)buf, "T:C:00C4E3BAC3##");
	app_network_data_proc((void *)buf);*/
}

