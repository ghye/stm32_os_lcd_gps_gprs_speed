#include "public.h"
#include "driv_red_green.h"
#include "app_red_green.h"

#if defined(HC_CONTROLER_)

void app_red_green_proc(char dev, enum lightstate s)
{
	switch (s) {
		case RED_OPEN:
			if (0 == dev)
				driv_red_green1_ropen();
			else if (1 == dev)
				driv_red_green2_ropen();
			break;
		case RED_CLOSE:
			if (0 == dev)
				driv_red_green1_rclose();
			else if (1 == dev)
				driv_red_green2_rclose();
			break;
		case GREEN_OPEN:
			if (0 == dev)
				driv_red_green1_gopen();
			else if (1 == dev)
				driv_red_green2_gopen();
			break;
		case GREEN_CLOSE:
			if (0 == dev)
				driv_red_green1_gclose();
			else if (1 == dev)
				driv_red_green2_gclose();
			break;
		default:
			break;
	}
}

void app_red_green_init(void)
{
	driv_red_green1_init();
	driv_red_green2_init();
}

#endif
