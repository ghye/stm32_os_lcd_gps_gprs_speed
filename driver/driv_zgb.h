#ifndef __DRIV_ZGB_H__
#define __DRIV_ZGB_H__

#include <stdint.h>
#include <stdbool.h>

void driv_zgb_init(void);
void driv_zgb_read_isr(uint8_t v);
void driv_zgb_get(char *buf, int *len, int maxlen);

#endif
