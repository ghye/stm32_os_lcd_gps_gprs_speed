#ifndef __APP_WEIGHBRIDGE_H__
#define __APP_WEIGHBRIDGE_H__

void app_weighbridge_proc(char *buf, int *len, int max);
void app_weighbridge_init(void);
void app_weighbridge_isr(unsigned char v);

#endif
