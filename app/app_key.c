#include <stdio.h>
#include <string.h>

#include "driv_key.h"
#include "app_key.h"

#if defined (CAR_DB44_V1_0_20130315_)

#define APP_KEY_NUM 8

struct app_key_list {
	uint8_t head;
	uint8_t tail;
	struct app_key ak[APP_KEY_NUM];
};

struct app_key_list akl;


void app_key_get_status(struct app_key *ak)
{

	if (akl.head == akl.tail) {
		ak->name = E_KEY_NAME_ERROR;
		return;
	}

	ak->name = akl.ak[akl.head].name;
	ak->status = akl.ak[akl.head].status;

	akl.head++;
	if (akl.head >= APP_KEY_NUM)
		akl.head = 0;
}

void app_key_set_status(enum key_name name, enum key_status status)
{
	akl.ak[akl.tail].name = name;
	akl.ak[akl.tail].status = status;

	akl.tail++;
	if (akl.tail >= APP_KEY_NUM)
		akl.tail = 0;
}

void app_key_init(void)
{
	driv_key_init();

	memset((void *)&akl, 0, sizeof(akl));
}

uint8_t STATUS_STR_SHORT[] = "u";
uint8_t STATUS_STR_LONG[] = "l";

uint8_t STR_OK[] = "O";
uint8_t STR_LEFT[] = "L";
uint8_t STR_RIGHT[] = "R";
uint8_t STR_BACK[] = "B";
uint8_t keystr[16];
enum key_name key = E_KEY_NAME_ERROR;

static uint8_t *app_key_get_key_str(enum key_name kname)
{
	uint8_t *p;

	p = NULL;
	
	switch (kname) {
	case E_KEY_NAME_OK:
		p = STR_OK;
		break;
	case E_KEY_NAME_LEFT:
		p = STR_LEFT;
		break;
	case E_KEY_NAME_RIGHT:
		p = STR_RIGHT;
		break;
	case E_KEY_NAME_BACK:
		p = STR_BACK;
		break;
	default:
		break;
	};

	return p;
}


uint8_t *app_key_process(void)
{
	uint8_t *p1;
	uint8_t *p2;
	struct app_key ak;
	
	app_key_get_status(&ak);
	if (ak.status == E_KEY_SINGLE_CLICK)
		p2 = STATUS_STR_SHORT;
	else
		p2 = STATUS_STR_LONG;
	p1 = app_key_get_key_str(ak.name);
	
	if (NULL == p1)
		return NULL;

	sprintf(keystr, "%s%s", p1, p2);
	return keystr;
}

#endif
