#ifndef __CORE_H__
#define __CORE_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	uint32_t *os_stk_ptr;       //��ǰ��ջ��ָ��
	uint8_t os_cur_id;		//��ǰ����ID
	uint8_t os_cur_status;	//��ǰ����״̬
	uint16_t os_delay_ticks;	//�ӳ�ticks
} OS_TCB;

extern volatile uint64_t ticks;

//���ȥ�����
void OS_CONTEX_SWITCH(void);
void OS_START(void);
void PendSV_Handler(void);

void os_task_init(void);
void os_task_creat(void (*task)(void *), uint8_t id);
void os_task_switch(void);
void os_task_ok(void);
void os_task_delayms(uint32_t ms);
void os_task_wakeup(void);

#endif

