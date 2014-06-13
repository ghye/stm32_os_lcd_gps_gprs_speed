#ifndef __APP_BRAKE_GATE_H__
#define __APP_BRAKE_GATE_H__

enum gatestate {
	GATE_UP = 1,
	GATE_DOWN
};

void app_brake_gate_proc(char dev, enum gatestate s);

#endif
