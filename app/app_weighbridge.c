#include "public.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "core.h"
#include "driv_systick.h"
#include "app_weighbridge.h"
#include "driv_weighbridge.h"
#include "alg_checksum.h"

#ifdef HC_CONTROLER_

#define WB_LEN		44	/* 至少两句整语句的长度 */
#define WB_TIMER 	1	/* 秒 */

static int tail;
static unsigned char wb_buf[WB_LEN];

static int weighbridge_parse_t1(char *buf, int max);
static int weighbridge_parse_t2(unsigned char *buf, int max);

void app_weighbridge_proc(char *buf, int *len, int max)
{
	#define MSG_REPORT_WB		"T:F:%d#"

	static uint64_t lticks = 0;

	int kg;
	int ltail;
	unsigned char lbuf[WB_LEN];

	*len = 0;

	if (ticks < lticks + WB_TIMER * HZ) {
		return;
	}
	lticks += WB_TIMER * HZ;

	ltail = tail;
	memcpy(lbuf, wb_buf + ltail, WB_LEN - ltail);
	memcpy(lbuf + (WB_LEN - ltail), wb_buf, ltail);

	kg = weighbridge_parse_t1((char *)lbuf, WB_LEN);
	if (kg >= 0) {
		sprintf(buf, MSG_REPORT_WB, kg);
		*len = strlen(buf);
	}
	/*sprintf((void *)lbuf, "kg=%d, tail=%d", kg, ltail);
	com_send_message(1, (void *)lbuf);
	for (kg = 0; kg < WB_LEN; kg++) {
		sprintf((void *)lbuf, "%.2X ", wb_buf[kg] & 0x7F);
		com_send_nchar(1, lbuf, strlen(lbuf));
	}
	com_send_message(1, "");*/
}

void app_weighbridge_init(void)
{
	driv_weighbridge_init();
	tail = 0;
}

void app_weighbridge_isr(unsigned char v)
{
	wb_buf[tail] = v;
	tail++;
	if (tail >= WB_LEN)
		tail = 0;
}

static int weighbridge_parse_t1(char *buf, int max)
{
	int i;
	int kg;
	char flag;

	kg = -1;
	flag = 0;
	for (i = 0; i < max; i++)
		buf[i] &= ~0x80;	/* 因为这个型号的地磅是7位数据长度 */
	for (i = 0; i < max; i++) {
		if ((*(buf + i) == ' ') && (max - i >= 11) && (*(buf + i + 9) == 0x0D) && (*(buf + i + 10) == 0x0A)) {
			flag = 1;
			break;
		}
	}
	if (flag == 1) {
		*(buf + i + 7) = '\0';
		kg = strtol((void *)(buf + i + 1), NULL, 10);
	}

	return kg;
}

/* 常州艾斯派尔电气科技有限公司，型号: AS232-SP/AS232D-SP */
static int weighbridge_parse_t2(unsigned char *buf, int max)
{
	int i;
	int kg;
	char flag;

	kg = -1;
	flag = 0;
	for (i = 0; i < max; i++) {
		if ((*(buf + i) == 0x02) && (max - i >= 18) && (*(buf + i + 16) == 0x0D)) {
			if (checksum_uchar(buf + i, 17) == *(buf + i + 17)) {
				flag = 1;
				break;
			}
		}
	}
	if (flag == 1) {
		/* 假定已经配置单位为公斤, 0位小数位, 数字为毛重 */
		kg = (*(buf + 6) << 24) + (*(buf + 7) << 16) + (*(buf + 8) << 8) + *(buf + 9);
	}

	return kg;
}

#endif
