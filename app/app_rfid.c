#include "app_rfid.h"
#include "driv_mfrc522.h"
#include "app_gprs.h"

#if defined (CAR_DB44_V1_0_20130315_)

uint8_t default_key[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
uint8_t g_mfrc522_buf[20];
uint32_t g_rfid_serial_num = 0;

static void app_rfid_warn(void)
{
	g_rfid_serial_num = (g_mfrc522_buf[3] << 24)  | (g_mfrc522_buf[2] << 16) | 
								(g_mfrc522_buf[1] << 8) | g_mfrc522_buf[0];

	app_beep_open();
	os_task_delayms(5);
	app_beep_close();
}

void app_rfid_init(void)
{
	driv_mfrc522_init();
}

void app_rfid_proc(void)
{
	driv_mfrc522_status_check();
	
	if (!driv_rfid_check(PICC_REQIDL, g_mfrc522_buf))
		return;
	if (!driv_rfid_anticoll(g_mfrc522_buf))
		return;

	app_rfid_warn();

	/*到处刷了一次卡，做相关处理*/
	sprintf(g_rfid_send_gprs_msg, "H:,%.10u,,#", g_rfid_serial_num);
	
	if (!driv_rfid_select(g_mfrc522_buf))
		return;
	/*if (!driv_rfid_auth(PICC_AUTHENT1A, 1, default_key, g_mfrc522_buf))
		return;
	if (!driv_rfid_read(2, g_mfrc522_buf))
		return;*/
	if (!driv_rfid_halt())
		return;

/*
	uint8_t buf[] = {1, 2, 3, 4};
	
	//driv_mfrc522_write(buf, sizeof(buf) / sizeof(buf[0]));

	driv_mfrc522_test();
	*/
}

#endif
