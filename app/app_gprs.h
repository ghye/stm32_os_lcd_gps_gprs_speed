#ifndef __APP_GPRS_H__
#define __APP_GPRS_H__

#include <stdint.h>

#define RFID_MSG_LEN 128
#define kEY_MSG_LEN 128

extern uint8_t g_rfid_send_gprs_msg[RFID_MSG_LEN];
extern uint8_t g_key_send_gprs_msg[kEY_MSG_LEN];

void app_gprs_clean_seqed_msgs(void);
void app_gprs_form_seqed_msgs(void);
void app_gprs_init(void);
int32_t app_gprs_socket(void);
void app_gprs_process_gprs_rbuf(uint8_t val);

#endif

