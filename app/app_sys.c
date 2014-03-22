#include "app_sys.h"

__asm void SystemReset(void) 
{ 
	MOV R0, #1           //;  
	MSR FAULTMASK, R0    //; ���FAULTMASK ��ֹһ���жϲ��� 
	LDR R0, =0xE000ED0C  //; 
	LDR R1, =0x05FA0004  //; 
	STR R1, [R0]         //; ϵͳ�����λ    

deadloop 
	B deadloop        //; ��ѭ��ʹ�������в�������Ĵ��� 
}

static void app_sys_reboot_pre(void)
{

}

void app_sys_reboot(void)
{
	app_sys_reboot_pre();

	SystemReset();
}
