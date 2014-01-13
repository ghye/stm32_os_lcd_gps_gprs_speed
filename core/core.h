#ifndef __CORE_H__
#define __CORE_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	uint32_t *os_stk_ptr;       //当前的栈顶指针
	uint8_t os_cur_id;		//当前任务ID
	uint8_t os_cur_status;	//当前任务状态
	uint16_t os_delay_ticks;	//延迟ticks
} OS_TCB;

extern volatile uint64_t ticks;

//汇编去处理的
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

