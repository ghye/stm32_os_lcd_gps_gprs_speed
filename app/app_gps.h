#ifndef __APP_GPS_H__
#define __APP_GPS_H__

#include "public.h"
#include "proto.h"

extern struct gprmc_ now_gps;	/* ���µ�gps��Ϣ */

struct gprmc_ *app_gps_gprmc_ft_read(void);
struct gprmc_ *app_gps_gprmc_lasted_read(int8_t *flag);
void app_gps_gprmc_save(struct gprmc_ * gprmc);
void app_gps_update_now_gps(struct gprmc_ * gprmc);
void app_gps_init(void);
void app_gps_parse(void);
void app_gps_rbuf_hander(uint8_t val);

#endif

