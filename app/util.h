#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

void delete_zero_datastr(char *str);
uint8_t *memstr(uint8_t *buf, uint32_t buflen, uint8_t *str, uint32_t strlen);

#endif
