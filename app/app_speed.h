#ifndef __APP_SPEED_H__
#define __APP_SPEED_H__

#include <stdint.h>
#include <stdbool.h>

struct g_speed_info_ {
	uint8_t speed_curr;
	uint32_t VSC;			/*���ٴ���ϵ��r/km*/
	uint32_t total_km;	/*�����*/
	uint32_t total_r;		/*����1KM��������*/
};
extern struct g_speed_info_ g_speed_info;

void app_speed_init(void);
void app_speed_read_raw(uint8_t val);
void app_speed_calc(void);

#endif
