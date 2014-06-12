#include "public.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "driv_systick.h"
#include "alg_loopback_buf.h"
#include "driv_coil.h"
#include "app_coil.h"

#ifdef HC_CONTROLER_

#define COIL_LEN			128
#define COIL_ISR_LEN	64

#define COIL_TIMER		2	/* 秒 */

static int head1;
static int head2;
static int head3;
static int head4;
static int tail1;
static int tail2;
static int tail3;
static int tail4;
static int head1_isr;
static int head2_isr;
static int head3_isr;
static int head4_isr;
static int tail1_isr;
static int tail2_isr;
static int tail3_isr;
static int tail4_isr;
static char coil1_isr[COIL_ISR_LEN];
static char coil2_isr[COIL_ISR_LEN];
static char coil3_isr[COIL_ISR_LEN];
static char coil4_isr[COIL_ISR_LEN];
static char coil1[COIL_LEN];
static char coil2[COIL_LEN];
static char coil3[COIL_LEN];
static char coil4[COIL_LEN];

void app_coil_init(void)
{
	driv_coil1_init();
	driv_coil2_init();
	driv_coil3_init();
	driv_coil4_init();

	alg_lbb_init(&tail1, &head1);
	alg_lbb_init(&tail2, &head2);
	alg_lbb_init(&tail3, &head4);
	alg_lbb_init(&tail4, &head4);
	alg_lbb_init(&tail1_isr, &head1_isr);
	alg_lbb_init(&tail2_isr, &head2_isr);
	alg_lbb_init(&tail3_isr, &head3_isr);
	alg_lbb_init(&tail4_isr, &head4_isr);
}

void app_coil_proc(char *buf, int *len, int max)
{
	#define COIL_BE_BRAKED_LEVEL	0
	#define MSG_REPORT_COIL		"T:K:%d,%d#"
	
	char c;
	char idx;
	char v;

	*len = 0;
	buf[0] = '\0';
	
	app_coil1_isr_proc();
	app_coil2_isr_proc();
	app_coil3_isr_proc();
	app_coil4_isr_proc();
	app_coil1_timer_proc();
	app_coil2_timer_proc();
	app_coil3_timer_proc();
	app_coil4_timer_proc();

	if (1 == alg_lbb_get_char(coil1, COIL_LEN, tail1, &head1, &c)) {
		if (COIL_BE_BRAKED_LEVEL == c)	/* 被遮挡的电平 */ {
			v = 1;
		} else {
			v = 0;
		}
		idx = 0;
		sprintf(buf + strlen(buf), MSG_REPORT_COIL, idx, v);
	}

	if (1 == alg_lbb_get_char(coil2, COIL_LEN, tail2, &head2, &c)) {
		if (COIL_BE_BRAKED_LEVEL == c)	/* 被遮挡的电平 */ {
			v = 1;
		} else {
			v = 0;
		}
		idx = 1;
		sprintf(buf + strlen(buf), MSG_REPORT_COIL, idx, v);
	}

	if (1 == alg_lbb_get_char(coil3, COIL_LEN, tail3, &head3, &c)) {
		if (COIL_BE_BRAKED_LEVEL == c)	/* 被遮挡的电平 */ {
			v = 1;
		} else {
			v = 0;
		}
		idx = 2;
		sprintf(buf + strlen(buf), MSG_REPORT_COIL, idx, v);
	}

	if (1 == alg_lbb_get_char(coil4, COIL_LEN, tail4, &head4, &c)) {
		if (COIL_BE_BRAKED_LEVEL == c)	/* 被遮挡的电平 */ {
			v = 1;
		} else {
			v = 0;
		}
		idx = 3;
		sprintf(buf + strlen(buf), MSG_REPORT_COIL, idx, v);
	}

	*len += strlen(buf);
}

void app_coil1_isr(unsigned char level)
{
	char lv;
	
	if (0 == level) {
		lv = 0;
	} else {
		lv = 1;
	}
	alg_lbb_insert(coil1_isr, COIL_ISR_LEN, &tail1_isr, lv);
}

void app_coil2_isr(unsigned char level)
{
	char lv;
	
	if (0 == level) {
		lv = 0;
	} else {
		lv = 1;
	}
	alg_lbb_insert(coil2_isr, COIL_ISR_LEN, &tail2_isr, lv);
}

void app_coil3_isr(unsigned char level)
{
	char lv;
	
	if (0 == level) {
		lv = 0;
	} else {
		lv = 1;
	}
	alg_lbb_insert(coil3_isr, COIL_ISR_LEN, &tail3_isr, lv);
}

void app_coil4_isr(unsigned char level)
{
	char lv;
	
	if (0 == level) {
		lv = 0;
	} else {
		lv = 1;
	}
	alg_lbb_insert(coil4_isr, COIL_ISR_LEN, &tail4_isr, lv);
}

void app_coil1_timer_proc(void)
{
	static uint64_t lticks = 0;
	char lv;

	if (ticks >= lticks + COIL_TIMER * HZ) {
		if (driv_coil1_iolevel())
			lv = 1;
		else 
			lv = 0;
		alg_lbb_insert(coil1, COIL_LEN, &tail1, lv);
		lticks += COIL_TIMER * HZ;
	}
}

void app_coil2_timer_proc(void)
{
	static uint64_t lticks = 0;
	char lv;

	if (ticks >= lticks + COIL_TIMER * HZ) {
		if (driv_coil2_iolevel())
			lv = 1;
		else 
			lv = 0;
		alg_lbb_insert(coil2, COIL_LEN, &tail2, lv);
		lticks += COIL_TIMER * HZ;
	}
}

void app_coil3_timer_proc(void)
{
	static uint64_t lticks = 0;
	char lv;

	if (ticks >= lticks + COIL_TIMER * HZ) {
		if (driv_coil3_iolevel())
			lv = 1;
		else 
			lv = 0;
		alg_lbb_insert(coil3, COIL_LEN, &tail3, lv);
		lticks += COIL_TIMER * HZ;
	}
}

void app_coil4_timer_proc(void)
{
	static uint64_t lticks = 0;
	char lv;

	if (ticks >= lticks + COIL_TIMER * HZ) {
		if (driv_coil4_iolevel())
			lv = 1;
		else 
			lv = 0;
		alg_lbb_insert(coil4, COIL_LEN, &tail4, lv);
		lticks += COIL_TIMER * HZ;
	}
}

void app_coil1_isr_proc(void)
{
	char c;

	if (1 == alg_lbb_get_char(coil1_isr, COIL_ISR_LEN, tail1_isr, &head1_isr, &c)) {
		alg_lbb_insert(coil1, COIL_LEN, &tail1, c);
	}
}

void app_coil2_isr_proc(void)
{
	char c;

	if (1 == alg_lbb_get_char(coil2_isr, COIL_ISR_LEN, tail2_isr, &head2_isr, &c)) {
		alg_lbb_insert(coil2, COIL_LEN, &tail2, c);
	}
}

void app_coil3_isr_proc(void)
{
	char c;

	if (1 == alg_lbb_get_char(coil3_isr, COIL_ISR_LEN, tail3_isr, &head3_isr, &c)) {
		alg_lbb_insert(coil3, COIL_LEN, &tail3, c);
	}
}

void app_coil4_isr_proc(void)
{
	char c;

	if (1 == alg_lbb_get_char(coil4_isr, COIL_ISR_LEN, tail4_isr, &head4_isr, &c)) {
		alg_lbb_insert(coil4, COIL_LEN, &tail4, c);
	}
}

#endif
