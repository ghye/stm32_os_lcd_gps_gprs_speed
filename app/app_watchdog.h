#ifndef __APP_WATCHDOG_H__
#define __APP_WATCHDOG_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

void app_wdg_init(void);
void app_wdg_start(void);
void app_wdg_keeplibe(void);
void app_wdg_update_task_tick(uint32_t task_id);

#endif
