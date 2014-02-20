#ifndef __DRIV_WATCHDOG_H__
#define __DRIV_WATCHDOG_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

void driv_wdg_init(void);
void driv_wdg_start(void);
void driv_wdg_keeplive(void);

#endif
