#ifndef __DRIV_RTC_H__
#define __DRIV_RTC_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

#define START_YEAR	2010	//default 2010, Jan.1, 00:00:00
#define SEC_IN_DAY	86400	// 1 day includes 86400 seconds

typedef struct {       /* date and time components */
	int16_t sec;    //senconds after the minute, 0 to 59
	int16_t min;    //minutes after the hour, 0 to 59
	int16_t hour;   //hours since midnight, 0 to 23
	int16_t mday;   //day of the month, 1 to 31
	int16_t month;  //months of the year, 1 to 12
	int16_t year;   //years, START_YEAR to START_YEAR+135
	int16_t wday;   //days since Sunday, 0 to 6
	int16_t yday;   //days of the year, 1 to 366
}Calendar_Def;

void driv_rtc_init(void);
void driv_rtc_get_time(Calendar_Def *pTimeNow);
void driv_rtc_set_time(Calendar_Def *pTimeNow);

#endif
