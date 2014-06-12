#ifndef __APP_CPU_ID_H__
#define __APP_CPU_ID_H__

void app_cpu_id_init(void);
void app_cpu_id_to_str(char *str);
void app_cpu_id_proc(char *buf, int *len, int max);

#endif
