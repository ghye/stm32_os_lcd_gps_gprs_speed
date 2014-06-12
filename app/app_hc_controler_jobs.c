#include "public.h"
#include "app_net_usart.h"
#include "app_zgb.h"
#include "app_ir.h"
#include "app_coil.h"
#include "app_net_usart.h"
#include "app_weighbridge.h"
#include "app_cpu_id.h"
#include "app_hc_controler_jobs.h"

#if defined(HC_CONTROLER_)

void app_hc_controler_task3(void)
{
	#define LBUF_LEN	80
	int len;
	char buf[LBUF_LEN];

	app_net_usart_proc(buf, &len, LBUF_LEN);
	if (len > 0) {

	}

	app_zgb_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
	}

	app_zgb2_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
	}

	app_ir_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
	}
	
	app_coil_proc(buf, &len, LBUF_LEN);
	if (len > 0) {
		app_net_usart_send(buf, len);
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
