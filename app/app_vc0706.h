#ifndef __APP_VC0706_H__
#define __APP_VC0706_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void app_vc0706_init(void);
void app_vc0706_frame(unsigned char **buf, int *len, int glen, int *head);
void app_vc0706_prepared_data(void);
unsigned int app_vc0706_frame_len(void);

#endif
