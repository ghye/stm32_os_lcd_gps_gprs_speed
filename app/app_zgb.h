#ifndef __APP_ZGB_H__
#define __APP_ZGB_H__

#include <stdint.h>
#include <stdbool.h>

void app_zgb_init(void);
void app_zgb_proc(char *buf, int *len, int maxlen);
void app_zgb2_init(void);
void app_zgb2_proc(char *buf, int *len, int maxlen);

#endif
