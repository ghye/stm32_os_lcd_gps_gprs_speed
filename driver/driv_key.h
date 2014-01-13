#ifndef __DRIV_KEY_H__
#define __DRIV_KEY_H__

#include <stdint.h>
#include <stdbool.h>

#define KEY_NUM	4

enum key_name {
	E_KEY_NAME_ERROR = 0,
	E_KEY_NAME_OK = 1,
	E_KEY_NAME_LEFT = 1 << 1,
	E_KEY_NAME_RIGHT = 1 << 2,
	E_KEY_NAME_BACK = 1 << 3
};

enum key_status {
	E_KEY_SINGLE_CLICK = 1,	/*单击*/
	E_KEY_DOUBLE_CLICK,	/*双击*/
	E_KEY_LONG_PRESS		/*长按*/
};

enum dkey_status {
	E_DKEY_NOPRESS = 1,
	E_DKEY_DEBOUNCE,		/*去抖动*/
	E_DKEY_SHORT_PRESS,
	E_DKEY_LONG_PRESS
};

struct dkey_st {
	uint64_t stime;	/*ticks 时间*/
	enum dkey_status status;
	enum key_name keyname;
};

extern struct dkey_st dkey[KEY_NUM];

void driv_key_init(void);
void driv_key_process(enum key_name kname);
void driv_key_tick_process(void);

#endif
