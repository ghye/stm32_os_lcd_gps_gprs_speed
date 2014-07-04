#include "public.h"

#if defined(CAR_TRUCK_1_5_140325_) || defined(CAR_TRUCK_1_3_140303_)

#include <string.h>

#include <stm32f10x_usart.h>
#include <stm32f10x.h>

#include "core.h"
#include "driv_systick.h"
#include "driv_vc0706.h"
#include "app_usart.h"
#include "app_gps.h"
#include "util.h"
#include "driv_usart.h"

#define DRIV_VC0706_FLAG_RECV	0x56
#define DRIV_VC0706_FLAG_ACK	0x76

#define DRIV_VC0706_CMD_GET_VERSION	0x11
#define DRIV_VC0706_CMD_SYSTEM_RESET 0x26
#define DRIV_VC0706_CMD_READ_FBUF	0x32
#define DRIV_VC0706_CMD_GET_FBUF_LEN 0x34
#define DRIV_VC0706_CMD_FBUF_CTRL		0x36

#define DRIV_VC0706_FBUF_CURRENT_FRAME	0x00
#define DRIV_VC0706_FBUF_NEXT_FRAME		0x01
#define DRIV_VC0706_FBUF_START_FRAME		0x03

#define VB_DATA_LEN		( 5 * 2 )
#define DRIV_VC0706_CAM_INFO_LEN	128	/* 图片信息域长度，包括拍图片时的经纬度等 */

#define DRIV_VC0706_DATA_MAX	16000

#if defined(CAR_TRUCK_1_5_140325_) || defined(CAR_TRUCK_1_3_140303_)
#define USARTX	USART3
#endif

#define VC0706_RXNE_IRQ_ENABLE()	USART_ITConfig(USARTX, USART_IT_RXNE, ENABLE)
#define VC0706_RXNE_IRQ_DISABLE()	USART_ITConfig(USARTX, USART_IT_RXNE, DISABLE)

#if defined(CAR_TRUCK_1_5_140325_) || defined(CAR_TRUCK_1_3_140303_)
#define DRIV_VC0706_IR_GPIO		GPIOC
#define DRIV_VC0706_IR_GPIO_PIN	GPIO_Pin_4
#endif

#define DRIV_VC0706_IR_OPEN()	GPIO_ResetBits(DRIV_VC0706_IR_GPIO, DRIV_VC0706_IR_GPIO_PIN)
#define DRIV_VC0706_IR_CLOSE()	GPIO_SetBits(DRIV_VC0706_IR_GPIO, DRIV_VC0706_IR_GPIO_PIN)

struct driv_vc0706_inf_ {
	unsigned short	head;
	unsigned short	tail;
	unsigned int		frame_len;
	unsigned char		cdata[DRIV_VC0706_DATA_MAX];
};

/* 函数声明 */
static void driv_vc0706_send_msg(void *msg, int len);
static int driv_vc0706_fill_command(unsigned char *buf, unsigned char vc0706_flag,
	unsigned char sequence, unsigned char vc0706_cmd, unsigned char data_len, unsigned char *data);
static void driv_vc0706_form_start_cfbuf(unsigned char *buf, int *len);
static void driv_vc0706_clean_recv(void);
static bool driv_vc0706_reset(void);
static void driv_vc0706_send_set_size(int flag);
static void driv_vc0706_form_stop_cfbuf(unsigned char *buf, int *len);
static void driv_vc0706_send_stop_cfbuf(void);
static bool driv_vc0706_waitfor_fbuf_ctrl(void);
static void driv_vc0706_send_get_len(void);
static bool driv_vc0706_waitfor_get_len(void);
static void driv_vc0706_parse_frame_len(void);
static bool driv_vc0706_check_frame_len(void);
static unsigned char driv_vc0706_read_fbuf_trans_data_mode(bool isDMA /* MCU or DMA*/,
	unsigned char recv_port_type /*0: MCU串口, 1:　高速串口,  2: SPI*/,
	bool isSPIHost);
static void driv_vc0706_form_read_fbuf(unsigned int start_addr, unsigned int pic_len, unsigned short delay, unsigned char *buf, int *len);
static void driv_vc0706_send_read_fbuf(void);
static bool driv_vc0706_wait_recv_data(void);
static void driv_vc0706_set_cam_info(void);
static void driv_vc0706_ir_init(void);

static struct driv_vc0706_inf_ driv_vc0706_inf;

void driv_vc0706_get_data(unsigned char **buf, int *len, int glen, int *phead)
{
	unsigned short head, tail;

	head = driv_vc0706_inf.head;
	tail = driv_vc0706_inf.tail;
	
	if (head >= tail) {
		*buf = NULL;
		*len = 0;
		return;
	}

	glen = (tail - head > glen) ? glen:(tail - head);
	*buf = driv_vc0706_inf.cdata + head;
	*len = glen;
	*phead = head;
	driv_vc0706_inf.head += glen;
}

void driv_vc0706_nos_init(void)
{
	VC0706_RXNE_IRQ_DISABLE();
	driv_vc0706_clean_recv();
	driv_vc0706_ir_init();
	DRIV_VC0706_IR_CLOSE();
}

bool driv_vc0706_os_init(void)
{
	DRIV_VC0706_IR_OPEN();
	if (false == driv_vc0706_reset())
		goto fail;
	VC0706_RXNE_IRQ_DISABLE();
	driv_vc0706_clean_recv();
	VC0706_RXNE_IRQ_ENABLE();
	driv_vc0706_send_set_size(1);
	os_task_delayms(800);
	os_task_delayms(2000);			/* for IR open */
	VC0706_RXNE_IRQ_DISABLE();
	driv_vc0706_clean_recv();
	VC0706_RXNE_IRQ_ENABLE();
	driv_vc0706_send_stop_cfbuf();
	os_task_delayms(400);
	if (false == driv_vc0706_waitfor_fbuf_ctrl())
		goto fail;
	VC0706_RXNE_IRQ_DISABLE();
	driv_vc0706_clean_recv();
	VC0706_RXNE_IRQ_ENABLE();
	driv_vc0706_send_get_len();
	if (false == driv_vc0706_waitfor_get_len())
		goto fail;
	driv_vc0706_parse_frame_len();
	if (false == driv_vc0706_check_frame_len())
		goto fail;

	VC0706_RXNE_IRQ_DISABLE();
	driv_vc0706_clean_recv();
	VC0706_RXNE_IRQ_ENABLE();
	driv_vc0706_send_read_fbuf();
	if (false == driv_vc0706_wait_recv_data())
		goto fail;

	driv_vc0706_set_cam_info();
	VC0706_RXNE_IRQ_DISABLE();
	DRIV_VC0706_IR_CLOSE();
	return true;
	
fail:
	VC0706_RXNE_IRQ_DISABLE();
	driv_vc0706_clean_recv();
	DRIV_VC0706_IR_CLOSE();
	return false;
}

unsigned int driv_vc0706_frame_len(void)
{
	return driv_vc0706_inf.frame_len;
}

void driv_vc0706_isr(unsigned char v)
{
	unsigned char *p;

	p = driv_vc0706_inf.cdata + DRIV_VC0706_CAM_INFO_LEN;	/* 偏移预留的位置 */
	p[driv_vc0706_inf.tail] = v;
	driv_vc0706_inf.tail++;
	if (driv_vc0706_inf.tail >= DRIV_VC0706_DATA_MAX)
		driv_vc0706_inf.tail--;
}

static void driv_vc0706_send_msg(void *msg, int len)
{
	com_send_nchar(USART_CAM_NUM, msg, len);
}

static int driv_vc0706_fill_command(unsigned char *buf, unsigned char vc0706_flag,
	unsigned char sequence, unsigned char vc0706_cmd, unsigned char data_len, unsigned char *data)
{
	buf[0] = vc0706_flag;
	buf[1] = sequence;
	buf[2] = vc0706_cmd;
	buf[3] = data_len;
	memcpy(buf + 4, data, data_len);
	//buf[4+data_len] = '\0';
	return (4 + data_len);
}

static void driv_vc0706_form_start_cfbuf(unsigned char *buf, int *len)
{
	unsigned char data;
	
	data = DRIV_VC0706_FBUF_START_FRAME;
	*len = driv_vc0706_fill_command(buf, DRIV_VC0706_FLAG_RECV, 0x00,
		DRIV_VC0706_CMD_FBUF_CTRL, 0x01, &data);
}

static void driv_vc0706_clean_recv(void)
{
	driv_vc0706_inf.head = 0;
	driv_vc0706_inf.tail = 0;
}

static bool driv_vc0706_reset(void)
{
	#define DRIV_VC0706_RESET_TIME			600	/* ms */
	#define DRIV_VC0706_RESET_TINY_TIME	200		/* ms */

	int len;
	unsigned char *p;
	unsigned char buf[16];
	unsigned char cbuf[] = {0x36,0x00,0x00};

	VC0706_RXNE_IRQ_DISABLE();
	driv_vc0706_clean_recv();
	VC0706_RXNE_IRQ_ENABLE();
	
	driv_vc0706_form_start_cfbuf(buf, &len);
	driv_vc0706_send_msg(buf, len);

	len = DRIV_VC0706_RESET_TIME / DRIV_VC0706_RESET_TINY_TIME;
	while (len--) {
		os_task_delayms(DRIV_VC0706_RESET_TINY_TIME);
		if (driv_vc0706_inf.tail >= 5)
			break;
	}

	if (driv_vc0706_inf.tail < 5)
		goto fail;

	p = memstr(driv_vc0706_inf.cdata + DRIV_VC0706_CAM_INFO_LEN, driv_vc0706_inf.tail, cbuf, sizeof(cbuf));
	if (p) {
		VC0706_RXNE_IRQ_DISABLE();
		driv_vc0706_clean_recv();
		return true;
	}

fail:
	VC0706_RXNE_IRQ_DISABLE();
	driv_vc0706_clean_recv();
	return false;
}

static void driv_vc0706_send_set_size(int flag)
{
	unsigned char sz[5] = {0x56, 0x00, 0x54, 0x01, 0x11};

	switch (flag) {
	case 0:
		sz[4] = 0x00; /*640x480 */
		break;
	case 1:
		sz[4] = 0x11; /* 320x240 */
		break;
	case 2:
		sz[4] = 0x22; /* 160x120 */
		break;
	default:
		sz[4] = 0x11;
		break;
	}

	driv_vc0706_send_msg(sz, 5);
}

static void driv_vc0706_form_stop_cfbuf(unsigned char *buf, int *len)
{
	unsigned char data;
	
	data = DRIV_VC0706_FBUF_CURRENT_FRAME;
	*len = driv_vc0706_fill_command(buf, DRIV_VC0706_FLAG_RECV, 0x00,
		DRIV_VC0706_CMD_FBUF_CTRL, 0x01, &data);
}

static void driv_vc0706_send_stop_cfbuf(void)
{
	int len;
	unsigned char buf[16];
	
	driv_vc0706_form_stop_cfbuf(buf, &len);
	driv_vc0706_send_msg(buf, len);
}

static bool driv_vc0706_waitfor_fbuf_ctrl(void)
{
	#define WF_FB_CTRL_TIME		500
	#define WF_FB_CTRL_TINY_TIME	100

	int len;

	len = WF_FB_CTRL_TIME / WF_FB_CTRL_TINY_TIME;
	while (len--) {
		os_task_delayms(WF_FB_CTRL_TINY_TIME);
		if (driv_vc0706_inf.tail >= 5)
			break;
	}

	if (driv_vc0706_inf.tail < 5)
		return false;

	if (*(driv_vc0706_inf.cdata + DRIV_VC0706_CAM_INFO_LEN + 3))
		return false;
	
	return true;
}

static void driv_vc0706_send_get_len(void)
{
	int len;
	unsigned char data;
	unsigned char buf[16];

	data = DRIV_VC0706_FBUF_CURRENT_FRAME;
	len = driv_vc0706_fill_command(buf, DRIV_VC0706_FLAG_RECV, 0x00,
		DRIV_VC0706_CMD_GET_FBUF_LEN, 0x01, &data);
	driv_vc0706_send_msg(buf, len);
}

static bool driv_vc0706_waitfor_get_len(void)
{
	#define WF_GET_LEN_TIME		500
	#define WF_GET_LEN_TINY_TIME	100

	int len;

	len = WF_GET_LEN_TIME / WF_GET_LEN_TINY_TIME;
	while (len--) {
		os_task_delayms(WF_GET_LEN_TINY_TIME);
		if (driv_vc0706_inf.tail >= 9)
			break;
	}

	if (driv_vc0706_inf.tail < 9)
		return false;

	if (0x00 == *(driv_vc0706_inf.cdata + DRIV_VC0706_CAM_INFO_LEN + 4))
		return false;
	
	return true;
}

static void driv_vc0706_parse_frame_len(void)
{
	unsigned char size;
	unsigned char *p;
	unsigned char *pc;

	size = 4;
	p = (unsigned char *)&driv_vc0706_inf.frame_len;
	pc = driv_vc0706_inf.cdata + DRIV_VC0706_CAM_INFO_LEN + 5;
	while(size-- >0)
		*p++ = pc[size];
}

static bool driv_vc0706_check_frame_len(void)
{
	if (driv_vc0706_inf.frame_len + VB_DATA_LEN > DRIV_VC0706_DATA_MAX) /*当缓存比较少时就当作失败*/
		return false;

	return true;
}

static unsigned char driv_vc0706_read_fbuf_trans_data_mode(bool isDMA /* MCU or DMA*/,
	unsigned char recv_port_type /*0: MCU串口, 1:　高速串口,  2: SPI*/,
	bool isSPIHost)
{
	unsigned char mode;

	mode = 0x00;
	if(isDMA)
		mode |= 0x01;
	switch (recv_port_type)
	{
		case 0:
			mode |= 0x01 << 1;
			break;
		case 1:
			mode |= 0x10 << 1;
			break;
		case 2:
			mode |= 0x11 <<1;
			break;
		default:
			break;
	}

	if(isSPIHost)
		mode |= 0x01 << 3;

	return mode;
}

static void driv_vc0706_form_read_fbuf(unsigned int start_addr, unsigned int pic_len, unsigned short delay, unsigned char *buf, int *len)
{
	buf[0] = DRIV_VC0706_FLAG_RECV;
	buf[1] = 0x00;
	buf[2] = DRIV_VC0706_CMD_READ_FBUF;
	buf[3] = 0x0c;
	buf[4] = DRIV_VC0706_FBUF_CURRENT_FRAME;
	buf[5] = driv_vc0706_read_fbuf_trans_data_mode(0, 0, 1);
	
	buf[6] = (start_addr >> 24) & 0xff;	
	buf[7] = (start_addr >> 16) & 0xff;		
	buf[8] = (start_addr >> 8) & 0xff;
	buf[9] = start_addr & 0xff;

	buf[10] = (pic_len >> 24) & 0xff;	
	buf[11] = (pic_len >> 16) & 0xff;	
	buf[12] = (pic_len >> 8) & 0xff;
	buf[13] = pic_len & 0xff;
	
	buf[14] = (delay >> 8) & 0xff;
	buf[15] = delay & 0xff;
	
	*len = 16;
}

static void driv_vc0706_send_read_fbuf(void)
{
	int len;
	//unsigned char data;
	unsigned char buf[16];

	driv_vc0706_form_read_fbuf(0, driv_vc0706_inf.frame_len, 1/*0x0BB8*/, buf, &len);
	driv_vc0706_send_msg(buf, len);
}

static bool driv_vc0706_wait_recv_data(void)
{
	#define WF_RCV_DATA_TIME		6000
	#define WF_RCV_DATA_TINY_TIME	100

	int len;

	len = WF_RCV_DATA_TIME / WF_RCV_DATA_TINY_TIME;
	while (len--) {
		os_task_delayms(WF_RCV_DATA_TINY_TIME);
		if (driv_vc0706_inf.tail >= driv_vc0706_inf.frame_len + VB_DATA_LEN)
			break;
	}

	if (driv_vc0706_inf.tail < driv_vc0706_inf.frame_len + VB_DATA_LEN)
		return false;

	driv_vc0706_inf.frame_len += VB_DATA_LEN + DRIV_VC0706_CAM_INFO_LEN;	/* 加上之后方便发送图片时不用再加了 */
	driv_vc0706_inf.tail += DRIV_VC0706_CAM_INFO_LEN;	/* 加上以后，发送图片时，只需要判断head和tail的比较 */
	return true;
}

static void driv_vc0706_set_cam_info(void)
{
	unsigned char buf[24];
	
	memset(driv_vc0706_inf.cdata, '\0', DRIV_VC0706_CAM_INFO_LEN);
	sprintf((void *)driv_vc0706_inf.cdata, "%f", now_gps.lat);
	strcat((void *)driv_vc0706_inf.cdata, ",");
	sprintf((void *)buf, "%f", now_gps.lon);
	strcat((void *)driv_vc0706_inf.cdata, (void *)buf);
	strcat((void *)driv_vc0706_inf.cdata, ",");
	sprintf((void *)buf, "%f", now_gps.speed);
	strcat((void *)driv_vc0706_inf.cdata, (void *)buf);
	strcat((void *)driv_vc0706_inf.cdata, ",");
	sprintf((void *)buf, "%f", now_gps.track);
	strcat((void *)driv_vc0706_inf.cdata, (void *)buf);
	strcat((void *)driv_vc0706_inf.cdata, ",");
	sprintf((void *)buf, "%d", 1 /*now_gps.is_valid*/);
	strcat((void *)driv_vc0706_inf.cdata, (void *)buf);
	strcat((void *)driv_vc0706_inf.cdata, ",");
	memcpy((void *)(driv_vc0706_inf.cdata +strlen((void *)driv_vc0706_inf.cdata)), (void *)now_gps.time.tm, 12);
}

static void driv_vc0706_ir_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	#if defined(CAR_TRUCK_1_5_140325_) || defined(CAR_TRUCK_1_3_140303_)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	#endif
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = DRIV_VC0706_IR_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DRIV_VC0706_IR_GPIO, &GPIO_InitStructure);
}

#endif
