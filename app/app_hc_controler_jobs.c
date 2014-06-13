#include "public.h"
#include "app_net_usart.h"
#include "app_zgb.h"
#include "app_ir.h"
#include "app_coil.h"
#include "app_net_usart.h"
#include "app_weighbridge.h"
#include "app_cpu_id.h"
#include "app_hc_controler_jobs.h"
#include "app_T_cmd.h"

#if defined(HC_CONTROLER_)

void app_hc_controler_task3(void)
{
	#define LBUF_LEN	80
	int i;
	int len;
	char buf[LBUF_LEN];

	i = 0;
	lp_net:
	app_net_usart_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_T_cmd_proc(buf, len);
		i += len;
		if (i < 720)
			goto lp_net;
	}

	i = 0;
	lp_zgb:
	app_zgb_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
		i += len;
		if (i < 720)
			goto lp_zgb;
	}

	i = 0;
	lp_zgb2:
	app_zgb2_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
		i += len;
		if (i < 720)
			goto lp_zgb2;
	}

	i = 0;
	lp_ir:
	app_ir_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
		i++;
		if (i < 50)
			goto lp_ir;
	}

	i = 0;
	lp_coil:
	app_coil_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
		i++;
		if (i < 50)
			goto lp_coil;
	}

	app_weighbridge_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
	}

	app_cpu_id_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
	}
}

void app_hc_controler_task2(void)
{
	/*app_red_green_proc();
	app_brake_gate_proc();*/
}

void app_hc_controler_task1(void)
{
	//app_voice_proc();
}

#endif
