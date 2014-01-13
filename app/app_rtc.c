
#include "app_rtc.h"

Calendar_Def calendar;

void app_rtc_init(void)
{
	driv_rtc_init();
}

void app_rtc_get_time(Calendar_Def *ptimenow)
{
	driv_rtc_get_time(ptimenow);
}

void app_rtc_set_time(Calendar_Def *ptimenow)
{
	driv_rtc_set_time(ptimenow);
}

void app_rtc_process(void)
{
	driv_rtc_get_time(&calendar);
}

