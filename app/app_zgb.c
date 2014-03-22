#include "public.h"

#if defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_)

#include "driv_zgb.h"
#include "app_zgb.h"

void app_zgb_init(void)
{
	driv_zgb_init();
}

void app_zgb_proc(char *buf, int *len, int maxlen)
{
	driv_zgb_get(buf, len, maxlen);		//0,00124B00025F1A85,-26
}


#if 0

#define HEAD		"Z:"
#define TAIL		"#"

#define ZGB_INFO_MAX	128

struct zgb_info_ {
	uint16_t head;
	uint16_t tail;
	char msg[ZGB_INFO_MAX];
};

struct zgb_info_ zgb_info;

void app_zgb_init(void)
{
	driv_zgb_init();
}

void app_zgb_proc(void)
{
	char buf[64];
	
	if (driv_zgb_get_single(buf)) {		//0,00124B00025F1A85,-26
	
		add_to_save_single(buf);
	}
}

int32_t app_zgb_get_single(char *buf)
{
	int32_t r;
	char buf[64];
	
	r = get_form_save_single(buf);
	if (r <= 0)
		return r;
	
}

static int32_t add_to_save_single(char *buf, char *tobuf, uint16_t *read, uint16_t *insert, int tobuf_MAX)
{
	int len;
	int li, lr;

	len = strlen(buf);
	li = *insert;
	lr = *read;

	if (li < lr) {
		if (li + len >= lr)
			return 0;
		memcpy(tobuf + li, buf, len);
	} else {
		if (tobuf_MAX - (li - lr) - 2 < len)
			return 0;
		if (tobuf_MAX - li >= len) {
			memcpy(tobuf + li, buf, len);
			li += len;
		} else {
			memcpy(tobuf + li, buf, tobuf_MAX - li);
			memcpy(tobuf, buf + tobuf_MAX - li, len - (tobuf_MAX - li));
			li = len - (tobuf_MAX - li);
		}
	}

	if (li >= tobuf_MAX)
		li = 0;

	*insert = li;
}

static int32_t get_form_save_single(char *buf, char *tobuf, uint16_t *read, uint16_t *insert, int tobuf_MAX, char *head, char *end)
{
	int li, lr;
	char *p1, p2;

	li = *insert;
	lr = *read;

	if (lr <= li) {
		p1 = memstr((void *)(tobuf + lr), li - lr, head, strlen(head));
		if (p1 == NULL) {
			lr = lr;
		}
		p1 += strlen(head);
		p2 = memstr((void *)p1, tobuf + li - p1, end, strlen(end));
		if (p2) {
			
		}
	}
}
#endif

#endif
