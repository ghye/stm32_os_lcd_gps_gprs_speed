#include <string.h>

#include "core.h"
#include "driv_systick.h"

#include "driv_speed.h"
#include "app_speed.h"
#include "app_rtc.h"

#if defined (CAR_DB44_V1_0_20130315_)

#define SPEED_MAX	140
#define VSC_DEF	66666

struct g_speed_info_ g_speed_info;

struct speed_st_ {
	uint16_t speed;
	//uint64_t tick;
};

struct speed_info_ {
	uint8_t head;
	uint8_t tail;
	struct speed_st_ speed[SPEED_MAX];
};
struct speed_info_ speed_info;

/*返回的是脉冲数*/
#define USE_INT	5	/*使用几个时隔的数据算速度，必须5的倍数*/
static uint32_t app_speed_curr_r(void)
{
	uint8_t i;
	uint8_t ltail;
	uint32_t r;
	
	ltail = speed_info.tail;

	r = 0;
	for (i=0; ltail && i<USE_INT; ltail--, i++) {
		r += speed_info.speed[ltail-1].speed;
	}
	if (i<USE_INT) {
		ltail = SPEED_MAX;
		for (; ltail && i<USE_INT; ltail--, i++) {
			r += speed_info.speed[ltail-1].speed;
		}
	}

	return r;
}

static uint8_t app_speed_curr(void)
{
	uint32_t sp;

	sp = app_speed_curr_r();
	sp = sp * 3600/ (USE_INT/5) /g_speed_info.VSC;

	return sp;
}

/*用于读取已收到的速度数据*/
static uint32_t app_speed_read_speed_data(void)
{
	uint16_t h, t, i;
	uint32_t r;

	h = speed_info.head;
	t = speed_info.tail;
	r = 0;
	if (h == t)
		return r;

	speed_info.head = t;
	if (h < t) {
		for (i=h; i<t; i++) {
			r += speed_info.speed[i].speed;
		}
		return r;
	}
	
	for (i=h; i<SPEED_MAX; i++) {
		r += speed_info.speed[i].speed;
	}
	for (i=0; i<t; i++) {
		r += speed_info.speed[i].speed;
	}
	return r;
}

static bool app_speed_if_stop(void)
{
	#define STOP_R	1	/*DB44规定一秒内大于1个脉冲或大于3km/h才是非停车*/

	//if (g_speed_info.VSC*3/3600 > 1)
	if (g_speed_info.VSC/1200 >= 1) {
		if (app_speed_curr_r() > g_speed_info.VSC/1200) 
			return false;
	}
	else if (app_speed_curr_r() > STOP_R) {
		return false;
	}
	return true;
}

/*停车前20s内的速度*/
static void app_speed_get_speed_20s(uint16_t *buf)
{
	uint8_t i, j;
	uint8_t ltail;
	uint32_t r;
	
	ltail = speed_info.tail;

	/*当前是０速度，跳过之*/
	for (i=0; i<USE_INT; i++) {
		if (ltail)
			ltail--;
		else
			ltail = SPEED_MAX-1;
	}

	#if 0
	for (j=0; j<20; j++) {
		r = 0;
		for (i=0; ltail && i<USE_INT; ltail--, i++) {
			r += speed_info.speed[ltail-1].speed;
		}
		if (i<USE_INT) {
			ltail = SPEED_MAX;
			for (; ltail && i<USE_INT; ltail--, i++) {
				r += speed_info.speed[ltail-1].speed;
			}
		}
		buf[j] = r;
	}
	
	#else
	if (0 == ltail)
		ltail = SPEED_MAX - 1;
	else 
		ltail--;
	for (j=0; j<100; j++) {
		buf[j] = speed_info.speed[ltail].speed;
		if (0 == ltail)
			ltail = SPEED_MAX - 1;
		else
			ltail--;
	}
	#endif
}

static void app_speed_stop_record(void)
{
	/*1. 先获得日历时间*/
	/*2. 存储车速、车辆制动状态信号和时间*/

	static Calendar_Def scalendar;
	static uint16_t buf[100];
	uint8_t i;

	scalendar = calendar;
	app_speed_get_speed_20s(buf);
	#if 0
	for (i=0; i<20; i++) {
		buf[i] = buf[i] * 3600/ (USE_INT/5) /g_speed_info.VSC;
	}

	#else
	for (i=0; i<100; i++) {
		buf[i] = buf[i] * 3600 *USE_INT /g_speed_info.VSC;
	}
	#endif
}

/*
	记录停车前20s内的车辆状态
*/
static void app_speed_stop_status_record(void)
{
	enum vehicle_s_ {
		V_STOP_BEFORE = 1,	/*车辆已经停止多时*/
		V_MOVE_LT_5S,		/*车辆开动小于5s*/
		V_MOVE_BE_5S,		/*车辆开动已超过5s*/
		V_STOP_JUST_NOW,	/*车辆现在才停车*/
	};
	static enum vehicle_s_ vehicle_s = V_STOP_BEFORE;
	static uint64_t t = 0;

	bool stop;

	stop = app_speed_if_stop();
	
	switch (vehicle_s) {
		case V_STOP_BEFORE:
			if (stop) {

			}
			else {
				t = ticks;
				vehicle_s = V_MOVE_LT_5S;
			}
			break;
		case V_MOVE_LT_5S:
			if (stop) {
				vehicle_s = V_STOP_BEFORE;
			}
			else {
				if (ticks >= t+HZ*5)
					vehicle_s = V_MOVE_BE_5S;
			}
			break;
		case V_MOVE_BE_5S:
			if (stop) {
				vehicle_s = V_STOP_JUST_NOW;
			}
			else {
				
			}
			break;
		case V_STOP_JUST_NOW:
			app_speed_stop_record();
			vehicle_s = V_STOP_BEFORE;
			break;
		default:
			break;
	}
}

static void app_speed_per_min(void)
{
	static uint8_t point_h = 0;
	static uint16_t cnt = 0;
	static uint32_t sr = 0;
	
	uint8_t i;
	uint8_t ltail;

	ltail = speed_info.tail;
	
	for (; (cnt < 60 * USE_INT) && (point_h != ltail); cnt++) {
		sr += speed_info.speed[point_h].speed;
		point_h++;
		if (point_h >= SPEED_MAX)
			point_h = 0;
	}
	if (cnt >= 60 * USE_INT) {
		/*计算和保存每分钟的速度*/
		sr = sr * 60 / g_speed_info.VSC;
		
		sr = 0;
		cnt = 0;
	}
}

void app_speed_init(void)
{
	driv_speed_init();

	memset((void *)&speed_info, 0, sizeof(struct speed_info_));

	g_speed_info.speed_curr = 0;
	
	/*应该在这里读取位于存储卡上的备份数据*/
	g_speed_info.total_km = 0;
	g_speed_info.total_r = 0;
	g_speed_info.VSC = VSC_DEF;
};

void app_speed_read_raw(uint8_t val)
{
	static bool sbit = true;
	static uint16_t sp = 0;

	if (true == sbit) {
		if (val & 0x80)
			return;
	}
	
	sp <<= 8;
	sp |= val;
	sbit = !sbit;
	if (true == sbit) {
		sp = ((sp >> 1) & 0x3F80) /*0b00111111 10000000*/ | (sp & 0x7F);
		speed_info.speed[speed_info.tail].speed = sp;
		speed_info.tail++;
		if (speed_info.tail >= SPEED_MAX)
			speed_info.tail = 0;
	}
}

void app_speed_calc(void)
{
	g_speed_info.speed_curr = app_speed_curr();

	/*记录停车前的信息*/
	app_speed_stop_status_record();

	/*记录每分钟的车速*/
	app_speed_per_min();

	g_speed_info.total_r += app_speed_read_speed_data();
	g_speed_info.total_km += g_speed_info.total_r/g_speed_info.VSC;
	g_speed_info.total_r %= g_speed_info.VSC;
}

#endif
