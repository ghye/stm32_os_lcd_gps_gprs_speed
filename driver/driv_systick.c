#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"

#include "driv_systick.h"

void driv_systick_init(void)
{
	SystemCoreClockUpdate();
	if (SysTick_Config((SystemCoreClock / (1000/TICK_MS)))) {	
	//if (SysTick_Config(SystemCoreClock / 1000)) {
		/*error*/
		while (1);
	}
	
	/* Select AHB clock(HCLK) as SysTick clock source */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

