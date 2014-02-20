#ifndef __DRIV_VOICE_H__
#define __DRIV_VOICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

enum e_otp_voice_pluses {
	E_OVP_EMPTY	=	1,
	E_OVP_1,
	E_OVP_2,
	E_OVP_3,
	E_OVP_4,
	E_OVP_5,
	E_OVP_6,
	E_OVP_7,
	E_OVP_8,
	E_OVP_9,
	E_OVP_10,
	E_OVP_100,
	E_OVP_TIMEOUT,
	E_OVP_CAREFOR,
	E_OVP_EXCEEDSPEED,
	E_OVP_KMPH,
	E_OVP_CONFIRM,
	E_OVP_THK,
	E_OVP_HPDOCK,
	E_OVP_ZWDOCK,
	E_OVP_XGDOCK,
	E_OVP_JLDOCK,
	E_OVP_INTO,
	E_OVP_OUTTO,
	E_OVP_QUEUE,
	E_OVP_ORDER,
	E_OVP_WORKING,
	E_OVP_NEWWEATHER,
	E_OVP_NEWCONGESTION,
	E_OVP_NEWDISPATCH,
	E_OVP_NEWTRAFFIC,
	E_OVP_NEWORDER
};

void otp_voice_init(void);
void driv_voice_play(enum e_otp_voice_pluses w);

#endif
