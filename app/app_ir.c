#include "public.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "driv_systick.h"
#include "alg_loopback_buf.h"
#include "driv_ir.h"
#include "app_ir.h"

#if defined(HC_CONTROLER_)

#define IR_LEN		128
#define IR_ISR_LEN	64

#define IR_TIMER		2	/* 秒 */

static int head1;
static int tail1;
static int head1_isr;
static int tail1_isr;
static char ir1[IR_LEN];
static char ir1_isr[IR_ISR_LEN];
static int head2;
static int tail2;
static int head2_isr;
static int tail2_isr;
static char ir2[IR_LEN];
static char ir2_isr[IR_ISR_LEN];

void app_ir_init(void)
{
	driv_ir1_init();
	driv_ir2_init();
	alg_lbb_init(&tail1, &head1);
	alg_lbb_init(&tail2, &head2);
	alg_lbb_init(&tail1_isr, &head1_isr);
	alg_lbb_init(&tail2_isr, &head2_isr);
}

void app_ir_proc(char *buf, int *len, int max)
{
	#define IR_BE_BRAKED_LEVEL	1
	#define MSG_REPORT_IR		"T:E:%d,%d#"
	
	char c;
	char idx;
	char v;

	*len = 0;
	buf[0] = '\0';
	
	app_ir1_isr_proc();
	app_ir2_isr_proc();
	app_ir1_timer_proc();
	app_ir2_timer_proc();

	if (1 == alg_lbb_get_char(ir1, IR_LEN, tail1, &head1, &c)) {
		if (IR_BE_BRAKED_LEVEL == c)	/* 被遮挡的电平 */ {
			v = 1;
		} else {
			v = 0;
		}
		idx = 0;
		sprintf(buf + strlen(buf), MSG_REPORT_IR, idx, v);
	}

	if (1 == alg_lbb_get_char(ir2, IR_LEN, tail2, &head2, &c)) {
		if (IR_BE_BRAKED_LEVEL == c)	/* 被遮挡的电平 */ {
			v = 1;
		} else {
			v = 0;
		}
		idx = 1;
		sprintf(buf + strlen(buf), MSG_REPORT_IR, idx, v);
	}

	*len += strlen(buf);
}

void app_ir1_isr(unsigned char level)
{
	char lv;
	
	if (0 == level) {
		lv = 0;
	} else {
		lv = 1;
	}
	alg_lbb_insert(ir1_isr, IR_ISR_LEN, &tail1_isr, lv);
}

void app_ir2_isr(unsigned char level)
{
	char lv;
	
	if (0 == level) {
		lv = 0;
	} else {
		lv = 1;
	}
	alg_lbb_insert(ir2_isr, IR_ISR_LEN, &tail2_isr, lv);
}

void app_ir1_timer_proc(void)
{
	static uint64_t lticks = 0;
	char lv;

	if (ticks >= lticks + IR_TIMER * HZ) {
		if (driv_ir1_iolevel())
			lv = 1;
		else 
			lv = 0;
		alg_lbb_insert(ir1, IR_LEN, &tail1, lv);
		lticks += IR_TIMER * HZ;
	}
}

void app_ir2_timer_proc(void)
{
	static uint64_t lticks = 0;
	char lv;

	if (ticks >= lticks + IR_TIMER * HZ) {
		if (driv_ir2_iolevel())
			lv = 1;
		else 
			lv = 0;
		alg_lbb_insert(ir2, IR_LEN, &tail2, lv);
		lticks += IR_TIMER * HZ;
	}
}

void app_ir1_isr_proc(void)
{
	char c;

	if (1 == alg_lbb_get_char(ir1_isr, IR_ISR_LEN, tail1_isr, &head1_isr, &c)) {
		alg_lbb_insert(ir1, IR_LEN, &tail1, c);
	}
}

void app_ir2_isr_proc(void)
{
	char c;

	if (1 == alg_lbb_get_char(ir2_isr, IR_ISR_LEN, tail2_isr, &head2_isr, &c)) {
		alg_lbb_insert(ir2, IR_LEN, &tail2, c);
	}
}

#endif
