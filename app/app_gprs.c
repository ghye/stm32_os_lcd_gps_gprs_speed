#include <stdint.h>
#include <stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include "core.h"
#include "driv_gprs.h"
#include "app_gprs.h"
#include "driv_systick.h"
#include "app_gps.h"
#include "app_network_data.h"
#include "app_hmc5883l_bmp085.h"
#include "util.h"
#include "app_zgb.h"
#include "app_vc0706.h"

#if defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_) || defined(CAR_TRUCK_1_3_140303_)

#define GPRS_MAX_MSG_LEN 512

#define ATE0 			"ATE0\x00D\x00A"
#define GET_IMEI 		"AT+CGSN\x00D\x00A"
#define SET_NETTYPE		"AT^SICS=0,conType,GPRS0\x00D\x00A"	/* 10.1.3 */
#define SET_APNTYPE		"AT^SICS=0,apn,3GNET\x00D\x00A"
#define SET_SRVTYPE		"AT^SISS=0,srvType,Socket\x00D\x00A"
#define SET_CONID		"AT^SISS=0,conId,0\x00D\x00A"
#define OPEN_CON0		"AT^SISO=0\x00D\x00A"
#define MG323_TP_TRANS	"AT^IPENTRANS=0\x00D\x00A"
#define CLOSE_CON0		"AT^SISC=0\x00D\x00A"

#if 0
#define SET_SRV_URL_0 "AT^SISS=0,address,\"socktcp://%s\"\x00D\x00A"
#define SET_SRV_URL_1 "AT^SISS=0,address,\"socktcp://truck.lnwo.cn:6969\"\x00D\x00A"
#define SET_SRV_URL_2 "AT^SISS=0,address,\"socktcp://113.105.139.109:6969\"\x00D\x00A"
#define SET_SRV_URL_3 "AT^SISS=0,address,\"socktcp://61.235.81.55:6969\"\x00D\x00A"
#define SET_SRV_URL_4 "AT^SISS=0,address,\"socktcp://test.link-world.cn:6969\"\x00D\x00A"
#define SET_SRV_URL_5 "AT^SISS=0,address,\"socktcp://203.88.202.116:6969\"\x00D\x00A"
#else
#define SET_SRV_URL_0 "AT^SISS=0,address,\"socktcp://%s\"\x00D\x00A"
#define SET_SRV_URL_1 "AT^SISS=0,address,\"socktcp://113.105.139.109:6969\"\x00D\x00A"
#define SET_SRV_URL_2 "AT^SISS=0,address,\"socktcp://113.105.139.109:6969\"\x00D\x00A"
#define SET_SRV_URL_3 "AT^SISS=0,address,\"socktcp://113.105.139.109:6969\"\x00D\x00A"
#define SET_SRV_URL_4 "AT^SISS=0,address,\"socktcp://113.105.139.109:6969\"\x00D\x00A"
#define SET_SRV_URL_5 "AT^SISS=0,address,\"socktcp://113.105.139.109:6969\"\x00D\x00A"
#endif


typedef enum{
	GPRS_STATUS_NOINIT,
	GPRS_STATUS_ATE,
	GPRS_STATUS_GET_IMEI,
	GPRS_STATUS_NETTYPE,
	GPRS_STATUS_APNTYPE,
	GPRS_STATUS_SRVTYPE,
	GPRS_STATUS_CONID,
	GPRS_STATUS_SRVURL,
	GPRS_STATUS_SOCKET_OPEN,
	GPRS_STATUS_SOCKET_OPEN_SUCC,
	GPRS_STATUS_SOCKET_TP,
	GPRS_STATUS_SOCKET_TP_SUCC,
	GPRS_STATUS_SOCKET_TP_FINISH,
	GPRS_STATUS_SOCKET_CLOSE,
	GPRS_STATUS_SOCKET_CLOSE_OK,
		
	GPRS_STATUS_MAX,
}GPRS_MODULE_STATUS;

#define GPRS_STATUS_TIMEOUT_S 120	/*s*/
uint64_t gprs_err_recoder = 0;

struct _gprs_info {
	bool send_imei_now;
	GPRS_MODULE_STATUS gprs_status;
	uint8_t imei[15];
	struct {
		uint16_t rbuf_index;
		uint8_t rbuf[GPRS_MAX_MSG_LEN];
	}rbuf_info;
	struct {
		uint8_t sbuf_len;
		uint8_t sbuf[GPRS_MAX_MSG_LEN];
	}sbuf_info;
};
struct _gprs_info gprs_info;

#define SEQED_MSGS_MAX_LEN 64
#define SEQED_MSGS_MAX_NUM 10
struct _seqed_msgs {
	uint32_t gprs_rbuf_point;
	uint8_t msgs_num;
	uint8_t msgs[SEQED_MSGS_MAX_NUM][SEQED_MSGS_MAX_LEN];
	uint32_t msgtmp_len;
	uint8_t msgtmp[SEQED_MSGS_MAX_LEN];
};
struct _seqed_msgs seqed_msgs;


struct srvs_ {
	uint8_t server_sel;
	uint8_t servers[6][80];
};
struct srvs_ srvs;

static int8_t g_IP_DNS[80];	/*服务器设置的命令*/

#if defined (CAR_DB44_V1_0_20130315_)
uint8_t g_rfid_send_gprs_msg[RFID_MSG_LEN];
#endif

#if defined (CAR_DB44_V1_0_20130315_)
uint8_t g_key_send_gprs_msg[kEY_MSG_LEN];
#endif
	
static void srvs_init(void)
{
	memset(&srvs, '\0', sizeof(struct srvs_));
	srvs.server_sel = 0;
	if (0 == strlen((void *)g_IP_DNS))
		//strcpy((void *)srvs.servers[0], (void *)g_IP_DNS);
		strcpy((void *)srvs.servers[0], SET_SRV_URL_1);
	else
		sprintf((void *)srvs.servers[0], SET_SRV_URL_0, g_IP_DNS);
	strcpy((void *)srvs.servers[1], SET_SRV_URL_1);
	strcpy((void *)srvs.servers[2], SET_SRV_URL_2);
	strcpy((void *)srvs.servers[3], SET_SRV_URL_3);
	strcpy((void *)srvs.servers[4], SET_SRV_URL_4);
	strcpy((void *)srvs.servers[5], SET_SRV_URL_5);
}

static void srvs_next(void)
{
	srvs.server_sel++;
	if (srvs.server_sel >= sizeof(srvs.servers)/sizeof(srvs.servers[0])) {
		srvs.server_sel = 0;
	}
}

static uint8_t *get_cur_srv(void)
{
	return srvs.servers[srvs.server_sel];
}

void app_gprs_seqed_msgs_init(void)
{
	uint32_t i;

	seqed_msgs.msgtmp_len = 0;
	seqed_msgs.msgs_num = 0;
	seqed_msgs.gprs_rbuf_point = 0;
	for (i=0; i<SEQED_MSGS_MAX_NUM; i++)
		memset(seqed_msgs.msgs, '\0', SEQED_MSGS_MAX_LEN);
}

void app_gprs_clean_seqed_msgs(void)
{
	uint32_t i;

	seqed_msgs.msgs_num = 0;
	for (i=0; i<SEQED_MSGS_MAX_NUM; i++)
		memset(seqed_msgs.msgs[i], '\0', SEQED_MSGS_MAX_LEN);
}
static void get_seqed_msgs(uint32_t start, uint32_t end)
{
	static bool newline = false;
	uint32_t i;
	uint8_t *p;

	p = gprs_info.rbuf_info.rbuf;
	p += start;
	for (i=0; i<end-start; i++) {
		if ((p[i] == 0x0D) || (p[i] == 0x0A)) {
			
			if (!newline) {
				if (seqed_msgs.msgtmp_len) {
					memcpy(seqed_msgs.msgs[seqed_msgs.msgs_num++], 
						seqed_msgs.msgtmp, seqed_msgs.msgtmp_len);
				}
				if (seqed_msgs.msgs_num >= SEQED_MSGS_MAX_NUM)
					seqed_msgs.msgs_num = 0;
			}
			seqed_msgs.msgtmp_len = 0;
			newline = true;
		}
		else {
			newline = false;	
			seqed_msgs.msgtmp[seqed_msgs.msgtmp_len++] = p[i];
		}
	}
}

void app_gprs_form_seqed_msgs(void)
{
	uint16_t i;
	uint32_t gindex;
	uint32_t spoint;
	
	gindex = gprs_info.rbuf_info.rbuf_index;
	spoint = seqed_msgs.gprs_rbuf_point;
	if (gindex == spoint)
		return ;
	if (gindex > spoint) {
		get_seqed_msgs(spoint, gindex);
	}
	else {
		get_seqed_msgs(spoint, GPRS_MAX_MSG_LEN/* - 1*/);
		get_seqed_msgs(0, gindex);
	}

	seqed_msgs.gprs_rbuf_point = gindex;

	for(i=0;i<seqed_msgs.msgs_num;i++) {
		//log_write(LOG_LEVEL_MSG, seqed_msgs.msgs[i], strlen((void *)seqed_msgs.msgs[i]));
		//log_write_ack("\x00A", 1);
	}
}

bool app_gprs_get_seqed_msg(uint8_t *p)
{
	uint32_t i;
	
	for(i=0;i<seqed_msgs.msgs_num;i++) {
		if (strstr((void *)seqed_msgs.msgs[i], (void *)p))
			return true;
	}

	return false;
}

bool app_gprs_get_seqed_msg_(uint32_t start, const uint8_t *p)
{
	uint32_t i;
	
	for(i=start;i<seqed_msgs.msgs_num;i++) {
		if (strstr((void *)seqed_msgs.msgs[i], (void *)p))
			return true;
	}

	return false;
}

/*从已序列号的信息里提取某个符合要求的信息*/
int32_t app_gprs_get_seqed_msg_body(uint32_t start, const uint8_t *str, uint8_t *p, uint32_t p_max)
{
	uint32_t i;

	for(i=start;i<seqed_msgs.msgs_num;i++) {
		if (strstr((void *)seqed_msgs.msgs[i], (void *)str)) {
			if (strlen((void *)seqed_msgs.msgs[i]) > p_max)
				return -1;
			strcpy((void *)p, (void *)seqed_msgs.msgs[i]);
			return i;
		}
	}

	return -1;
}

static void app_gprs_clean_gprs_sbuf(void)
{
	gprs_info.sbuf_info.sbuf_len = 0;
	memset(gprs_info.sbuf_info.sbuf, 0, GPRS_MAX_MSG_LEN);
}

static void app_gprs_clean_gprs_rbuf(void)
{
	driv_gprs_rx_it_disable();
	memset(gprs_info.rbuf_info.rbuf, 0, sizeof(gprs_info.rbuf_info.rbuf)/sizeof(gprs_info.rbuf_info.rbuf[0]));
	gprs_info.rbuf_info.rbuf_index = 0;
	driv_gprs_rx_it_enable();
}

void app_gprs_start_gprs_mode(void)
{
	memset(&gprs_info, 0, sizeof(gprs_info));
	gprs_info.send_imei_now = true;
	gprs_info.gprs_status = GPRS_STATUS_NOINIT;
	app_gprs_clean_gprs_sbuf();
	app_gprs_clean_gprs_rbuf();
}

static void app_gprs_send_ate0(void)
{
	extern volatile unsigned int Timer1, Timer2;
	
	gprs_info.gprs_status = GPRS_STATUS_ATE;
	driv_gprs_send_msg(ATE0, strlen(ATE0));
}

static void app_gprs_send_get_imei(void)
{	
	if(app_gprs_get_seqed_msg("OK") /*返回OK*/)
	{
		gprs_info.gprs_status = GPRS_STATUS_GET_IMEI;
		driv_gprs_send_msg(GET_IMEI, strlen(GET_IMEI));
	}
	else if (app_gprs_get_seqed_msg("ERROR")) {
		driv_gprs_send_msg(ATE0, strlen(ATE0));
	}
	else
	{
	}
}

static bool app_gprs_get_imei(void)
{
	uint32_t i;
	uint8_t tmp[20];
	
	for(i=0;i<seqed_msgs.msgs_num;i++) {
		if ((seqed_msgs.msgs[i][2] > 0) && (seqed_msgs.msgs[i][10]) > 0) {
			memcpy(gprs_info.imei, seqed_msgs.msgs[i], 15);
			memcpy(tmp, gprs_info.imei, 15);
			tmp[15] = '\0';

			return true;
		}
	}

	return false;
}

static void app_gprs_send_nettype(void)
{
	if (app_gprs_get_seqed_msg("OK")) {
		if(app_gprs_get_imei())
		{
			gprs_info.gprs_status = GPRS_STATUS_NETTYPE;
			driv_gprs_send_msg(SET_NETTYPE, strlen(SET_NETTYPE));
		}
		else 
			driv_gprs_send_msg(GET_IMEI, strlen(GET_IMEI));
	}
	else
	{
		//gprs_send_cmd(1, GET_IMEI, strlen(GET_IMEI));
	}
}

static void app_gprs_send_apntype(void)
{
	if(app_gprs_get_seqed_msg("OK")/*收到OK*/)
	{
		gprs_info.gprs_status = GPRS_STATUS_APNTYPE;
		driv_gprs_send_msg(SET_APNTYPE, strlen(SET_APNTYPE));
	}
	else if (app_gprs_get_seqed_msg("ERROR"))
	{
		driv_gprs_send_msg(SET_NETTYPE, strlen(SET_NETTYPE));
	}
	else {

	}
}

static int32_t app_gprs_set_sbuf_imei(void)
{
	uint8_t A_tmp[20];
	uint8_t tmp[20];
	
	//if(lw_is_sbuf_empty())
	{
		//lw_set_gprs_sbuf(TEST_CMD_D);
		//lw_gprs_get_imei();
		if((gprs_info.imei[0])>'0' && (gprs_info.imei[1]>'0'))
		{		
			memcpy(A_tmp, gprs_info.imei, 15);
			A_tmp[15] = '\0';
			sprintf((void *)tmp, "A:%s#", A_tmp);	
			//sprintf(tmp, "D:%s#", A_tmp);	
			//lw_set_gprs_sbuf(tmp);
			//gprs_info.gprs_status = GPRS_STATUS_NETTYPE;
			gprs_info.send_imei_now = true;
			return 0;
		}
	}

	return -1;
}

static void app_gprs_send_srvtype(void)
{
	if(app_gprs_get_seqed_msg("OK") /*收到OK*/)
	{
		gprs_info.gprs_status = GPRS_STATUS_SRVTYPE;
		driv_gprs_send_msg(SET_SRVTYPE, strlen(SET_SRVTYPE));
	}
	else if (app_gprs_get_seqed_msg("ERROR"))
	{
		driv_gprs_send_msg(SET_APNTYPE, strlen(SET_APNTYPE));
	}
	else {

	}
}

static void app_gprs_send_conid(void)
{
	if(app_gprs_get_seqed_msg("OK") /*收到OK*/)
	{
		gprs_info.gprs_status = GPRS_STATUS_CONID;
		driv_gprs_send_msg(SET_CONID, strlen(SET_CONID));
	}
	else if (app_gprs_get_seqed_msg("ERROR"))
	{
		driv_gprs_send_msg(SET_SRVTYPE, strlen(SET_SRVTYPE));
	}
	else {

	}
}

static void app_gprs_send_srvurl(void)
{
	if(app_gprs_get_seqed_msg("OK") /*收到OK*/)
	{
		gprs_info.gprs_status = GPRS_STATUS_SRVURL;
		//gprs_send_cmd(1, SET_SRV_URL_1, strlen(SET_SRV_URL_1));
		#if 0
		while (strlen((void *)get_cur_srv()) == 0)
			srvs_next();
		driv_gprs_send_msg((void *)get_cur_srv(), strlen((void *)get_cur_srv()));
		#else
		driv_gprs_send_msg(SET_SRV_URL_1, strlen(SET_SRV_URL_1));
		#endif

	}
	else if (app_gprs_get_seqed_msg("ERROR"))
	{
		driv_gprs_send_msg(SET_CONID, strlen(SET_CONID));
	}
	else {

	}
}

static void app_gprs_send_socket_open(void)
{
	if(app_gprs_get_seqed_msg("OK") /*收到OK*/)
	{
		gprs_info.gprs_status = GPRS_STATUS_SOCKET_OPEN;
		driv_gprs_send_msg(OPEN_CON0, strlen(OPEN_CON0));
	}
	else if (app_gprs_get_seqed_msg("ERROR"))
	{
		//gprs_send_cmd(1, SET_SRV_URL_1, strlen(SET_SRV_URL_1));
		#if 0
		driv_gprs_send_msg((void *)get_cur_srv(), strlen((void *)get_cur_srv()));
		#else
		driv_gprs_send_msg(SET_SRV_URL_1, strlen(SET_SRV_URL_1));
		#endif
	}
	else {

	}
}

static void app_gprs_check_socket_opend_succ(void)
{
	static bool sk_opend_ok = false;
	uint32_t i;
	uint8_t *p;
	uint8_t *ptmp;

	if (!sk_opend_ok) {
		if (app_gprs_get_seqed_msg("OK") /*接收到OK*/) {
			sk_opend_ok = true;
		}
		else if (app_gprs_get_seqed_msg("ERROR") /*接收到ERROR*/) {
			driv_gprs_send_msg(OPEN_CON0, strlen(OPEN_CON0));
		}
		else {/*等待*/

		}
	}
	if (sk_opend_ok){
		for(i=0;i<seqed_msgs.msgs_num;i++) {
			if ((p = (void *)strstr((void *)seqed_msgs.msgs[i], "SISW")) &&
					(ptmp = (void *)strstr((void *)(p+1), ",")) &&
					(p = (void *)strstr((void *)(ptmp+1), ","))) { /*收到类似"^SISW: 0,1,1024" 的主动上报信息*/
				*(p) = '\0';
				if (atoi((void *)(ptmp+1))) {
					sk_opend_ok = false; /*为了下次重新打开可再使用*/
					//gprs_info.send_status.session_max_len = atoi((void *)(p+1));
					gprs_info.gprs_status = GPRS_STATUS_SOCKET_OPEN_SUCC;
					break;
				}
			}
		}
	}
}

static int32_t app_gprs_send_socket_tp_mode(void)
{
	gprs_info.gprs_status = GPRS_STATUS_SOCKET_TP;
	driv_gprs_send_msg(MG323_TP_TRANS, strlen(MG323_TP_TRANS));

	return 0;
}

static int32_t app_gprs_check_socket_tp_mode_succ(void) 
{
	if (app_gprs_get_seqed_msg("OK") /*收到OK*/) {
		gprs_info.gprs_status = GPRS_STATUS_SOCKET_TP_SUCC;
	}
	else if (app_gprs_get_seqed_msg("ERROR") /*收到ERROR*/) {
		driv_gprs_send_msg(MG323_TP_TRANS, strlen(MG323_TP_TRANS));
	}
	else { /*等待*/

	}

	return 0;
}

static void app_gprs_send_imei(void)
{
	uint8_t imei[20];
	uint8_t buf[80];
		
	memcpy(imei, gprs_info.imei, 15);
	imei[15] = '\0';
	sprintf((void *)buf, "A:%s#", imei);	
	driv_gprs_send_msg((void *)buf, strlen((void *)buf));

	sprintf((void *)buf, "V:%s", FUN);
	strcat((void *)buf, HVERSION);
	strcat((void *)buf, SVERSION);
	strcat((void *)buf, "_");
	strcat((void *)buf, __DATE__);
	strcat((void *)buf, " ");
	strcat((void *)buf, __TIME__);
	strcat((void *)buf, "#");
	driv_gprs_send_msg((void *)buf, strlen((void *)buf));

	sprintf((void *)buf, "T:T:%llu#", ticks / HZ);
	driv_gprs_send_msg((void *)buf, strlen((void *)buf));
}

static void app_gprs_make_gps_msg(char *buf, struct gprmc_ *prmc)
{
	char tbuf[16];
	
	sprintf((void *)buf, "%s", "s:");
	sprintf((void *)tbuf, "%lf", prmc->lat);
	delete_zero_datastr((void *)tbuf);
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	sprintf((void *)tbuf, "%lf", prmc->lon);
	delete_zero_datastr((void *)tbuf);
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	sprintf((void *)tbuf, "%lf", prmc->speed);
	delete_zero_datastr((void *)tbuf);
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	sprintf((void *)tbuf, "%lf", prmc->track);
	delete_zero_datastr((void *)tbuf);
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	sprintf((void *)tbuf, "%d", (prmc->status==STATUS_FIX) ? 1:0);
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	memcpy((void *)tbuf, (void *)prmc->time.tm, 12);
	tbuf[12] = '\0';
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, "#");
}

static void app_gprs_make_gps_msg_imei(char *buf, struct gprmc_ *prmc)
{
	char tbuf[16];
	
	sprintf((void *)buf, "%s", "T:S:");
	sprintf((void *)tbuf, "%lf", prmc->lat);
	delete_zero_datastr((void *)tbuf);
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	sprintf((void *)tbuf, "%lf", prmc->lon);
	delete_zero_datastr((void *)tbuf);
	strcat((void *)buf, (void *)tbuf);
/*	strcat((void *)buf, ",");
	sprintf((void *)tbuf, "%lf", prmc->speed);
	delete_zero_datastr((void *)tbuf);
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	sprintf((void *)tbuf, "%lf", prmc->track);
	delete_zero_datastr((void *)tbuf);
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	sprintf((void *)tbuf, "%d", (prmc->status==STATUS_FIX) ? 1:0);
	strcat((void *)buf, (void *)tbuf);*/
	strcat((void *)buf, ",");
	memcpy((void *)tbuf, (void *)prmc->time.tm, 12);
	tbuf[12] = '\0';
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, ",");
	memcpy(tbuf, gprs_info.imei, 15);
	tbuf[15] = '\0';
	strcat((void *)buf, (void *)tbuf);
	strcat((void *)buf, "#");
}

#if defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_)
#include "app_usart.h"
static void app_gprs_send_gps_zgb(void)
{
	#define s_cmd "s:%lf,%lf,%lf,%lf,%d,%s#"
	int8_t flag;
	uint8_t cnt;
	struct gprmc_ *prmc;
	//int8_t time[13];
	int8_t buf[80];
	
	cnt = 1;
	while (cnt--) {
		//prmc = app_gps_gprmc_ft_read();
		prmc = app_gps_gprmc_lasted_read(&flag);

		if (NULL == prmc) {
			return;
		}

		app_gprs_make_gps_msg_imei((void *)buf, prmc);
		com_send_nchar(USART_ZGB_NUM, (void *)buf, strlen((void *)buf));
	}
}

/*app_gprs_discard_zgb: 旨在当未联网时，丢弃来自zigbee的数据*/
static void app_gprs_discard_zgb(void)
{
	#define L_MSG_MAX	128
	
	int cnt;
	int len;
	char msg[L_MSG_MAX];

	cnt = 0;
	while (cnt < 1280) {
		app_zgb_proc(msg, &len, L_MSG_MAX);
		if (len > 0) {
			//driv_gprs_send_msg(msg, len);
			cnt += len;
		} else {
			break;
		}
	}
}
#endif

static void app_gprs_send_gps(void)
{
	#define s_cmd "s:%lf,%lf,%lf,%lf,%d,%s#"
	int8_t flag;
	uint8_t cnt;
	struct gprmc_ *prmc;
	int8_t time[13];
	int8_t buf[80];
	
	cnt = 5;
	while (cnt--) {
		//prmc = app_gps_gprmc_ft_read();
		prmc = app_gps_gprmc_lasted_read(&flag);

		if (NULL == prmc) {
			return;
		}

		app_gprs_make_gps_msg((void *)buf, prmc);
		//app_gprs_make_gps_msg_imei(buf, prmc);
		driv_gprs_send_msg((void *)buf, strlen((void *)buf));
	}
}

#if defined (CAR_DB44_V1_0_20130315_)
static void app_gprs_send_rfid(void)
{
	uint32_t len;

	len = strlen((void *)g_rfid_send_gprs_msg);
	if (len > 0) {
		driv_gprs_send_msg((void *)g_rfid_send_gprs_msg, len);
		g_rfid_send_gprs_msg[0] = '\0';
	}
}
#endif

#if defined (CAR_DB44_V1_0_20130315_)
static void app_gprs_send_key(void)
{
	uint32_t len;

	len = strlen((void *)g_key_send_gprs_msg);
	if (len > 0) {
		driv_gprs_send_msg((void *)g_key_send_gprs_msg, len);
		g_key_send_gprs_msg[0] = '\0';
	}
}
#endif

#if (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_))

static void get_ORT(char *inbuf, int *start, char *tobuf)
{
	char *p1;
	char *p2;

	p1 = strstr(inbuf + *start, "ORT,"); 
	if (p1 == NULL) {
		tobuf[0] = '\0';
		return;
	}

	p1 += strlen("ORT,");
	p2 = strstr(p1, "#");
	if (p2 == NULL) {
		tobuf[0] = '\0';
		return;
	}

	strncpy(tobuf, p1, p2 - p1);
	tobuf[p2 - p1] = '\0';
	*start = p2 - inbuf + 1;
}

static void app_gprs_send_three_dimensional(void)
{
	#if defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_)

	#define DIM_INT_S	3
	#define L_MSG_MAX	128

	#define TEST_ZGB	0
	#define TEST_ZGB_PORT	USART_ZGB_NUM
	
	static uint64_t s_dim_ticks = 0;
	
	int cnt;
	int len;
	uint64_t lticks;
	char msg[L_MSG_MAX];
	//char graw[128];
	char imei[16];
	//char *p;

	#if TEST_ZGB
	int i;
	char buf[64];
	#endif

	lticks = ticks;

/*	cnt = 0;
	while (cnt++ < 10) {
		app_zgb_proc(&p);
		if (p != NULL) {
			driv_gprs_send_msg(p, strlen(p));
		} else {
			break;
		}
	}*/

	cnt = 0;
	while (cnt < 20) {
		app_zgb_proc(msg, &len, L_MSG_MAX);
		if (len > 0) {
			#if TEST_ZGB
			com_send_message(TEST_ZGB_PORT, "read ok:");
			for (i = 0; i < len; i++) {
				sprintf(buf, "%.2X (%c) ", msg[i], msg[i]);
				com_send_string(TEST_ZGB_PORT, buf);
			}
			com_send_message(TEST_ZGB_PORT, "");
			sprintf(buf, "%.2X,%.2X", msg[0], msg[len - 1]);
			com_send_message(TEST_ZGB_PORT, buf);
			#endif

			driv_gprs_send_msg(msg, len);
			cnt += len;
		} else {
			#if TEST_ZGB
			com_send_message(TEST_ZGB_PORT, "read empty:");
			#endif
			
			break;
		}
	}

	if (lticks >= s_dim_ticks + DIM_INT_S * HZ) {
		strcpy(msg, "T:M:");
		app_hmc5883l_bmp085_msg(msg + strlen(msg));
		memcpy(imei, gprs_info.imei, 15);
		imei[15] = '\0';
		strcat(msg, ",");
		strcat(msg, imei);		
		strcpy(msg + strlen(msg), "#");
		driv_gprs_send_msg(msg, strlen(msg));
		
		s_dim_ticks = lticks;
	}
	
	#else
	
	#define DIM_INT_S	5
	
	static uint32_t smy_ticks = 0;
	char msg[128];
	uint64_t lticks;

	lticks = ticks;
	if (lticks < smy_ticks + DIM_INT_S * HZ)
		return;
	smy_ticks = lticks;
	strcpy(msg, "T:H:");
	app_hmc5883l_bmp085_msg(msg + strlen(msg));
	strcpy(msg + strlen(msg), "#");
	driv_gprs_send_msg(msg, strlen(msg));
	
	#endif
}

#endif

static void app_gprs_send_tp_end(void) 
{
	#define TP_END_FLAG "+++"

	os_task_delayms(1000);
	driv_gprs_send_msg(TP_END_FLAG, strlen(TP_END_FLAG));
	os_task_delayms(1000);
}

static bool app_gprs_close(void)
{
	driv_gprs_send_msg(CLOSE_CON0, strlen(CLOSE_CON0));

	return true;
}

static void app_gprs_check_gprs_close(void)
{	
	if(app_gprs_get_seqed_msg("OK") /*返回OK*/)
	{
		gprs_info.gprs_status = GPRS_STATUS_NOINIT;
	}
	else if (app_gprs_get_seqed_msg("ERROR")){
		driv_gprs_send_msg(CLOSE_CON0, strlen(CLOSE_CON0));
	}
	else {
		//gprs_send_cmd(1, CLOSE_CON0, strlen(CLOSE_CON0));
	}
}

static void app_gprs_status_recoder_reset(void)
{
	gprs_err_recoder = GPRS_STATUS_TIMEOUT_S*HZ + ticks;
}

static bool app_gprs_status_recoder_read(void)
{
	return (gprs_err_recoder < ticks) ? true:false;
}

static void app_gprs_reset(void)
{
	#if 0
	driv_gprs_reset();
	os_task_delayms(4000);
	driv_gprs_norm();
	os_task_delayms(5000);
	
	#else
	driv_gprs_mon_norm();
	os_task_delayms(1000);
	driv_gprs_mon_reset();
	os_task_delayms(5000);
	driv_gprs_mon_norm();
	os_task_delayms(1000);
	driv_gprs_mon_reset();
	os_task_delayms(5000);
	#endif
}

static void app_gprs_check_status_err(void)
{
	static int8_t old_status;

	if (old_status != gprs_info.gprs_status) {
		old_status = gprs_info.gprs_status;
		app_gprs_status_recoder_reset();
	}
	if (app_gprs_status_recoder_read()) {
		if (old_status == GPRS_STATUS_SOCKET_OPEN) {
			/*如果接港务局专用sim卡，是不能解析域名的，会导致AT^SISO=0
			一直是的GPRS模块返回ERROR而不是"Remote host is unreachable"或者
			"Remote host has rejected the connection"*/
			srvs_next();
		}
		gprs_info.gprs_status = GPRS_STATUS_NOINIT;

		//driv_gprs_power_disable();
		driv_gprs_send_msg("app_gprs_check_status_err", strlen("app_gprs_check_status_err"));
		app_gprs_reset();
		app_gprs_status_recoder_reset();
		//gprs_send_cmd(1, "check_gprs_status_err\x00D\x00A", strlen("check_gprs_status_err\x00D\x00A"));
	}
}

static void process_net_reboot_cmd(void)
{
	#include "app_sys.h"
	
	//int32_t i;
	int32_t data;
	int32_t start;
	uint8_t buf[SEQED_MSGS_MAX_LEN];
	uint8_t *p;

	start = 0;
	//while ((start = lw_get_seqed_msg_body(start, "F:", buf, sizeof(buf)/sizeof(buf[0])) >= 0)) {
	while (1) {
		start = app_gprs_get_seqed_msg_body(start, "K:", buf, SEQED_MSGS_MAX_LEN);
		if (start < 0)
			break;
		p = (void *)strstr((void *)buf, "K:");
		if (!p) goto next;
		data = atoi((void *)(p+strlen("K:")));
		switch (data) {
		case 1:
			app_sys_reboot();
			break;
		case 2:
	//		del_files("/");
			break;
		case 3:
	//		del_jpgs("/");
			break;
		case 4:
	//		reset_gps();
			break;
		default:
			break;
		}

		next:
		start++;
	}
}

static void app_gprs_network_data(void)
{
	uint16_t val;
	int32_t ret;
	uint8_t buf[SEQED_MSGS_MAX_LEN];

	process_net_reboot_cmd();

	#if defined (CAR_DB44_V1_0_20130315_)
	
	val = 0;
	while (1) {
		ret = app_gprs_get_seqed_msg_body(val, "T:", buf, SEQED_MSGS_MAX_LEN);
		if (ret < 0) 
			break;
		app_network_data_proc((void *)buf);
		val++;
	}

/*	strcpy((void *)buf, "T:A:00C4E3BAC3##");
	app_network_data_proc((void *)buf);
	strcpy((void *)buf, "T:C:00C4E3BAC3##");
	app_network_data_proc((void *)buf);*/

	#endif
}

#if defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_)
#include "app_usart.h"
static void app_gprs_route_zgb(void)
{
	#define DIM_INT_S	4
	
	static uint64_t s_dim_ticks = 0;
	
	uint64_t lticks;
	char msg[128];
	char imei[16];

	lticks = ticks;

	if (lticks >= s_dim_ticks + DIM_INT_S * HZ) {
		strcpy(msg, "T:M:");
		app_hmc5883l_bmp085_msg(msg + strlen(msg));
		memcpy(imei, gprs_info.imei, 15);
		imei[15] = '\0';
		strcat(msg, ",");
		strcat(msg, imei);	
		strcpy(msg + strlen(msg), "#");
		com_send_nchar(USART_ZGB_NUM, msg, strlen(msg));
		
		s_dim_ticks = lticks;
	}
}
#endif

#if defined(CAR_TRUCK_1_5_140325_) || defined(CAR_TRUCK_1_3_140303_)
static char appcs_network_offline = 1;

static void app_gprs_cam_offline_init(void)
{
	appcs_network_offline = 1;
}

static void app_gprs_cam_proc(void)
{
	#define APP_GPRS_CAM_DATA_SIZE		300
	#define APP_GPRS_CAM_WF_ACK_TIMEOUT	8	/* s */
	#define APP_GPRS_CAM_SEND_INT			1	/* s 发送间隔 */
	#define APP_GPRS_CAM_PRECMD			(4 + 3 + 4 + 2 + 6)
	#define APP_GPRS_CAM_SEND_H 			"U:"

	enum app_gprs_cam_send_status_ {
		APCSS_FREE = 0,
		APCSS_WF_ACK,
		APCSS_RESEND
	};

	static enum app_gprs_cam_send_status_ appcs_s = APCSS_FREE;
	static int len;
	static int head;
	int index;
	unsigned int flen;
	static unsigned char *p;
	static uint64_t appcs_wf_ack_tick = 0;
	static uint64_t appcs_int_tick = 0;
	char buf[24];

	if (1 == appcs_network_offline) {
		appcs_network_offline = 0;
		if (APCSS_WF_ACK == appcs_s) {
			appcs_s = APCSS_RESEND;
		}
	} 

	switch (appcs_s) {
	case APCSS_FREE:
	if (appcs_int_tick + APP_GPRS_CAM_SEND_INT * HZ < ticks) {
		app_vc0706_frame(&p, &len, APP_GPRS_CAM_DATA_SIZE, &head);
		if (len > 0) {
			//len += APP_GPRS_CAM_PRECMD;
			flen = app_vc0706_frame_len();
			index = head / APP_GPRS_CAM_DATA_SIZE;
			sprintf(buf, "%s%.4X,%.3X,%.4X,%.2X#", APP_GPRS_CAM_SEND_H, flen, len, head, index);
			driv_gprs_send_msg(buf, strlen(buf));
			driv_gprs_send_msg(p, len);
			appcs_int_tick = ticks;
			appcs_wf_ack_tick = ticks;
			appcs_s = APCSS_WF_ACK;
		}
	}
	break;
	case APCSS_WF_ACK:
		/*if (1 == appcs_network_offline) {
			appcs_network_offline = 0;
			appcs_s = APCSS_RESEND;
		} else */
		{
			if (app_gprs_get_seqed_msg("U:")) {
				appcs_s = APCSS_FREE;
			} else {
				if (appcs_wf_ack_tick + APP_GPRS_CAM_WF_ACK_TIMEOUT * HZ < ticks)
					appcs_s = APCSS_RESEND;
			}
		}
	break;
	case APCSS_RESEND:
		flen = app_vc0706_frame_len();
		index = head / APP_GPRS_CAM_DATA_SIZE;
		sprintf(buf, "%s%.4X,%.3X,%.4X,%.2X#", APP_GPRS_CAM_SEND_H, flen, len, head, index);
		driv_gprs_send_msg(buf, strlen(buf));
		driv_gprs_send_msg(p, len);
		appcs_wf_ack_tick = ticks;
		appcs_s = APCSS_WF_ACK;
	break;
	default:
	break;
	}
}
#endif

static void app_gprs_network_offline(void)
{
#if defined(CAR_TRUCK_1_5_140325_) || defined(CAR_TRUCK_1_3_140303_)
	app_gprs_cam_offline_init();
#endif
}

void app_gprs_init(void)
{
	memset(g_IP_DNS, '\0', sizeof(g_IP_DNS));

	#if defined (CAR_DB44_V1_0_20130315_)
	g_rfid_send_gprs_msg[0] = '\0';
	#endif

	#if defined (CAR_DB44_V1_0_20130315_)
	g_key_send_gprs_msg[0] = '\0';
	#endif

	srvs_init();
	
	app_gprs_start_gprs_mode();
	app_gprs_seqed_msgs_init();
	driv_gprs_init();
	app_gprs_status_recoder_reset();
}

int32_t app_gprs_socket(void)
{
	app_gprs_check_status_err();
	app_gprs_network_data();

	#if defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_)
	if (gprs_info.gprs_status < GPRS_STATUS_SOCKET_TP_SUCC) {
		app_gprs_route_zgb();
		app_gprs_send_gps_zgb();
		app_gprs_discard_zgb();
	}
	#endif

	if (app_gprs_get_seqed_msg("START")) {
		//reset_gprs();
		gprs_info.gprs_status = GPRS_STATUS_NOINIT;
	}
	//else if (app_gprs_get_seqed_msg("closed") || (gprs_info.gprs_status ==GPRS_STATUS_SOCKET_OPEN))
	else if (app_gprs_get_seqed_msg("closed")) {
		gprs_info.gprs_status = GPRS_STATUS_SOCKET_TP_FINISH;
	}
	else if (app_gprs_get_seqed_msg("Remote host is unreachable") ||
			app_gprs_get_seqed_msg("Remote host has rejected the connection") || (app_gprs_get_seqed_msg("ERROR") && app_gprs_get_seqed_msg("^SIS:")) ||
			app_gprs_get_seqed_msg("^IPSTATE:")) {
		app_gprs_reset();
		gprs_info.gprs_status = GPRS_STATUS_NOINIT;
		srvs_next();
	}

	if (gprs_info.gprs_status > GPRS_STATUS_SOCKET_TP && gprs_info.gprs_status < GPRS_STATUS_SOCKET_TP_FINISH && app_gprs_get_seqed_msg("ERROR")) {
		app_gprs_reset();
		gprs_info.gprs_status = GPRS_STATUS_NOINIT;
		srvs_next();
	}

	switch(gprs_info.gprs_status)
	{
		case GPRS_STATUS_NOINIT:
			os_task_delayms(2000);
			app_gprs_start_gprs_mode();
			app_gprs_seqed_msgs_init();
			app_gprs_send_ate0();
			app_gprs_network_offline();
			break;
		case GPRS_STATUS_ATE:
			os_task_delayms(2000);
			app_gprs_send_get_imei();
			break;
		case GPRS_STATUS_GET_IMEI:
			os_task_delayms(2000);
			app_gprs_send_nettype();
			break;
		case GPRS_STATUS_NETTYPE:
			os_task_delayms(2000);
			app_gprs_send_apntype();
			break;
		case GPRS_STATUS_APNTYPE:
			os_task_delayms(2000);
			if(app_gprs_set_sbuf_imei() != 0 )
				return -1;
			app_gprs_send_srvtype();
			break;			
		case GPRS_STATUS_SRVTYPE:
			os_task_delayms(2000);
			app_gprs_send_conid();
			break;
		case GPRS_STATUS_CONID:
			os_task_delayms(2000);
			app_gprs_send_srvurl();
			break;
		case GPRS_STATUS_SRVURL:
			os_task_delayms(2000);
			app_gprs_send_socket_open();
			break;
		case GPRS_STATUS_SOCKET_OPEN:
			os_task_delayms(2000);
			app_gprs_check_socket_opend_succ();
			break;
		case GPRS_STATUS_SOCKET_OPEN_SUCC:
			os_task_delayms(2000);
			app_gprs_send_socket_tp_mode();
			break;
		case GPRS_STATUS_SOCKET_TP:
			os_task_delayms(2000);
			app_gprs_check_socket_tp_mode_succ();
			break;
		case GPRS_STATUS_SOCKET_TP_SUCC:
			if (true == gprs_info.send_imei_now) {
				gprs_info.send_imei_now = false;
				app_gprs_send_imei();
				//os_task_delayms(10000);
			}
			app_gprs_send_gps();

			#if defined (CAR_DB44_V1_0_20130315_)
			app_gprs_send_rfid();
			#endif

			#if defined (CAR_DB44_V1_0_20130315_)
			app_gprs_send_key();
			#endif

			#if (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_)|| defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_))
			app_gprs_send_three_dimensional();
			#endif

			#if defined(CAR_TRUCK_1_5_140325_) || defined(CAR_TRUCK_1_3_140303_)
			app_gprs_cam_proc();
			#endif
			
			app_gprs_status_recoder_reset();
			break;
		case GPRS_STATUS_SOCKET_TP_FINISH:
			app_gprs_send_tp_end();
			gprs_info.gprs_status = GPRS_STATUS_SOCKET_CLOSE;
			break;
		case GPRS_STATUS_SOCKET_CLOSE:
			app_gprs_close();
			gprs_info.gprs_status = GPRS_STATUS_SOCKET_CLOSE_OK;
			break;			
		case GPRS_STATUS_SOCKET_CLOSE_OK:
			app_gprs_check_gprs_close();
			break;
		default:
			break;
	}

	return 0;
}

void app_gprs_process_gprs_rbuf(uint8_t val)
{
	gprs_info.rbuf_info.rbuf[gprs_info.rbuf_info.rbuf_index++] = val;
	if(gprs_info.rbuf_info.rbuf_index >= GPRS_MAX_MSG_LEN)
		gprs_info.rbuf_info.rbuf_index = 0;
}

#endif
