#ifndef __APP_VOICE_H__
#define __APP_VOICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

void app_voice_data_insert(uint8_t *buf, uint16_t len);
int32_t app_voice_data_read(void);
void app_voice_init(void);
void app_voice_play(void);

#endif
