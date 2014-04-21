#include "public.h"

#if defined(CAR_TRUCK_1_5_140325_)

#include "core.h"
#include "driv_systick.h"
#include "driv_vc0706.h"
#include "app_vc0706.h"

#define DRIV_VC0706_PHOTOGRAPH_TIME			( 60 * 10 )	/* s */
#define DRIV_VC0706_PHOTOGRAPH_BOOT_TIME	( 30 )		/* s */
	
enum app_vc0706_msg_ {
	AV_MSG_IDLY	= 0,
	AV_MSG_HAVE_DATA,
	AV_MSG_EMPTY_DATA
};

static enum app_vc0706_msg_ av_msg = AV_MSG_IDLY;

static uint64_t dvi_tick = 0;		/* 拍照间隔时间 */

static void app_vc0706_msg_init(void)
{
	av_msg = AV_MSG_IDLY;
}

static bool app_vc0706_msg_is_idly(void)
{
	if (av_msg == AV_MSG_IDLY)
		return true;
	return false;
}

static bool app_vc0706_msg_is_have_data(void)
{
	if (av_msg == AV_MSG_HAVE_DATA)
		return true;
	return false;
}

static bool app_vc0706_msg_is_empty_data(void)
{
	if (av_msg == AV_MSG_EMPTY_DATA)
		return true;
	return false;
}

static void app_vc0706_msg_set_idly(void)
{
	av_msg = AV_MSG_IDLY;
}

static void app_vc0706_msg_set_have_data(void)
{
	av_msg = AV_MSG_HAVE_DATA;
}

static void app_vc0706_msg_set_empty_data(void)
{
	av_msg = AV_MSG_EMPTY_DATA;
}

static void app_vc0706_update_time_to_photograph(void)
{
	dvi_tick += DRIV_VC0706_PHOTOGRAPH_TIME * HZ;
}

static bool app_vc0706_is_time_to_photograph(void)
{
	static bool av_sys_boot = true;	/* 用于系统启动时及时拍第一张照片 */

	if (av_sys_boot) {
		if (DRIV_VC0706_PHOTOGRAPH_BOOT_TIME * HZ > ticks) {
			return false;
		} else {
			av_sys_boot = false;
			return true;
		}
	}
	
	if (dvi_tick + DRIV_VC0706_PHOTOGRAPH_TIME * HZ > ticks) {
		return false;
	}
	app_vc0706_update_time_to_photograph();
	return true;
}

/*防止当摄像头有问题时不间断地开启IR导致摄像头大量发热*/
static bool app_vc0706_is_time_to_open_ir(void)
{
	#define APP_VC0706_AVOID_IR_TOO_HOLT_TIME	30	/* s */

	static uint64_t av_ir_holt_tick = 0;

	if (av_ir_holt_tick + APP_VC0706_AVOID_IR_TOO_HOLT_TIME * HZ > ticks)
		return false;
	
	av_ir_holt_tick = ticks;
	return true;
}

void app_vc0706_init(void)
{
	driv_vc0706_nos_init();
	app_vc0706_msg_init();
}

/*
buf: 返回发送首地址
len: 返回可发送字节数
glen: 上层申请的最大字节数
head: 返回driv_vc0706底层缓存的head值
*/
void app_vc0706_frame(unsigned char **buf, int *len, int glen, int *head)
{
	if (!app_vc0706_msg_is_have_data())
		return;

	driv_vc0706_get_data(buf, len, glen, head);
	if (*len > 0) {
		
	} else {
		//driv_vc0706_os_init();
		app_vc0706_msg_set_empty_data();
	}
}

void app_vc0706_prepared_data(void)
{
	if (app_vc0706_msg_is_have_data())
		return;
	if (!app_vc0706_is_time_to_photograph())
		return;
	if (driv_vc0706_os_init()) {
		app_vc0706_msg_set_have_data();
	}
}

unsigned int app_vc0706_frame_len(void)
{
	return driv_vc0706_frame_len();
}

#endif