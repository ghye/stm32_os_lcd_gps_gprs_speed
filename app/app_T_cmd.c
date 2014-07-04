#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "public.h"

#include "app_sys.h"
#include "app_red_green.h"
#include "app_brake_gate.h"
#include "app_net_usart.h"
#include "app_T_cmd.h"
#include "app_voice.h"

#if defined(HC_CONTROLER_)

static void app_mk_TY_cmd(char *buf, int num)
{
	#define TY_CMD "T:Y:%d#"

	sprintf(buf, TY_CMD, num);
}

static char *app_this_cmd_check(char *buf, int len, char *str)
{
	int i;

	if (strlen(str) > len)
		return NULL;
	for (i = 0; i < strlen(str); i++)
		if (buf[i] != str[i])
			break;
	if (i >= strlen(str))
		return buf + strlen(str);
	return NULL;
}

static void app_voice_action(char *buf, int len)
{
	int i;
	int num;
	unsigned char v;
	char lbuf[16];
	char *p;

	//T:I:0,010A#
	p = buf;
	for (i = 0; i < len; i++)
		if (p[i] == ',')
			break;
	if (i >= len)
		return;
	strncpy(lbuf, p, i);
	lbuf[i] = '\0';
	num = strtol(lbuf, NULL, 10);
	p += i + 1;
	len -= i + 1;

	for (i = 0; i < len / 2; i++) {
		memcpy(lbuf, p + i * 2, 2);
		lbuf[2] = '\0';
		v = strtol(lbuf, NULL, 16);
		app_voice_data_insert(&v, 1);
	}
	
	app_mk_TY_cmd(lbuf, num);
	app_net_usart_send(lbuf, strlen(lbuf));
}

static void app_red_green_action(char *buf, int len)
{
	int i;
	int num;
	char dev;
	enum lightstate s;
	char lbuf[16];
	char *p;

	//T:J:1234,0,0#
	p = buf;
	for (i = 0; i < len; i++)
		if (p[i] == ',')
			break;
	if (i >= len)
		return;
	strncpy(lbuf, p, i);
	lbuf[i] = '\0';
	num = strtol(lbuf, NULL, 10);
	p += i + 1;
	len -= i + 1;

	for (i = 0; i < len; i++)
		if (p[i] == ',')
			break;
	if (i >= len)
		return;
	strncpy(lbuf, p, i);
	lbuf[i] = '\0';
	dev = strtol(lbuf, NULL, 10);
	p += i + 1;
	len -= i + 1;

	for (i = 0; i < len; i++)
		if (p[i] == '#')
			break;
	if (i >= len)
		return;
	strncpy(lbuf, p, i);
	lbuf[i] = '\0';
	i = strtol(lbuf, NULL, 10);
	switch (i) {
	case 1:
		s = RED_OPEN;
		break;
	case 2:
		s = GREEN_OPEN;
		break;
	default:
		break;
	}

	app_red_green_proc(dev, s);

	app_mk_TY_cmd(lbuf, num);
	app_net_usart_send(lbuf, strlen(lbuf));
}

static void app_brake_gate_action(char *buf, int len)
{
	int i;
	int num;
	char dev;
	enum gatestate s;
	char lbuf[16];
	char *p;

	//T:L:1234,0,1#
	s = GATE_NONE;
	p = buf;
	for (i = 0; i < len; i++)
		if (p[i] == ',')
			break;
	if (i >= len)
		return;
	strncpy(lbuf, p, i);
	lbuf[i] = '\0';
	num = strtol(lbuf, NULL, 10);
	p += i + 1;
	len -= i + 1;

	for (i = 0; i < len; i++)
		if (p[i] == ',')
			break;
	if (i >= len)
		return;
	strncpy(lbuf, p, i);
	lbuf[i] = '\0';
	dev = strtol(lbuf, NULL, 10);
	p += i + 1;
	len -= i + 1;

	for (i = 0; i < len; i++)
		if (p[i] == '#')
			break;
	if (i >= len)
		return;
	strncpy(lbuf, p, i);
	lbuf[i] = '\0';
	i = strtol(lbuf, NULL, 10);
	switch (i) {
	case 0:
		s = GATE_DOWN;
		break;
	case 1:
		s = GATE_UP;
		break;
	default:
		break;
	}

	app_brake_gate_proc(dev, s);

	app_mk_TY_cmd(lbuf, num);
	app_net_usart_send(lbuf, strlen(lbuf));
}

char app_T_cmd_proc(char *buf, int len)
{
	char *p;

	p = app_this_cmd_check(buf, len, "T:B:");
	if (NULL !=p ) {
		app_sys_reboot();
		goto done;
	}
	
	p = app_this_cmd_check(buf, len, "T:I:");
	if (NULL !=p ) {
		app_voice_action(p, len - (p - buf));
		goto done;
	}

	p = app_this_cmd_check(buf, len, "T:J:");
	if (NULL !=p ) {
		app_red_green_action(p, len - (p - buf));
		goto done;
	}

	p = app_this_cmd_check(buf, len, "T:L:");
	if (NULL !=p ) {
		app_brake_gate_action(p, len - (p - buf));
		goto done;
	}

	return 0;

	done:
	return 1;
}

#endif
