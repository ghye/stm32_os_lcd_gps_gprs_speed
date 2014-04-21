#ifndef __DRIV_VC0706_H__
#define __DRIV_VC0706_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void driv_vc0706_get_data(unsigned char **buf, int *len, int glen, int *phead);
void driv_vc0706_nos_init(void);
bool driv_vc0706_os_init(void);
unsigned int driv_vc0706_frame_len(void);
void driv_vc0706_isr(unsigned char v);

#endif
