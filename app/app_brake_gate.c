#include "public.h"
#include "driv_red_green.h"
#include "app_brake_gate.h"

#if defined(HC_CONTROLER_)

void app_brake_gate_proc(char dev, enum gatestate s)
{
	switch (s) {
	case GATE_UP:
		if (0 == dev)
			driv_red_green1_gopen();
		else if (1 == dev)
			driv_red_green2_gopen();
		break;
	case GATE_DOWN:
		if (0 == dev)
			driv_red_green1_ropen();
		else if (1 == dev)
			driv_red_green2_ropen();
		break;
	default:
		break;
	}
}

#endif
