#ifndef __DRIV_GPRS_H__
#define __DRIV_GPRS_H__

#include "public.h"

void driv_gprs_rx_it_disable(void);
void driv_gprs_rx_it_enable(void);
void driv_gprs_send_msg(void *msg, uint32_t len);
void driv_gprs_power_enable(void);
void driv_gprs_power_disable(void);
void driv_gprs_reset(void);
void driv_gprs_norm(void);
void driv_gprs_mon_reset(void);
void driv_gprs_mon_norm(void);
void driv_gprs_init(void);

#endif

