#ifndef __DRIV_SYSTICK_H__
#define __DRIV_SYSTICK_H__

#define TICK_MS	1	/*1ms*/
#define HZ 		(1000/TICK_MS)

void driv_systick_init(void);

#endif

