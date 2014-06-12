#include "public.h"
#include <string.h>
#include "driv_zgb.h"

#if defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_) || defined(HC_CONTROLER_) || defined(HC_CONTROLER_)

#include "alg_loopback_buf.h"
#include "app_usart.h"

#define ZGB_RAW_MAX	8000

int insert, Read;
char raw[ZGB_RAW_MAX];

#if defined(HC_CONTROLER_)
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "driv_net_usart.h"

#define RS485_ZGB1_RX_E_PIN		GPIO_Pin_11
#define RS485_ZGB2_RX_E_PIN		GPIO_Pin_12
#define RS485_ZGB1_RX_E_PORT		GPIOA
#define RS485_ZGB2_RX_E_PORT		GPIOB

void driv_zgb1_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RS485_ZGB1_RX_E_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_ZGB1_RX_E_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(RS485_ZGB1_RX_E_PORT, RS485_ZGB1_RX_E_PIN);
}

void driv_zgb2_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = RS485_ZGB2_RX_E_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RS485_ZGB2_RX_E_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(RS485_ZGB2_RX_E_PORT, RS485_ZGB2_RX_E_PIN);
}
#endif


void driv_zgb_init(void)
{
	#if defined(HC_CONTROLER_)
	driv_zgb1_gpio_init();
	#endif
	alg_lbb_init(&insert, &Read);
}

void driv_zgb_read_isr(uint8_t v)
{
	alg_lbb_insert(raw, ZGB_RAW_MAX, &insert, v);
}

void driv_zgb_get(char *buf, int *len, int maxlen)
{
	#define TEST_ZGB 0
	#define TEST_ZGB_PORT	USART_ZGB_NUM

	#if TEST_ZGB
	char str[64];

	sprintf(str, "bf insert:%d,read:%d", insert, Read);
	com_send_message(TEST_ZGB_PORT, (void *)str);
	#endif

	alg_lbb_get_single(raw, ZGB_RAW_MAX, insert, &Read, buf, len, maxlen, '#');

	#if TEST_ZGB
	sprintf(str, "af insert:%d,read:%d", insert, Read);
	com_send_message(TEST_ZGB_PORT, (void *)str);
	#endif
}

#if defined(HC_CONTROLER_)

int insert2, Read2;
char raw2[ZGB_RAW_MAX];

void driv_zgb2_init(void)
{
	driv_zgb2_gpio_init();
	alg_lbb_init(&insert2, &Read2);
}

void driv_zgb2_read_isr(uint8_t v)
{
	alg_lbb_insert(raw2, ZGB_RAW_MAX, &insert2, v);
}

void driv_zgb2_get(char *buf, int *len, int maxlen)
{
	#define TEST_ZGB 0
	#define TEST_ZGB_PORT	USART_ZGB_NUM

	#if TEST_ZGB
	char str[64];

	sprintf(str, "bf insert:%d,read:%d", insert2, Read2);
	com_send_message(TEST_ZGB_PORT, (void *)str);
	#endif

	alg_lbb_get_single(raw2, ZGB_RAW_MAX, insert2, &Read2, buf, len, maxlen, '#');

	#if TEST_ZGB
	sprintf(str, "af insert:%d,read:%d", insert2, Read2);
	com_send_message(TEST_ZGB_PORT, (void *)str);
	#endif
}
#endif

#if 0
#if defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) 

#define ZGB_RAW_NUM	400
#define ZGB_RAW_MAX	70

#elif defined(DouLunJi_CAR_TRUCK_1_3_140303_)
#define ZGB_RAW_NUM	200
#define ZGB_RAW_MAX	70
#endif

int insert, read;
int isr_next;
char raw[ZGB_RAW_NUM][ZGB_RAW_MAX];

void driv_zgb_init(void)
{
	insert = 0;
	read = 0;
	isr_next = 0;

	memset(raw, '\0', ZGB_RAW_MAX * ZGB_RAW_NUM);
}

void driv_zgb_read_isr(uint8_t v)
{
	raw[insert][isr_next] = v;
	isr_next++;
	if (isr_next >= ZGB_RAW_MAX) {
		isr_next = ZGB_RAW_MAX - 1;
	}

	if (v == '#') {
		raw[insert][isr_next] = '\0';
		insert++;
		if (insert >= ZGB_RAW_NUM)
			insert = 0;
		isr_next = 0;
	}	
}

void driv_zgb_get(char **buf)
{
	if (insert == read) {
		//buf[0] = '\0';
		*buf = NULL;
		return;
	}

	raw[read][ZGB_RAW_MAX - 1] = '\0';
//	strcpy(buf, raw[read]);
	*buf = &raw[read][0];

	read++;
	if (read >= ZGB_RAW_NUM)
		read = 0;
}
#endif

/*#define ZGB_RAW_MAX	64

int len;
char raw[ZGB_RAW_MAX];
char msg[ZGB_RAW_MAX];

void driv_zgb_init(void)
{
	len = 0;
	memset(msg, '\0', ZGB_RAW_MAX);
}

void driv_zgb_read_isr(uint8_t v)
{
	char *p1;
	char *p2;
	
	if (v == '#') {
		p1 = strstr(raw, "ORT,");
		if (p1 == NULL) {
			len = 0;
			return;
		}
		p1 += strlen("ORT,");
		raw[len] = '\0';
		strcpy(msg, p1);
		len = 0;
	} else {
		raw[len] = v;
		len++;
		if (len >= ZGB_RAW_MAX)
			len = 0;
	}
}

void driv_zgb_get(char *buf)
{
	strcpy(buf, msg);
	msg[0] = '\0';
}
*/

#if 0

#define ZGB_RAW_MAX	256

struct zgb_raw_info_ {
	uint16_t head;
	uint16_t tail;
	uint8_t raw[ZGB_RAW_MAX];
};

struct zgb_raw_info_ zgb_raw_info;

void driv_zgb_init(void)
{
	zgb_raw_info.head = 0;
	zgb_raw_info.tail = 0;
}

void driv_zgb_read_isr(uint8_t v)
{
	zgb_raw_info.raw[zgb_raw_info.tail] = v;
	zgb_raw_info.tail++;

	if (zgb_raw_info.tail >= ZGB_RAW_MAX) {
		zgb_raw_info.tail = 0;
	}
}

int32_t driv_zgb_get_single(char *buf)
{
	
}
#endif

#endif
