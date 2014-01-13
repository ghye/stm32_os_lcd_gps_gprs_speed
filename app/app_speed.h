#ifndef __APP_SPEED_H__
#define __APP_SPEED_H__

#include <stdint.h>
#include <stdbool.h>

struct g_speed_info_ {
	uint8_t speed_curr;
	uint32_t VSC;			/*车速传递系数r/km*/
	uint32_t total_km;	/*总里程*/
	uint32_t total_r;		/*不足1KM的脉冲数*/
};
extern struct g_speed_info_ g_speed_info;

void app_speed_init(void);
void app_speed_read_raw(uint8_t val);
void app_speed_calc(void);

#endif
