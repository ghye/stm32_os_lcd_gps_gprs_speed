#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "driv_systick.h"
#include "app_usart.h"
#include "driv_cpu_id.h"

#if defined(HC_CONTROLER_)

#define SEND_CPU_ID_TIMER	5	/* √Î */

void app_cpu_id_init(void)
{
	driv_cpu_id_init();
}

void app_cpu_id_to_str(char *str)
{
	int i;
	unsigned int *p;

	driv_cpu_id_get(&p);

	for (i = 0; i < 3; i++)
		sprintf(str + 8 * i, "%.8X", *(p + i));
}

void app_cpu_id_proc(char *buf, int *len, int max)
{
	static uint64_t lticks = 0;

	if (ticks >= lticks + SEND_CPU_ID_TIMER * HZ) {
		sprintf(buf, "%s", "T:N:");
		app_cpu_id_to_str(buf + strlen(buf));
		sprintf(buf + strlen(buf), "%s", "#");
		*len = strlen(buf);
		lticks += SEND_CPU_ID_TIMER * HZ;

		/*∑¢ÀÕ ±º‰¥¡*/
		sprintf(buf + strlen(buf), "T:T:%llu#", ticks / HZ);
		*len = strlen(buf);
	} else {
		*len = 0;
	}


	//com_send_message(USART_GPS_NUM, (void *)cpuid);
}

#endif
