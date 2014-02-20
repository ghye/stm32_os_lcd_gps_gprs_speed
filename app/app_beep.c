#include "driv_beep.h"
#include "app_beep.h"

#if defined (CAR_DB44_V1_0_20130315_)

void app_beep_init(void)
{
	driv_beep_init();
}

void app_beep_open(void)
{
	driv_beep_open();
}

void app_beep_close(void)
{
	driv_beep_close();
}

#endif
