#ifndef __APP_NETWORK_DATA_H__
#define __APP_NETWORK_DATA_H__

#include <stdint.h>
#include <stdbool.h>

#define YN_Q_LEN	64
#define YN_Q_MAX	5
#define VOICE_MAX	5
#define TEXT_NOTICE_LEN	64

struct app_network_yn_question_ {
	uint16_t index;
	uint8_t text[YN_Q_LEN];
};

struct app_network_yn_questions_ {
	uint32_t head;
	uint32_t tail;
	struct app_network_yn_question_ app_lcd_yn_question[YN_Q_MAX];
};

struct app_network_voice_ {
	uint16_t index;
};

struct app_network_voices_ {
	uint32_t head;
	uint32_t tail;
	struct app_network_voice_ app_network_voice[VOICE_MAX];
};

struct app_network_text_notice_ {
	uint16_t index;
	uint8_t text[TEXT_NOTICE_LEN];
};



extern struct app_network_yn_questions_ 		app_network_yn_questions;
extern struct app_network_voices_ 			app_network_voices;
extern struct app_network_text_notice_		app_network_text_notice;

void app_network_data_proc(uint8_t *buf);
void app_network_data_init(void);

#endif

