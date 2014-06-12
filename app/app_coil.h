#ifndef __APP_COIL_H__
#define __APP_COIL_H__

void app_coil_init(void);
void app_coil_proc(char *buf, int *len, int max);
void app_coil1_isr(unsigned char level);
void app_coil2_isr(unsigned char level);
void app_coil3_isr(unsigned char level);
void app_coil4_isr(unsigned char level);
void app_coil1_timer_proc(void);
void app_coil2_timer_proc(void);
void app_coil3_timer_proc(void);
void app_coil4_timer_proc(void);
void app_coil1_isr_proc(void);
void app_coil2_isr_proc(void);
void app_coil3_isr_proc(void);
void app_coil4_isr_proc(void);

#endif
