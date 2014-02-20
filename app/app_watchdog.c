#include "core.h"
#include "driv_systick.h"
#include "driv_watchdog.h"
#include "app_watchdog.h"

#define MAX_TASKS	4

struct app_wdg_strut_ {
	uint64_t task_tick[MAX_TASKS];
};

struct app_wdg_strut_ app_wdg_strut;

static int32_t app_wdg_check_all_task_ok(void);


void app_wdg_init(void)
{
	uint32_t i;
	
	driv_wdg_init();

	for (i = 0; i < MAX_TASKS; i++)
		app_wdg_strut.task_tick[i] = 0;
}

void app_wdg_start(void)
{
	driv_wdg_start();
}

void app_wdg_keeplibe(void)
{
	if (app_wdg_check_all_task_ok() == 1)
		driv_wdg_keeplive();
}

void app_wdg_update_task_tick(uint32_t task_id)
{
	if (task_id < MAX_TASKS)
		app_wdg_strut.task_tick[task_id] = ticks;
}

static int32_t app_wdg_check_all_task_ok(void)
{
	uint32_t i;
	int32_t r = 1;

	for (i = 0; i < MAX_TASKS; i++) {
		if (app_wdg_strut.task_tick[i] + 20 * HZ < ticks)
			r = 0;
	}

	return r;
}
