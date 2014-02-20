#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"

#include "core.h"
#include "driv_lcd.h"

#if defined (CAR_DB44_V1_0_20130315_)

#define LCD_LAMP_OFF()	GPIO_ResetBits(GPIOD, GPIO_Pin_14)
#define LCD_LAMP_ON()	GPIO_SetBits(GPIOD, GPIO_Pin_14)
#define LCD_RESET_ENABLE()	GPIO_ResetBits(GPIOD, GPIO_Pin_15)
#define LCD_RESET_DISABLE()	GPIO_SetBits(GPIOD, GPIO_Pin_15)
#define LCD_CS_HIGH()	GPIO_SetBits(GPIOA, GPIO_Pin_15)
#define LCD_CS_LOW()	GPIO_ResetBits(GPIOA, GPIO_Pin_15)
#define LCD_SPI_NAME	SPI3

#define COMMAND	0
#define DATA			1

#define DRIV_LCD_SPI_SEND(v) \
	/* Wait for SPIx Tx buffer empty */ \
	while (SPI_I2S_GetFlagStatus(LCD_SPI_NAME, SPI_I2S_FLAG_TXE) == RESET); \
	/* Send SPIx data */ \
	SPI_I2S_SendData(LCD_SPI_NAME, v);

static uint8_t DRIV_LCD_SPI_READ(void)
{
/* Wait to receive a byte */
    while(SPI_I2S_GetFlagStatus(LCD_SPI_NAME, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(LCD_SPI_NAME); 
}

static uint8_t driv_lcd_read_char(void)	
{
	uint8_t r;
	uint8_t v;
	
	//LCD_CS_HIGH();

	v = 0xF8 | 0x04 | 0x00;
	DRIV_LCD_SPI_SEND(v);
	DRIV_LCD_SPI_READ();

	DRIV_LCD_SPI_SEND(0xFF);
 	v = DRIV_LCD_SPI_READ();
	
	DRIV_LCD_SPI_SEND(0xFF);
 	r = DRIV_LCD_SPI_READ();

	r = (v & 0xF0) | (r & 0x0F);
	
	//LCD_CS_LOW();

	return r;
}

static void driv_lcd_read_idle_loop(void)
{
	uint8_t r;

	do {
		r = driv_lcd_read_char();
	} while (r & 0x80);
}

/*
	rs_flag: val是数据还是命令
*/
static void driv_lcd_write_char(bool rs_flag, uint8_t val)	
{
	uint8_t v;
	uint64_t i;

	//driv_lcd_read_idle_loop();
	/*i = ticks;
	i += 2;
	while (i > ticks) ;*/
	
	//LCD_CS_HIGH();

	v = 0xF8 | (rs_flag ? 0x02:0x00);
	//v =0xa5;
	DRIV_LCD_SPI_SEND(v);

 	//v = DRIV_LCD_SPI_READ();
	
	v = val & 0xF0;
	//v =0xa5;
	DRIV_LCD_SPI_SEND(v);

	v = val << 4;
	//v =0xa5;
	DRIV_LCD_SPI_SEND(v);
	
	//LCD_CS_LOW();

	//os_task_delayms(1);
	/*i = ticks;
	i += 2;
	while (i > ticks) ;*/
	
}

static void driv_lcd_fun_init(void)
{
	uint64_t i;
	printf("11");
	LCD_RESET_DISABLE();
	driv_lcd_write_char (COMMAND, 0x30);  /*30---基本指令动作*/
	driv_lcd_write_char (COMMAND, 0x01);  /*清屏，地址指针指向00H*/printf("22");
	//os_task_delayms(10);
	i = ticks;
	i += 2;
	while (i > ticks);
	driv_lcd_write_char (COMMAND, 0x06);  /*光标的移动方向*/
	driv_lcd_write_char (COMMAND, 0x0c);  /*开显示，关游标*/
}

static void driv_lcd_port_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
#if 1
	/* GPIOA, GPIOB and SPI3 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
#endif

#if 1
	/* Configure SPI3 pins: SCK, MISO and MOSI ----------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*Configure SPI3 pins: NSS*/
	/**/
#else
	/* Configure SPI1 pins: NSS, SCK, MISO and MOSI ----------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

	/*Configure SPI3 pins: NSS*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*LCD LAMP背光灯*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/*LCD reset*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

#if 1
	SPI_Cmd(LCD_SPI_NAME, DISABLE);
	/* SPI1 configuration ------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = /*SPI_CPOL_Low; */SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = /*SPI_CPHA_1Edge; */SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(LCD_SPI_NAME, &SPI_InitStructure);

	/* Enable SPI1 NSS output for master mode */
	SPI_SSOutputCmd(LCD_SPI_NAME, ENABLE);

	SPI_Cmd(LCD_SPI_NAME, ENABLE);
#endif
	//LCD_CS_LOW();
	LCD_CS_HIGH();

	LCD_LAMP_ON();
}

void driv_lcd_init(void)
{

	//#include "mul_lcd.h"
	
	driv_lcd_port_init();
	driv_lcd_fun_init();
		
	driv_lcd_tx_init_dma();

	/*driv_lcd_clean_all_dots_dma();
	driv_lcd_clrram_dma();*/

	//init_lcd ();
}

void driv_lcd_clrram(void)
{
	driv_lcd_write_char(COMMAND,0x30);
	driv_lcd_write_char(COMMAND,0x01);
	os_task_delayms(20);
}

/*
	row: 
		0	-	第一行
		2	-	第二行
	column:
		0~9
	说明:　因为该lcd显示中英文字符时，列的地址是指中文大小，即+1表示
			　一个中文字符；两个英文占一个中文字符的大小。需注意这个区别
*/
void driv_lcd_disp_addr(void *buf, uint32_t len, uint8_t row/*行*/, uint8_t column/*列*/)
{
	uint8_t addr;
	uint32_t cnt;
	uint8_t *p;

	if (0 == row)
		addr = 0x80;
	else
		addr = 0x90;
	addr += column;
	p = (uint8_t *)buf;
	driv_lcd_write_char(COMMAND, 0x30);
	driv_lcd_write_char(COMMAND, addr);

	for (cnt=0; cnt<len; cnt++) {
		driv_lcd_write_char(DATA, p[cnt]);
	}
}

void driv_lcd_clean_all_dots(void)
{
	uint8_t i,j;
	
	for(j=0; j<32; j++) {
		for(i=0; i<10; i++) {
			driv_lcd_write_char(COMMAND, 0x34);
			driv_lcd_write_char(COMMAND, 0x80+j);
			driv_lcd_write_char(COMMAND, 0x80+i);
			driv_lcd_write_char(COMMAND, 0x30);
			driv_lcd_write_char(DATA, 0);
			driv_lcd_write_char(DATA, 0);
		}
	}
	driv_lcd_write_char(COMMAND, 0x36);
}

/*
	length:	buf资源的长度，指像数个数，必须是16的倍数
	width:	buf资源的宽度，指像数个数
	row:		显示位置　行，指像数个数
	column:	显示位置　列，指像数个数，必须是16的倍数
*/
void driv_lcd_disp_dots_addr(void *buf, uint8_t length, uint8_t width, uint8_t row, uint8_t column)
{
	uint8_t i,j;
	uint8_t *p;

	p = (uint8_t *)buf;
	for(j=0; j<width; j++) {
		for(i=0; i<length/8/2; i++) {
			driv_lcd_write_char(COMMAND, 0x34);
			driv_lcd_write_char(COMMAND, 0x80+j+row);
			driv_lcd_write_char(COMMAND, 0x80+i+column/16);
			driv_lcd_write_char(COMMAND, 0x30);
			driv_lcd_write_char(DATA, p[j*length/8 +i*2]);
			driv_lcd_write_char(DATA, p[j*length/8 +i*2+1]);
		}
	}
	driv_lcd_write_char(COMMAND, 0x36);
}

/******************************DMA************************************/
/******************************DMA************************************/
#define LCD_TX_BASE  (&SPI3->DR)	//0x40013804
#define LCD_TXBUF_LEN	256

volatile bool lcd_dma_running;
DMA_InitTypeDef lcd_txDMA_InitStructure;
uint16_t lcd_tlen;
uint8_t lcd_tbuf[LCD_TXBUF_LEN];

/*返回未能存入dma buffer的长度*/
/*uint16_t driv_lcd_fill_data_dma(void *buf, uint16_t len)
{
	uint16_t l, i;
	uint8_t *p;

	i = 0;
	p = (uint8_t *)buf;
	for (; (lcd_tlen < LCD_TXBUF_LEN) && len; lcd_tlen++, len--) {
		lcd_tbuf[lcd_tlen] = p[i++];
	}

	return len;
}*/

static bool driv_lcd_dma_running(void)
{
	return lcd_dma_running;
}

static void driv_lcd_write_char_dma(bool rs_flag, uint8_t val)	
{
	uint8_t v;
	uint64_t i;

	while (driv_lcd_dma_running()) 
		os_task_delayms(1);
	
	if (lcd_tlen +3 >= LCD_TXBUF_LEN) {
		driv_lcd_tx_reload_dma();
		while (driv_lcd_dma_running()) 
			os_task_delayms(1);
	}
	v = 0xF8 | (rs_flag ? 0x02:0x00);
	lcd_tbuf[lcd_tlen++] = v;
	
	v = val & 0xF0;
	lcd_tbuf[lcd_tlen++] = v;

	v = val << 4;
	lcd_tbuf[lcd_tlen++] = v;
}


void driv_lcd_tx_init_dma(void)
{	
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_DeInit(DMA2_Channel2);  
	DMA_Cmd(DMA2_Channel2, DISABLE);
	lcd_txDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)LCD_TX_BASE;
	lcd_txDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;
	lcd_txDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	lcd_txDMA_InitStructure.DMA_BufferSize = 0; /* for future */
	lcd_txDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	lcd_txDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	lcd_txDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	lcd_txDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	lcd_txDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	lcd_txDMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	lcd_txDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel2, &lcd_txDMA_InitStructure);
	DMA_ITConfig(DMA2_Channel2, DMA_IT_TC, ENABLE);
	SPI_Cmd(SPI3, DISABLE);
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(SPI3, ENABLE);

	lcd_tlen = 0;
	lcd_dma_running = false;
}

void driv_lcd_tx_reload_dma(void)
{
	DMA_Cmd(DMA2_Channel2, DISABLE);
	lcd_txDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)lcd_tbuf;
	lcd_txDMA_InitStructure.DMA_BufferSize = lcd_tlen;
	DMA_Init(DMA2_Channel2, &lcd_txDMA_InitStructure);
	DMA_Cmd(DMA2_Channel2, ENABLE);

	lcd_dma_running = true;
}

void driv_lcd_tx_dma_enable(void)
{
	DMA_Cmd(DMA2_Channel2, ENABLE);

	lcd_dma_running = true;
}

void driv_lcd_tx_dma_disable(void)
{
	DMA_Cmd(DMA2_Channel2, DISABLE);

	lcd_tlen = 0;
	lcd_dma_running = false;
}

void driv_lcd_clrram_dma(void)
{
	driv_lcd_write_char_dma(COMMAND,0x30);
	driv_lcd_write_char_dma(COMMAND,0x01);
	driv_lcd_tx_reload_dma();

	os_task_delayms(10);
}

/*
	row: 
		0	-	第一行
		2	-	第二行
	column:
		0~9
	说明:　因为该lcd显示中英文字符时，列的地址是指中文大小，即+1表示
			　一个中文字符；两个英文占一个中文字符的大小。需注意这个区别
*/
void driv_lcd_disp_dma(void *buf, uint32_t len, uint8_t row/*行*/, uint8_t column/*列*/)
{
	uint8_t addr;
	uint32_t cnt;
	uint8_t *p;
	
	if (0 == row)
		addr = 0x80;
	else
		addr = 0x90;
	addr += column;
	p = (uint8_t *)buf;
	driv_lcd_write_char_dma(COMMAND, 0x30);
	driv_lcd_write_char_dma(COMMAND, addr);

	for (cnt=0; cnt<len; cnt++) {
		driv_lcd_write_char_dma(DATA, p[cnt]);
	}
}

void driv_lcd_clean_all_dots_dma(void)
{
	uint8_t i,j;
	
	for(j=0; j<32; j++) {
		for(i=0; i<10; i++) {
			driv_lcd_write_char_dma(COMMAND, 0x34);
			driv_lcd_write_char_dma(COMMAND, 0x80+j);
			driv_lcd_write_char_dma(COMMAND, 0x80+i);
			driv_lcd_write_char_dma(COMMAND, 0x30);
			driv_lcd_write_char_dma(DATA, 0);
			driv_lcd_write_char_dma(DATA, 0);
		}
	}
	driv_lcd_write_char_dma(COMMAND, 0x36);

	driv_lcd_tx_reload_dma();
}

/*
	length:	buf资源的长度，指像数个数，必须是16的倍数
	width:	buf资源的宽度，指像数个数
	row:		显示位置　行，指像数个数
	column:	显示位置　列，指像数个数，必须是16的倍数
*/
void driv_lcd_disp_dots_dma(void *buf, uint8_t length, uint8_t width, uint8_t row, uint8_t column)
{
	uint8_t i,j;
	uint8_t *p;

	p = (uint8_t *)buf;
	for(j=0; j<width; j++) {
		for(i=0; i<length/8/2; i++) {
			driv_lcd_write_char_dma(COMMAND, 0x34);
			driv_lcd_write_char_dma(COMMAND, 0x80+j+row);
			driv_lcd_write_char_dma(COMMAND, 0x80+i+column/16);
			driv_lcd_write_char_dma(COMMAND, 0x30);
			driv_lcd_write_char_dma(DATA, p[j*length/8 +i*2]);
			driv_lcd_write_char_dma(DATA, p[j*length/8 +i*2+1]);
		}
	}
	driv_lcd_write_char_dma(COMMAND, 0x36);
}

#endif
