#include "stm32f10x_iwdg.h"
#include "driv_watchdog.h"

void driv_wdg_init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//�����Ĵ�����д

	IWDG_SetPrescaler(IWDG_Prescaler_256);//40Kʱ��256��Ƶ

	IWDG_SetReload(0xfff);                 //��������ֵ�����26��

	//IWDG_ReloadCounter();             //����������

	//IWDG_Enable();                       //�������Ź�
}

void driv_wdg_start(void)
{
	IWDG_ReloadCounter();             //����������

	IWDG_Enable();                       //�������Ź�
}

void driv_wdg_keeplive(void)
{
	IWDG_ReloadCounter();             //����������
}
