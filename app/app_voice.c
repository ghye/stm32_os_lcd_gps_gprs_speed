#include <string.h>

#include "driv_voice.h"
#include "app_voice.h"

#if defined (CAR_DB44_V1_0_20130315_) || defined(HC_CONTROLER_)

#define APP_VOICE_LEN	128

struct app_voice_data_ {
	uint16_t head;
	uint16_t tail;
	uint8_t snd[APP_VOICE_LEN];
};

struct app_voice_data_ app_voice_data;

void app_voice_data_init(void)
{
	app_voice_data.head = 0;
	app_voice_data.tail = 0;
}

void app_voice_data_insert(uint8_t *buf, uint16_t len)
{
	uint32_t tail, l;
	uint8_t *p;

	p = buf;
	tail = app_voice_data.tail;
	
	while (len > 0) {
		l = (len < (APP_VOICE_LEN - tail)) ? (len):(APP_VOICE_LEN - tail);
		memcpy((void *)(app_voice_data.snd + tail), (void *)p, l);

		len -= l;
		p += l;
		tail += l;
		if (tail >= APP_VOICE_LEN) {
			tail = 0;
		}
	}

	app_voice_data.tail = tail;
}

int32_t app_voice_data_read(void)
{
	uint32_t head, tail;
	int32_t data;

	head = app_voice_data.head;
	tail = app_voice_data.tail;
	
	if (head == tail)
		return -1;

	data = app_voice_data.snd[head];
	head++;
	if (head >= APP_VOICE_LEN)
		head = 0;

	app_voice_data.head = head;
	
	return data;
}

void app_voice_init(void)
{
	otp_voice_init();

	app_voice_data_init();
}

void app_voice_play(void)
{
	static uint8_t d = 2;
	int32_t data;

#if 0
	data = app_voice_data_read();
	if (data >= 0) {
		driv_voice_play(data);
	} /*else {
		app_voice_data_insert(&d, 1);
		d += 2;
		if (d > E_OVP_NEWORDER)
			d = 2;
	}*/
#else

	static uint8_t i = 0;

	i += 1;

	if (i > 108)//E_OVP_NEWORDER)
		i = 2;
	
	driv_voice_play(i);
#endif
}

#endif
