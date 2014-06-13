#ifndef __APP_RED_GREEN_H__
#define __APP_RED_GREEN_H__

enum lightstate {
	RED_OPEN = 1,
	RED_CLOSE,
	GREEN_OPEN,
	GREEN_CLOSE
};

void app_red_green_proc(char dev, enum lightstate s);
void app_red_green_init(void);

#endif
