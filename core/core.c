#include "core.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "driv_systick.h"
#include "app_usart.h"
#include "public.h"

//任务大小
#define START_STK_SIZE 1024	//128
#define START_MAX_TSK 4

//任务堆栈
//uint32_t TASK_1_STK[START_STK_SIZE];
//uint32_t TASK_2_STK[START_STK_SIZE];
//uint32_t TASK_3_STK[START_STK_SIZE];
__align(8) uint32_t TASKS_STK[START_MAX_TSK+1][START_STK_SIZE];

//任务状态
#define TASK_STOP	0x00
#define TASK_DELAY	0x01
#define TASK_RUN	0xff

OS_TCB OS_TASK_BLOCK[START_MAX_TSK+1];

OS_TCB *OS_CUR_SP;		//当前任务指针
OS_TCB *OS_NEXT_SP;	//下个任务指针

volatile uint64_t ticks = 0;
static bool g_os_start = false;

void os_task_init(void)
{
	memset((void *)TASKS_STK, 0, (START_MAX_TSK+1)*START_STK_SIZE);
}

//任务创建函数
void os_task_creat(void (*task)(void *), uint8_t id)
{
	uint32_t *stk;
	uint8_t i;
	
	//stk = ptos;                                       /*  Load stack pointer          */
	for (i=0; i<START_MAX_TSK+1; i++) {
		if (TASKS_STK[i][START_STK_SIZE-1] != 0x01000000UL)
			break;
	}
	stk = &TASKS_STK[i][START_STK_SIZE-1];
	
	/*  模拟成异常，自动把寄存器压栈*/
	*(stk) = (uint32_t)0x01000000UL;                     /*  xPSR                        */ 
	*(--stk) = (uint32_t)task;                                /*  Entry Point of the task     */
	*(--stk) = (uint32_t)0xFFFFFFFEUL;                   /*  R14 (LR)  (init value will  */
	                                                               
	*(--stk) = (uint32_t)0x12121212UL;                   /*  R12                         */
	*(--stk) = (uint32_t)0x03030303UL;                   /*  R3                          */
	*(--stk) = (uint32_t)0x02020202UL;                   /*  R2                          */
	*(--stk) = (uint32_t)0x01010101UL;                   /*  R1                          */
	*(--stk) = (uint32_t)0x00000000UL;                   /*  R0                          */

	*(--stk) = (uint32_t)0x11111111UL;                   /*  R11                         */
	*(--stk) = (uint32_t)0x10101010UL;                   /*  R10                         */
	*(--stk) = (uint32_t)0x09090909UL;                   /*  R9                          */
	*(--stk) = (uint32_t)0x08080808UL;                   /*  R8                          */
	*(--stk) = (uint32_t)0x07070707UL;                   /*  R7                          */
	*(--stk) = (uint32_t)0x06060606UL;                   /*  R6                          */
	*(--stk) = (uint32_t)0x05050505UL;                   /*  R5                          */
	*(--stk) = (uint32_t)0x04040404UL;                   /*  R4                          */

	OS_TASK_BLOCK[id].os_stk_ptr = stk;
	OS_TASK_BLOCK[id].os_cur_id = id;
}

static uint8_t next_switch_task(uint8_t cur_id)
{
	uint8_t i;

	for (i=cur_id+1; i<START_MAX_TSK; i++) {
		if (OS_TASK_BLOCK[i].os_cur_status != TASK_DELAY)
			return i;
	}

	for (i=0; i<cur_id; i++) {
		if (OS_TASK_BLOCK[i].os_cur_status != TASK_DELAY)
			return i;
	}

	return cur_id;
}

//任务切换函数
void os_task_switch(void)
{
	uint8_t i= 0;
	uint8_t id;
	
	for(i = 0; i < START_MAX_TSK; i++)		//查找下一个待命的任务
	{
		if(OS_TASK_BLOCK[i].os_cur_status == TASK_RUN)
		{
			//OS_TASK_BLOCK[i].os_cur_status = TASK_STOP;
			id = next_switch_task(i);
			if (id == i)
				return;	/*if the same, donot switch*/
			OS_TASK_BLOCK[i].os_cur_status = TASK_STOP;
			OS_TASK_BLOCK[id].os_cur_status = TASK_RUN;
			OS_NEXT_SP = &OS_TASK_BLOCK[id];
			/*if((i+1) > (START_MAX_TSK-1))
			{
				OS_TASK_BLOCK[0].os_cur_status = TASK_RUN;
				OS_NEXT_SP = &OS_TASK_BLOCK[0];
			}
			else
			{
				OS_TASK_BLOCK[i+1].os_cur_status = TASK_RUN;
				OS_NEXT_SP = &OS_TASK_BLOCK[i+1];
			}*/
			break;
		}
	}
	
	OS_CONTEX_SWITCH();		//准备切换至下一个任务
}

void os_task_idle(void *arg)
{
	uint8_t i;
	uint8_t buf[64];
	while (1) {
		buf[0]='\0';
		for (i=0; i<=START_MAX_TSK; i++)
			sprintf((void *)(buf + strlen((void *)buf)), "%u,%u;", OS_TASK_BLOCK[i].os_cur_status, OS_TASK_BLOCK[i].os_delay_ticks);
		sprintf((void *)(buf + strlen((void *)buf)), "%llu;", ticks);
		//com_send_message(USART_GPS_NUM, (void *)buf);

		i = next_switch_task(0);
		if (OS_TASK_BLOCK[i].os_cur_status != TASK_DELAY) {
			OS_TASK_BLOCK[i].os_cur_status = TASK_RUN;
			OS_NEXT_SP = &OS_TASK_BLOCK[i];
			OS_CONTEX_SWITCH();
		}
	}
}

void os_task_ok(void)
{
	os_task_creat(os_task_idle, START_MAX_TSK);
	
	//开始调度
	OS_CUR_SP = OS_NEXT_SP = &OS_TASK_BLOCK[0];
	OS_TASK_BLOCK[0].os_cur_status = TASK_RUN;

	g_os_start = true;
}

/*
	ms: 不应该小于systick间隔
*/
void os_task_delayms(uint32_t ms)
{
	uint8_t i= 0;
	uint8_t id;

	ms /= TICK_MS;
	if (0 == ms)
		ms = 1;
	
	for(i = 0; i < START_MAX_TSK; i++) {
		if(OS_TASK_BLOCK[i].os_cur_status == TASK_RUN) {
			OS_TASK_BLOCK[i].os_delay_ticks = ms;
			OS_TASK_BLOCK[i].os_cur_status = TASK_DELAY;
			id = next_switch_task(i);
			if (id == i) {
				OS_NEXT_SP = &OS_TASK_BLOCK[START_MAX_TSK];
			}
			else {
				OS_TASK_BLOCK[id].os_cur_status = TASK_RUN;
				OS_NEXT_SP = &OS_TASK_BLOCK[id];
			}
			break;
		}
	}
	
	OS_CONTEX_SWITCH();
}

void os_task_wakeup(void)
{
	int16_t id = -1;
	uint8_t i= 0;

	if (false == g_os_start)
		return;
	
	for(i = 0; i < START_MAX_TSK; i++) {
		if(OS_TASK_BLOCK[i].os_cur_status == TASK_DELAY) {
			OS_TASK_BLOCK[i].os_delay_ticks--;
			if (0 == OS_TASK_BLOCK[i].os_delay_ticks) {
				OS_TASK_BLOCK[i].os_cur_status = TASK_STOP;
			}
		}
	}

#if 0
	i = (OS_NEXT_SP - &OS_TASK_BLOCK[0]) / sizeof(OS_TASK_BLOCK[0]);
	if (i < START_MAX_TSK)	/*有应用任务在执行中*/
		return;
	
	srand((int)ticks);
	i = (START_MAX_TSK - 1) * ((1.0 * rand()) / RAND_MAX);
	i %= START_MAX_TSK - 1;	

	id = next_switch_task(i);
	if (i == id) {
		if (OS_TASK_BLOCK[id].os_cur_status == TASK_DELAY)
			return;
	}
	OS_TASK_BLOCK[id].os_cur_status = TASK_RUN;
	OS_NEXT_SP = &OS_TASK_BLOCK[id];
	OS_CONTEX_SWITCH();			
#endif

	/*若有正在运行的task,　则等待该task自身调用切换任务接口*/
}

