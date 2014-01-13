#ifndef __APP_KEY_H__
#define __APP_KEY_H__

#include <stdint.h>
#include <stdbool.h>

#include "driv_key.h"

struct app_key {
	enum key_status status;
	enum key_name name;
};

void app_key_init(void);
uint8_t *app_key_process(void);
void app_key_set_status(enum key_name name, enum key_status status);
void app_key_get_status(struct app_key *ak);

#endif
