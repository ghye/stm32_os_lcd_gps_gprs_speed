#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "proto.h"

#include "core.h"
#include "driv_gps.h"
#include "driv_systick.h"
#include "app_gps.h"
#include "app_nmea.h"

#define GPS_MAX_MSG_LEN	90	//128
#define GPS_MAX_ARRY 		40	//20

struct gps_info_ {
	struct {
		uint8_t rbuf[GPS_MAX_ARRY][GPS_MAX_MSG_LEN];
		uint8_t rbuf_arry_head;
		uint8_t rbuf_arry_tail;
		uint16_t rbuf_index[GPS_MAX_ARRY];
	}rbuf_info;
};
struct gps_info_ gps_info;

#define FIXED_TIME_RMC_MAX	5
#define SAVE_TIME_RMC_MAX		5
struct gprmcs_record_ {
	bool gprmc_first;// = false;	/*捕捉第一个GPRMC语句标志*/
	uint8_t gprmc_std_sec;		/*第一个GPRMC语句的秒数*/
	uint8_t head_ft;
	uint8_t tail_ft;
	uint8_t head_s;
	uint8_t tail_s;

	/*
		因为GPS、GPRS分别属于两个任务，所以不能只有一个gprmcs_lasted。
		可以通过指针的方式，因为对于STM32来说32位的操作的原子的。
	*/
	struct gprmc_ *pgprmcs_lasted;
	struct gprmc_ gprmcs_lasted[2];
	
	struct gprmc_ gprmcs_ft[FIXED_TIME_RMC_MAX];
	struct gprmc_ gprmcs_s[SAVE_TIME_RMC_MAX];
};
struct gprmcs_record_ gprmcs_record;

static uint8_t gpsbuf[GPS_MAX_MSG_LEN];

#define GPS_STATUS_TIMEOUT_S	40
static uint64_t gps_err_recoder = 0;

#if defined(CAR_DB44_V1_0_20130315_)
static uint16_t GPS_INT_S = 2;//30;
#elif defined(DouLunJi_CAR_GBC_V1_2_130511_)
static uint16_t GPS_INT_S = 30;
#endif

static uint64_t gps_next_ticks;

static void app_gps_err_recoder_reset(void)
{
	gps_err_recoder = GPS_STATUS_TIMEOUT_S*HZ + ticks;
}

static bool app_gps_err_recoder_read(void)
{
	return (gps_err_recoder < ticks) ? true:false;	
}

static void app_gps_rbuf_head_clean(void)
{
	memset(gps_info.rbuf_info.rbuf[gps_info.rbuf_info.rbuf_arry_head], 0, GPS_MAX_MSG_LEN);
	gps_info.rbuf_info.rbuf_index[gps_info.rbuf_info.rbuf_arry_head++] = 0;
	if(gps_info.rbuf_info.rbuf_arry_head >= GPS_MAX_ARRY)
		gps_info.rbuf_info.rbuf_arry_head = 0;


/*	gps_info.rbuf_info.rbuf_index = 0;
	memset(gps_info.rbuf_info.rbuf, 0, sizeof(gps_info.rbuf_info.rbuf)/sizeof(gps_info.rbuf_info.rbuf[0]));*/
}

static int32_t app_gps_rbuf_head_read(uint8_t *buf)
{
	int32_t ret = -1;
	
	if(gps_info.rbuf_info.rbuf_arry_head != gps_info.rbuf_info.rbuf_arry_tail) {
		memcpy(buf, gps_info.rbuf_info.rbuf[gps_info.rbuf_info.rbuf_arry_head],
			gps_info.rbuf_info.rbuf_index[gps_info.rbuf_info.rbuf_arry_head]);
		app_gps_rbuf_head_clean();
		ret = 0;
	}
	return ret;
}

static int32_t app_gps_sentence_read(void)
{
	int32_t ret = -1;

	if (app_gps_err_recoder_read()) {
		driv_gps_power_reset();
		app_gps_err_recoder_reset();
	}
	
	memset(gpsbuf, '\0', GPS_MAX_MSG_LEN);
	ret = app_gps_rbuf_head_read(gpsbuf);
	if(0 == ret) {
		app_gps_err_recoder_reset();
	}

	return ret;
}

static bool app_gps_time_isto_ft(struct gprmc_ * gprmc)
{
	/*需要改进*/
	if (gprmc->time.sec == gprmcs_record.gprmc_std_sec)
		return true;
	return false;
}

static struct gprmc_ *app_gps_gprmc_s_read(void)
{
	uint8_t pt;
	
	if (gprmcs_record.head_s == gprmcs_record.tail_s)
		return NULL;
	pt = gprmcs_record.head_s;
	gprmcs_record.head_s++;
	if (gprmcs_record.head_s >= FIXED_TIME_RMC_MAX)
		gprmcs_record.head_s = 0;
	return &gprmcs_record.gprmcs_s[pt];
}

static void app_gps_gprmc_ft_save(struct gprmc_ * prmc)
{
	if (true == gprmcs_record.gprmc_first) {
		if (app_gps_time_isto_ft(prmc)) {
			memcpy(&gprmcs_record.gprmcs_ft[gprmcs_record.tail_ft], prmc, sizeof(struct gprmc_));
			gprmcs_record.tail_ft++;
			if (gprmcs_record.tail_ft >= FIXED_TIME_RMC_MAX)
				gprmcs_record.tail_ft = 0;

			/*当来不及读取的时候会溢出，这里做循环覆盖*/
			if (gprmcs_record.tail_ft == gprmcs_record.head_ft) {
				gprmcs_record.head_ft++;
				if (gprmcs_record.head_ft >= FIXED_TIME_RMC_MAX)
					gprmcs_record.head_ft = 0;
			}
		}
	}
	else {

	}
}

static void app_gps_gprmc_lasted_save(struct gprmc_ * prmc)
{
	if (gprmcs_record.pgprmcs_lasted == &(gprmcs_record.gprmcs_lasted[0])) {
		memcpy(&gprmcs_record.gprmcs_lasted[1], prmc, sizeof(struct gprmc_));
		gprmcs_record.pgprmcs_lasted = &(gprmcs_record.gprmcs_lasted[1]);
	}
	else {
		memcpy(&gprmcs_record.gprmcs_lasted[0], prmc, sizeof(struct gprmc_));
		gprmcs_record.pgprmcs_lasted = &(gprmcs_record.gprmcs_lasted[0]);
	}
}

static void app_gps_parse_past(void)
{
	struct gprmc_ *prmc;
	
	prmc = app_gps_gprmc_s_read();
	if (NULL == prmc)
		return;


	/*在这里可以做处理每个GPRMC信息*/


	app_gps_gprmc_ft_save(prmc);
	app_gps_gprmc_lasted_save(prmc);
}

struct gprmc_ *app_gps_gprmc_ft_read(void)
{
	uint8_t pt;
	
	if (gprmcs_record.head_ft == gprmcs_record.tail_ft)
		return NULL;
	pt = gprmcs_record.head_ft;
	gprmcs_record.head_ft++;
	if (gprmcs_record.head_ft >= FIXED_TIME_RMC_MAX)
		gprmcs_record.head_ft = 0;
	return &gprmcs_record.gprmcs_ft[pt];
}

struct gprmc_ *app_gps_gprmc_lasted_read(int8_t *flag)
{
	struct gprmc_ *plasted;
	
	if (gps_next_ticks > ticks ) {
		*flag = -1;
		return NULL;
	}

	plasted = gprmcs_record.pgprmcs_lasted;
	if (STATUS_FIX != plasted->status) {
		*flag = -2;
		gps_next_ticks = ticks + GPS_INT_S*HZ;
		return NULL;
	}

	gps_next_ticks = ticks + GPS_INT_S*HZ;
	
	return plasted;
}

void app_gps_gprmc_save(struct gprmc_ * gprmc)
{
	/*间隔上传的时间基准点*/
	if (false == gprmcs_record.gprmc_first) {
		gprmcs_record.gprmc_std_sec = gprmc->time.sec;
		gprmcs_record.gprmc_first = true;
	}

	memcpy(&gprmcs_record.gprmcs_s[gprmcs_record.tail_s], gprmc, sizeof(struct gprmc_));
	gprmcs_record.tail_s++;
	if (gprmcs_record.tail_s >= FIXED_TIME_RMC_MAX)
		gprmcs_record.tail_s = 0;

	/*当来不及读取的时候会溢出，这里做循环覆盖*/
	if (gprmcs_record.tail_s == gprmcs_record.head_s) {
		gprmcs_record.head_s++;
		if (gprmcs_record.head_s >= FIXED_TIME_RMC_MAX)
			gprmcs_record.head_s = 0;
	}
}

void app_gps_init(void)
{
	driv_gps_init();

	app_gps_err_recoder_reset();
	
	memset(&gps_info, 0, sizeof(gps_info));
	memset(&gprmcs_record, 0, sizeof(gprmcs_record));
	gprmcs_record.gprmc_first = false;
	gps_next_ticks= ticks + GPS_INT_S*HZ;
	gprmcs_record.pgprmcs_lasted = &(gprmcs_record.gprmcs_lasted[0]);
}

void app_gps_parse(void)
{
	uint8_t cnt = 0;
	
	while (app_gps_sentence_read() == 0) {
		if (1 == app_nmea_parse(gpsbuf))
			break;
		if (cnt++ > 40)
			break;
	}
	
	app_gps_parse_past();
}

static uint8_t irq_data;
void app_gps_rbuf_hander(uint8_t val)
{
	if(val == '$')
	{
		irq_data = gps_info.rbuf_info.rbuf_arry_tail;
		++ gps_info.rbuf_info.rbuf_arry_tail;
		if(gps_info.rbuf_info.rbuf_arry_tail >= GPS_MAX_ARRY)
			gps_info.rbuf_info.rbuf_arry_tail = 0;	
		if(gps_info.rbuf_info.rbuf_arry_tail == gps_info.rbuf_info.rbuf_arry_head)
		{
			gps_info.rbuf_info.rbuf_arry_tail = irq_data;
			gps_info.rbuf_info.rbuf_index[gps_info.rbuf_info.rbuf_arry_tail] = 0;
		}
	}
	gps_info.rbuf_info.rbuf[gps_info.rbuf_info.rbuf_arry_tail][gps_info.rbuf_info.rbuf_index[gps_info.rbuf_info.rbuf_arry_tail]] = val;		
	if (gps_info.rbuf_info.rbuf_index[gps_info.rbuf_info.rbuf_arry_tail] < GPS_MAX_MSG_LEN -1)
		gps_info.rbuf_info.rbuf_index[gps_info.rbuf_info.rbuf_arry_tail]++;
}
