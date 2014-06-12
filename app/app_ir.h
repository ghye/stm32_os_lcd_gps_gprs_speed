#ifndef __APP_IR_H__
#define __APP_IR_H__

void app_ir_init(void);
void app_ir_proc(char *buf, int *len, int max);
void app_ir1_isr(unsigned char level);
void app_ir2_isr(unsigned char level);
void app_ir1_timer_proc(void);
void app_ir2_timer_proc(void);
void app_ir1_isr_proc(void);
void app_ir2_isr_proc(void);

#endif
