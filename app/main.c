
#include "core.h"
#include "tasks.h"
#include "board_init.h"

//主函数
int main(void)
{
	board_init();
	
	os_task_init();
	
	//创建三个任务
	os_task_creat(task_gprs, 0);
	os_task_creat(task2, 1);
	os_task_creat(task3, 2);
	os_task_creat(task4, 3);

	os_task_ok();
	OS_START();
	
	while(1);
}

#if 0
//用户函数
void task1(void *arg)
{
	uint8_t i = 0;
	uint16_t j;
	
	while(1)
	{
		j=1+(int)(10.0*rand()/(RAND_MAX+1.0));
		j *= 100;
		i++;
		app_usart2_send(1, i);
		os_task_delayms(j);
		//os_task_switch();
	}
}

void task2(void *arg)
{
	uint8_t i = 0;
	uint16_t j;
	
	while(1)
	{
		j=1+(int)(10.0*rand()/(RAND_MAX+1.0));
		j *= 100;
		i++;
		//app_usart2_gpio();
		app_usart2_send(2, i);
		//os_task_delayms(j);
		os_task_switch();
	}
}

void task3(void *arg)
{
	uint8_t i = 0;
	uint16_t j;
	
	while(1)
	{
		j=1+(int)(10.0*rand()/(RAND_MAX+1.0));
		j *= 100;
		i++;
		app_usart2_send(3, i);
		os_task_delayms(j);
		//os_task_switch();
	}
}
#endif

