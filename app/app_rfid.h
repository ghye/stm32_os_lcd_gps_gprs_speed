#ifndef __APP_RFID_H__
#define __APP_RFID_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

extern uint32_t g_rfid_serial_num;

void app_rfid_init(void);
void app_rfid_proc(void);

#endif

