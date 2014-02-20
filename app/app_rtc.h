#ifndef __APP_RTC_H__
#define __APP_RTC_H__

#include "public.h"
#include "driv_rtc.h"

extern Calendar_Def calendar;

void app_rtc_init(void);
void app_rtc_get_time(Calendar_Def *ptimenow);
void app_rtc_set_time(Calendar_Def *ptimenow);
void app_rtc_process(void);

#endif
