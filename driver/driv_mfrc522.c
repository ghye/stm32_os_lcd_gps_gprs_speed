#include <string.h>

#include <stm32f10x_gpio.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_rcc.h>

#include "core.h"
#include "driv_mfrc522.h"
#include "driv_systick.h"

#if defined (CAR_DB44_V1_0_20130315_)

#define DRIV_MFRC522_DATA_LEN		128

#define I2C1_SLAVE_ADDRESS7	0x75
#define I2C_MFRC522_ADDRESS7	0x5E

#define I2C_TIMEOUT	((uint32_t)0x3F)
#define MFRC522_I2C	I2C1

#define MFRC522_RST_ON()	GPIO_ResetBits(GPIOB, GPIO_Pin_9)
#define MFRC522_RST_OFF()	GPIO_SetBits(GPIOB, GPIO_Pin_9)

#define MAXRLEN 18

struct driv_mfrc522_struct_ {
	uint16_t head;
	uint16_t tail;
	uint8_t data[DRIV_MFRC522_DATA_LEN];
};

struct driv_mfrc522_data_  {
	struct driv_mfrc522_struct_	wdata;
	struct driv_mfrc522_struct_	rdata;
};

struct driv_mfrc522_status_ {
	uint8_t status;
};

struct driv_mfrc522_data_ driv_mfrc522_data;
struct driv_mfrc522_status_ driv_mfrc522_status;

static void driv_mfrc522_data_init(void);
static void driv_mfrc522_status_init(void);
static void mfrc522_i2c_init(void);
static void mfrc522_gpio_init(void);
bool driv_mfrc522_reset(void);
bool driv_mfrc522_antenna_off(void);
bool driv_mfrc522_antenna_on(void);
bool driv_mfrc522_config_type(char type);

void driv_mfrc522_init(void)
{
	mfrc522_i2c_init();
	mfrc522_gpio_init();

	driv_mfrc522_reset();
	driv_mfrc522_antenna_off();
	driv_mfrc522_antenna_on();
	driv_mfrc522_config_type('A');

	driv_mfrc522_data_init();
	driv_mfrc522_status_init();
}

void driv_mfrc522_write(uint8_t *buf, uint32_t len)
{
	if (DRIV_MFRC522_DATA_LEN - driv_mfrc522_data.wdata.tail >= len) {
		memcpy(driv_mfrc522_data.wdata.data + driv_mfrc522_data.wdata.tail, buf, len);
		driv_mfrc522_data.wdata.tail += len;
		if (driv_mfrc522_data.wdata.tail >= DRIV_MFRC522_DATA_LEN)
			driv_mfrc522_data.wdata.tail = 0;
	} else {
		memcpy(driv_mfrc522_data.wdata.data + driv_mfrc522_data.wdata.tail, buf,
			DRIV_MFRC522_DATA_LEN - driv_mfrc522_data.wdata.tail);
		memcpy(driv_mfrc522_data.wdata.data, buf + DRIV_MFRC522_DATA_LEN - driv_mfrc522_data.wdata.tail,
			len - (DRIV_MFRC522_DATA_LEN - driv_mfrc522_data.wdata.tail));
		driv_mfrc522_data.wdata.tail = len - (DRIV_MFRC522_DATA_LEN - driv_mfrc522_data.wdata.tail);
		if (driv_mfrc522_data.wdata.tail >= DRIV_MFRC522_DATA_LEN)
			driv_mfrc522_data.wdata.tail = 0;
	}
}

void driv_mfrc522_read(uint8_t *buf, uint32_t *len)
{
	uint32_t head;
	uint32_t tail;
	
	head = driv_mfrc522_data.rdata.head;
	tail = driv_mfrc522_data.rdata.tail;
	
	if (head == tail) {
		*len = 0;
		return;
	}

	if (tail > head) {
		memcpy(buf, driv_mfrc522_data.rdata.data + head, tail - head);
		*len = tail - head;
		driv_mfrc522_data.rdata.head = tail;
	} else {
		memcpy(buf, driv_mfrc522_data.rdata.data + head, DRIV_MFRC522_DATA_LEN - head);
		*len = DRIV_MFRC522_DATA_LEN - head;
		memcpy(buf + *len, driv_mfrc522_data.rdata.data, tail);
		*len += tail;
		driv_mfrc522_data.rdata.head = tail;
	}
}

static void driv_mfrc522_data_init(void)
{
	driv_mfrc522_data.wdata.head = 0;
	driv_mfrc522_data.wdata.tail = 0;
	driv_mfrc522_data.rdata.head = 0;
	driv_mfrc522_data.rdata.tail = 0;
}

static void driv_mfrc522_status_init(void)
{
	driv_mfrc522_status.status = 0;
}

static void mfrc522_i2c_init(void)
{	
	#define ClockSpeed				100000
	
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;

	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, DISABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	
	/* Configure I2C1 pins: SCL and SDA ----------------------------------------*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_DeInit(I2C1);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;//I2C1_SLAVE_ADDRESS7 << 1;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
	I2C_Init(I2C1, &I2C_InitStructure);

	I2C_Cmd(I2C1, ENABLE);

	/* Enable I2C1 Error interrupt */
	I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
}

static void mfrc522_gpio_init(void)
{
	uint64_t i;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	MFRC522_RST_OFF();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_SetBits(GPIOE, GPIO_Pin_0);
}

static bool mfrc522_read_byte(uint8_t reg_addr, uint8_t *buf, uint32_t len)
{
	uint32_t I2C_TimeOut = I2C_TIMEOUT;
	uint64_t i;

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	I2C_TimeOut = I2C_TIMEOUT;
	while ((I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) && I2C_TimeOut)  /*!< EV5 */
	{ 
		i = ticks;
		i += 1;
		while (i > ticks) ;
		I2C_TimeOut--;
	}
	if (I2C_TimeOut == 0)
	{ 
		I2C_SoftwareResetCmd(I2C1, ENABLE);
		I2C_SoftwareResetCmd(I2C1, DISABLE);
		mfrc522_i2c_init();
		//driv_mfrc522_init();
	}
	
	/*!< Send LM75_I2C START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	/*!< Test on LM75_I2C EV5 and clear it */
	I2C_TimeOut = I2C_TIMEOUT;
	while ((!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB)) && I2C_TimeOut)  /*!< EV5 */
	{ 
		I2C_TimeOut--;
	}
	if (I2C_TimeOut == 0)
	{ 
		return false;
	}

	/*!< Send STLM75 slave address for write */
	I2C_Send7bitAddress(I2C1, I2C_MFRC522_ADDRESS7, I2C_Direction_Transmitter);
	I2C_TimeOut = I2C_TIMEOUT;
	while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && I2C_TimeOut)/* EV6 */
	{
		I2C_TimeOut--;
	}

	if ((I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) != 0x00) || (I2C_TimeOut == 0))
	{
		return false;
	}

	/* Send the device's internal address to write to */
	I2C_SendData(I2C1, reg_addr);

	/* Test on TXE FLag (data sent) */
	I2C_TimeOut = I2C_TIMEOUT;
	while ((!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)))
	{
		if((I2C_TimeOut--) == 0) 
			return false;
	}


	/* Send START condition a second time */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on SB Flag */
	I2C_TimeOut = I2C_TIMEOUT;
	while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB))
	{
		if((I2C_TimeOut--) == 0) 
			return false;
	}

	/* Send LM75 address for read */
	I2C_Send7bitAddress(I2C1, I2C_MFRC522_ADDRESS7, I2C_Direction_Receiver);

	/* Test on ADDR Flag */
	I2C_TimeOut = I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		if((I2C_TimeOut--) == 0) 
			return false;
	}

	while (len) {
		if (len == 1) {
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}

		I2C_TimeOut = I2C_TIMEOUT;
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			if((I2C_TimeOut--) == 0) 
				return false;
		}

		*buf = I2C_ReceiveData(I2C1);

		buf++;
		len--;
	}
	
	//os_task_delayms(1);
	
	/* Send STOP Condition */
	//I2C_GenerateSTOP(I2C1, ENABLE);

	return true;
}

static bool mfrc522_read_bytes(uint8_t reg_addr, uint8_t *buf, uint32_t len)
{
	#if 0
	bool f;
	
	while (len--) {
		f = mfrc522_read_byte(reg_addr/*++*/, buf++);
		if (f == false)
			break;
	}

	return f;
	#else
	return mfrc522_read_byte(reg_addr, buf, len);
	#endif
}

static bool mfrc522_write_byte(uint8_t reg_addr, uint8_t value)
{
	uint32_t I2C_TimeOut = I2C_TIMEOUT;
	uint64_t i;

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	I2C_TimeOut = I2C_TIMEOUT;
	while ((I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) && I2C_TimeOut)  /*!< EV5 */
	{ 
		i = ticks;
		i += 1;
		while (i > ticks) ;
		I2C_TimeOut--;
	}
	if (I2C_TimeOut == 0)
	{ 
		I2C_SoftwareResetCmd(I2C1, ENABLE);
		I2C_SoftwareResetCmd(I2C1, DISABLE);
		mfrc522_i2c_init();
		//driv_mfrc522_init();
	}
	
	/*!< Send LM75_I2C START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	/*!< Test on LM75_I2C EV5 and clear it */
	I2C_TimeOut = I2C_TIMEOUT;
	while ((!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB)) && I2C_TimeOut)  /*!< EV5 */
	{ 
		I2C_TimeOut--;
	}
	if (I2C_TimeOut == 0)
	{ 
		return false;
	}

	/*!< Send STLM75 slave address for write */
	I2C_Send7bitAddress(I2C1, I2C_MFRC522_ADDRESS7, I2C_Direction_Transmitter);

	I2C_TimeOut = I2C_TIMEOUT;
	while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && I2C_TimeOut)/* EV6 */
	{
		I2C_TimeOut--;
	}

	if ((I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) != 0x00) || (I2C_TimeOut == 0))
	{
		return false;
	}
	
	I2C_SendData(I2C1, reg_addr);
	I2C_TimeOut = I2C_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && I2C_TimeOut)
	{
		I2C_TimeOut--;
	}

	I2C_SendData(I2C1, value);
	I2C_TimeOut = I2C_TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && I2C_TimeOut)
	{
		I2C_TimeOut--;
	}

	/* Send STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);

	return true;
}

static bool mfrc522_write_bytes(uint8_t reg_addr, uint8_t *buf, uint32_t len)
{
	bool f;
	
	while (len--) {
		f = mfrc522_write_byte(reg_addr/*++*/,*(buf++));
		if (f == false)
			break;
	}

	return f;
}

static bool mfrc522_set_bits(uint8_t reg_addr, uint8_t mask)
{
	uint8_t v;

	if (!mfrc522_read_byte(reg_addr, &v, 1))
		return false;
	if (!mfrc522_write_byte(reg_addr, v | mask))
		return false;

	return true;
}

static bool mfrc522_clear_bits(uint8_t reg_addr, uint8_t mask)
{
	uint8_t v;

	if (!mfrc522_read_byte(reg_addr, &v, 1))
		return false;
	if (!mfrc522_write_byte(reg_addr, v & ~mask))
		return false;

	return true;
}

/*IT ERR interrupt process*/
void mfrc522_err_it(void)
{
	ITStatus s;
	
	s = I2C_GetITStatus(MFRC522_I2C, I2C_IT_SMBALERT);
	if (s) {
		I2C_ClearITPendingBit(MFRC522_I2C, I2C_IT_SMBALERT);
	}

	s = I2C_GetITStatus(MFRC522_I2C, I2C_IT_TIMEOUT);
	if (s) {
		I2C_ClearITPendingBit(MFRC522_I2C, I2C_IT_TIMEOUT);
	}

	s = I2C_GetITStatus(MFRC522_I2C, I2C_IT_PECERR);
	if (s) {
		I2C_ClearITPendingBit(MFRC522_I2C, I2C_IT_PECERR);
	}

	s = I2C_GetITStatus(MFRC522_I2C, I2C_IT_OVR);
	if (s) {
		I2C_ClearITPendingBit(MFRC522_I2C, I2C_IT_OVR);
	}

	s = I2C_GetITStatus(MFRC522_I2C, I2C_IT_AF);
	if (s) {
		I2C_ClearITPendingBit(MFRC522_I2C, I2C_IT_AF);
	}

	s = I2C_GetITStatus(MFRC522_I2C, I2C_IT_ARLO);
	if (s) {
		I2C_ClearITPendingBit(MFRC522_I2C, I2C_IT_ARLO);
	}

	s = I2C_GetITStatus(MFRC522_I2C, I2C_IT_BERR);
	if (s) {
		I2C_ClearITPendingBit(MFRC522_I2C, I2C_IT_BERR);
	}

	driv_mfrc522_status.status = 1;
}

bool driv_mfrc522_reset(void)
{
	bool f;
	uint8_t v;
	uint64_t i;
	
	MFRC522_RST_OFF();
	i = ticks;
	i += 1 * HZ / 500;
	while (i > ticks) ;
	MFRC522_RST_ON();
	i = ticks;
	i += 1 * HZ / 500;
	while (i > ticks) ;
	MFRC522_RST_OFF();
	i += 1 * HZ / 500;
	while (i > ticks) ;
	
	f = mfrc522_read_byte(CommandReg, &v, 1);
	if (!f)
		return false;
	f = mfrc522_write_byte(CommandReg, PCD_RESETPHASE);
	if (!f)
		return false;
	i = ticks;
	i += 1 * HZ / 500;
	while (i > ticks) ;
	f = mfrc522_write_byte(ModeReg, 0x3D);            /*和Mifare卡通讯，CRC初始值0x6363*/
	if (!f)
		return false;
	f = mfrc522_write_byte(TReloadRegL, 30);          
	if (!f)
		return false;
	f = mfrc522_write_byte(TReloadRegH, 0);
	if (!f)
		return false;
	f = mfrc522_write_byte(TModeReg, 0x8D);
	if (!f)
		return false;
	f = mfrc522_write_byte(TPrescalerReg, 0x3E);
	if (!f)
		return false;
	f = mfrc522_write_byte(TxAutoReg, 0x40); 
	if (!f)
		return false;

	return true;
}

bool driv_mfrc522_antenna_off(void)
{
	return mfrc522_clear_bits(TxControlReg, 0x03);
}

bool driv_mfrc522_antenna_on(void)
{
	uint8_t v;
	
	if (!mfrc522_read_byte(TxControlReg, &v, 1))
		return false;
	if (!(v & 0x03)) {
		return mfrc522_set_bits(TxControlReg, 0x03);
	}

	return true;
}

bool driv_mfrc522_config_type(char type)
{
	bool f = false;
	uint8_t v;
	uint64_t i;
	
	if (type == 'A')                     //ISO14443_A
	{
		f = mfrc522_clear_bits(Status2Reg, 0x08);
		if (!f)
			return false;

		/*     WriteRawRC(CommandReg,0x20);    //as default   
		WriteRawRC(ComIEnReg,0x80);     //as default
		WriteRawRC(DivlEnReg,0x0);      //as default
		WriteRawRC(ComIrqReg,0x04);     //as default
		WriteRawRC(DivIrqReg,0x0);      //as default
		WriteRawRC(Status2Reg,0x0);//80    //trun off temperature sensor
		WriteRawRC(WaterLevelReg,0x08); //as default
		WriteRawRC(ControlReg,0x20);    //as default
		WriteRawRC(CollReg,0x80);    //as default
		*/       
		f = mfrc522_write_byte(ModeReg, 0x3D);//3F
		if (!f)
			return false;
		f = mfrc522_read_byte(ModeReg, &v, 1);
		if (!f)
			return false;
		/*     WriteRawRC(TxModeReg,0x0);      //as default???
		WriteRawRC(RxModeReg,0x0);      //as default???
		WriteRawRC(TxControlReg,0x80);  //as default???

		WriteRawRC(TxSelReg,0x10);      //as default???
		*/    
		f = mfrc522_write_byte(RxSelReg, 0x86);//84
		if (!f)
			return false;
		//      WriteRawRC(RxThresholdReg,0x84);//as default
		//      WriteRawRC(DemodReg,0x4D);      //as default

		//      WriteRawRC(ModWidthReg,0x13);//26
		f = mfrc522_write_byte(RFCfgReg, 0x7F);   //4F
		if (!f)
			return false;
		/*   WriteRawRC(GsNReg,0x88);        //as default???
		WriteRawRC(CWGsCfgReg,0x20);    //as default???
		WriteRawRC(ModGsCfgReg,0x20);   //as default???
		*/
		f = mfrc522_write_byte(TReloadRegL, 30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		if (!f)
			return false;
		f = mfrc522_write_byte(TReloadRegH, 0);
		if (!f)
			return false;
		f = mfrc522_write_byte(TModeReg, 0x8D);
		if (!f)
			return false;
		f = mfrc522_write_byte(TPrescalerReg, 0x3E);
		if (!f)
			return false;


		//     PcdSetTmo(106);
		i = ticks;
		i += 1 * HZ / 500;
		while (i > ticks) ;
		f = driv_mfrc522_antenna_on();
		if (!f)
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：
//          cmd[IN]:RC522命令字
//          sbuf[IN]:通过RC522发送到卡片的数据
//          slen[IN]:发送数据的字节长度
//          rbuf[OUT]:接收到的卡片返回数据
//          rbitlen[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
static int8_t mfrc522_to_card(uint8_t cmd, uint8_t *sbuf, uint32_t slen, uint8_t *rbuf, uint32_t *rbitlen)
{
	#define WAIT_MS 26
	
	bool f, f1;
	int8_t s;
	uint8_t irqEn = 0;
	uint8_t waitFor = 0;
	uint8_t v, v1;
	uint32_t i;
	
	switch (cmd)
	{
	case PCD_AUTHENT:
		irqEn	= 0x12;
		waitFor	= 0x10;
	break;
	case PCD_TRANSCEIVE:
		irqEn	= 0x77;
		waitFor	= 0x30;
	break;
	default:
	break;
	}

	s = MI_ERR;
	f = mfrc522_write_byte(ComIEnReg, irqEn | 0x80);
	if (!f)
		return s;
	f = mfrc522_clear_bits(ComIrqReg, 0x80);
	if (!f)
		return s;
	f = mfrc522_write_byte(CommandReg, PCD_IDLE);
	if (!f)
		return s;
	f = mfrc522_set_bits(FIFOLevelReg, 0x80);
	if (!f)
		return s;

	f = mfrc522_write_bytes(FIFODataReg, sbuf, slen);
	if (!f)
		return s;

	f = mfrc522_write_byte(CommandReg, cmd);
	if (!f)
		return s;

	if (cmd == PCD_TRANSCEIVE) {
		f = mfrc522_set_bits(BitFramingReg, 0x80);
		if (!f)
			return s;
	}

	i = WAIT_MS;	/*操作M1卡最大等待时间25ms*/
	do
	{
		if (i != WAIT_MS)
			os_task_delayms(1);
		v1 = 0;
		f = mfrc522_read_byte(ComIrqReg, &v1, 1);
		if (!f)
			return s;
		i--;
	} while ((i != 0) && !(v1 & 0x01) && !(v1 & waitFor));
	f = mfrc522_clear_bits(BitFramingReg, 0x80);
	if (!f)
		return s;

	if (i != 0)
	{
		f = mfrc522_read_byte(ErrorReg, &v, 1);
		if (!f)
			return s;
		if (!(v & 0x1B)) {
			s = MI_OK;
		}
		if (v1 & irqEn & 0x01) {
			s = MI_NOTAGERR;		/*没有卡??*/
		}
		if (cmd == PCD_TRANSCEIVE) {
		//else if (cmd == PCD_TRANSCEIVE) {
			f = mfrc522_read_byte(FIFOLevelReg, &v, 1);
			f1 = mfrc522_read_byte(ControlReg, &v1, 1);
			if (f && f1) {
				v1 &= 0x07;
				if (v1) {
					*rbitlen = (v - 1) * 8 + v1;
				} else {
					*rbitlen = v * 8;
				}

				if (v == 0) {
					v = 1;
				}
				if (v > MAXRLEN)
					v = MAXRLEN;

				f = mfrc522_read_bytes(FIFODataReg, rbuf, v);
				if (!f)
					s = MI_ERR;
			} else {
				s = MI_ERR;
			}
		}
	}
	else {
		s = MI_ERR;
	}

	f = mfrc522_set_bits(ControlReg, 0x80);
	if (!f)
		s = MI_ERR;
	f = mfrc522_write_byte(CommandReg, PCD_IDLE);
	if (!f)
		s = MI_ERR;

	return s;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
static bool mfrc522_cal_crc(uint8_t *wbuf, uint32_t wlen, uint8_t *rbuf)
{
	bool f;
	uint8_t v;
	uint32_t i;
	
	f = mfrc522_clear_bits(DivIrqReg, 0x04);
	if (!f)
		return false;
	f = mfrc522_write_byte(CommandReg, PCD_IDLE);
	if (!f)
		return false;
	f = mfrc522_set_bits(FIFOLevelReg, 0x80);
	if (!f)
		return false;

	for (i=0; i<wlen; i++) {
		f = mfrc522_write_byte(FIFODataReg, *(wbuf + i));
		if (!f)
			return false;
	}

	f = mfrc522_write_byte(CommandReg, PCD_CALCCRC);
	if (!f)
		return false;

	i = 9;
	do {
		if (i != 9)
			os_task_delayms(1);
		v = 0;
		f = mfrc522_read_byte(DivIrqReg, &v, 1);
		if (!f)
			return false;
	} while (i && !(v & 0x04));

	f = mfrc522_read_byte(CRCResultRegL, &v, 1);
	if (!f)
		return false;
	rbuf[0] = v;
	f = mfrc522_read_byte(CRCResultRegM, &v, 1);
	if (!f)
		return false;	
	rbuf[1] = v;

	return true;
}

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: 
//          req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          type[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire 
//返    回: 成功返回true
/////////////////////////////////////////////////////////////////////
bool driv_rfid_check(uint8_t req_code, uint8_t *type)
{
	bool f;
	int8_t ff;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];
	
	f = mfrc522_clear_bits(Status2Reg, 0x08);
	if (!f)
		return false;
	f = mfrc522_write_byte(BitFramingReg, 0x07);
	if (!f)
		return false;

	f = mfrc522_set_bits(TxControlReg,0x03);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, &req_code, 1, rbuf, &rbitlen);
	if ((ff == MI_OK) && (rbitlen == 0x10)) {
		*type 		= rbuf[0];
		*(type + 1)	= rbuf[1];
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: snum[OUT]:卡片序列号，4字节
//返    回: 成功返回true
/////////////////////////////////////////////////////////////////////  
bool driv_rfid_anticoll(uint8_t *snum)
{
	bool f, f1;
	int8_t ff;
	uint8_t snr_check;
	uint32_t i;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];
	
	f = mfrc522_clear_bits(Status2Reg, 0x08);
	if (!f)
		return false;
	f = mfrc522_write_byte(BitFramingReg, 0x00);
	if (!f)
		return false;
	f = mfrc522_clear_bits(CollReg, 0x80);
	if (!f)
		return false;

	rbuf[0] = PICC_ANTICOLL1;
	rbuf[1] = 0x20;
	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 2, rbuf, &rbitlen);
	if (ff == MI_OK) {
		f = true;
		snr_check = 0;
		for (i=0; i<4; i++) {
			*(snum + i) = rbuf[i];
			snr_check ^= rbuf[i];
		}
		if (snr_check != rbuf[i])
			f = false;
	}

	f1 = mfrc522_set_bits(CollReg, 0x80);
	if (!f1)
		return false;
	
	return f;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: snum[IN]:卡片序列号，4字节
//返    回: 成功返回true
/////////////////////////////////////////////////////////////////////
bool driv_rfid_select(uint8_t *snum)
{
	bool f;
	int8_t ff;
	uint8_t i;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];

	rbuf[0] = PICC_ANTICOLL1;
	rbuf[1] = 0x70;
	rbuf[6] = 0;
	for (i=0; i<4; i++) {
		rbuf[i + 2] 	= *(snum + i);
		rbuf[6]		^=  *(snum + i);
	}

	f = mfrc522_cal_crc(rbuf, 7, &rbuf[7]);
	if (!f)
		return false;

	f = mfrc522_clear_bits(Status2Reg, 0x08);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 9, rbuf, &rbitlen);
	if ((ff == MI_OK) && (rbitlen == 0x18)) {
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: 
//          cmd[IN]: 密码验证模式命令
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥 
//          addr[IN]：块地址
//          pwd[IN]：密码
//          snum[IN]：卡片序列号，4字节
//返    回: 成功返回true
/////////////////////////////////////////////////////////////////////
bool driv_rfid_auth(uint8_t cmd, uint8_t addr, uint8_t *pwd, uint8_t *snum)
{
	bool f;
	int8_t ff;
	uint8_t v;
	uint32_t i;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];

	rbuf[0] = cmd;
	rbuf[1] = addr;

	for (i=0; i<6; i++) {
		rbuf[i + 2] = *(pwd + i);
	}
	for (i=0; i<6; i++) {
		rbuf[i + 8] = *(snum + i);
	}

	ff = mfrc522_to_card(PCD_AUTHENT, rbuf, 12, rbuf, &rbitlen);
	if (ff != MI_OK)
		return false;

	f = mfrc522_read_byte(Status2Reg, &v, 1);
	if (!f)
		return false;
	if (!(v & 0x08))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: 
//          addr[IN]：块地址
//          buf[OUT]：读出的数据，16字节
//返    回: 成功返回true  
///////////////////////////////////////////////////////////////////// 
bool driv_rfid_read(uint8_t addr, uint8_t *buf)
{
	bool f;
	int8_t ff;
	uint32_t i;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];

	rbuf[0] = PICC_READ;
	rbuf[1] = addr;
	f = mfrc522_cal_crc(rbuf, 2, &rbuf[2]);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 4, rbuf, &rbitlen);
	if ((ff == MI_OK) && (rbitlen == 0x90)) {
		for (i=0; i<16; i++) {
			*(buf + i) = rbuf[i];
		}
	} else {
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: 
//          addr[IN]：块地址
//          buf[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK  
/////////////////////////////////////////////////////////////////////  
bool driv_rfid_write(uint8_t addr, uint8_t *buf)
{
	bool f;
	int8_t ff;
	uint32_t i;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];

	rbuf[0] = PICC_WRITE;
	rbuf[1] = addr;
	f = mfrc522_cal_crc(rbuf, 2, &rbuf[2]);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 4, rbuf, &rbitlen);
	if ((ff != MI_OK) || (rbitlen != 4) || ((rbuf[0] & 0x0F) != 0x0A))
		return false;

	for (i=0; i<16; i++) {
		rbuf[i] = *(buf + i);
	}
	f = mfrc522_cal_crc(rbuf, 16, &rbuf[16]);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 18, rbuf, &rbitlen);
	if ((ff != MI_OK) || (rbitlen != 4) || ((rbuf[0] & 0x0F) != 0x0A))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: 
//          cmd[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          buf[IN]：4字节增(减)值，低位在前
//返    回: 成功返回true  
/////////////////////////////////////////////////////////////////////                 
bool driv_rfid_value(uint8_t cmd, uint8_t addr, uint8_t *buf)
{
	bool f;
	int8_t ff;
	uint32_t i;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];

	rbuf[0] = cmd;
	rbuf[1] = addr;
	f = mfrc522_cal_crc(rbuf, 2, &rbuf[2]);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 4, rbuf, &rbitlen);
	if ((ff != MI_OK) || (rbitlen != 4) || ((rbuf[0] & 0x0F) != 0x0A))
		return false;	

	for (i=0; i<16; i++) {
		rbuf[i] = *(buf + i);
	}
	f = mfrc522_cal_crc(rbuf, 4, &rbuf[4]);
	if (!f)
		return false;
	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 6, rbuf, &rbitlen);
	if (ff == MI_ERR) 
		return false;

	rbuf[0] = PICC_TRANSFER;
	rbuf[1] = addr;
	f = mfrc522_cal_crc(rbuf, 2, &rbuf[2]);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 4, rbuf, &rbitlen);
	if ((ff != MI_OK) || (rbitlen != 4) || ((rbuf[0] & 0x0F) != 0x0A))
		return false;

	return true;
}


/////////////////////////////////////////////////////////////////////
//功    能：备份钱包
//参数说明: 
//          saddr[IN]：源地址
//          daddr[IN]：目标地址
//返    回: 成功返回true  
/////////////////////////////////////////////////////////////////////
bool driv_bak_value(uint8_t saddr, uint8_t daddr)
{
	bool f;
	int8_t ff;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];

	rbuf[0] = PICC_RESTORE;
	rbuf[1] = saddr;
	f = mfrc522_cal_crc(rbuf, 2, &rbuf[2]);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 4, rbuf, &rbitlen);
	if ((ff != MI_OK) || (rbitlen != 4) || ((rbuf[0] & 0x0F) != 0x0A))
		return false;

	rbuf[0] = 0;
	rbuf[1] = 0;
	rbuf[2] = 0;
	rbuf[3] = 0;
	f = mfrc522_cal_crc(rbuf, 4, &rbuf[4]);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 6, rbuf, &rbitlen);
	if (ff == MI_ERR)
		return false;

	rbuf[0] = PICC_TRANSFER;
	rbuf[1] = daddr;
	f = mfrc522_cal_crc(rbuf, 2, &rbuf[2]);
	if (!f)
		return false;
	
	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 4, rbuf, &rbitlen);
	if ((ff != MI_OK) || (rbitlen != 4) || ((rbuf[0] & 0x0F) != 0x0A))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回true  
/////////////////////////////////////////////////////////////////////
bool driv_rfid_halt(void)
{
	bool f;
	int8_t ff;
	uint32_t rbitlen;
	uint8_t rbuf[MAXRLEN];

	rbuf[0] = PICC_HALT;
	rbuf[1] = 0;
	f = mfrc522_cal_crc(rbuf, 2, &rbuf[2]);
	if (!f)
		return false;

	ff = mfrc522_to_card(PCD_TRANSCEIVE, rbuf, 4, rbuf, &rbitlen);
	if (ff != MI_OK)
		return false;

	return true;
}

void driv_mfrc522_status_check(void)
{
	if (driv_mfrc522_status.status) {
		driv_mfrc522_status.status = 0;
		I2C_ClearFlag(MFRC522_I2C, I2C_IT_SMBALERT | I2C_IT_TIMEOUT | I2C_IT_PECERR | I2C_IT_OVR |
			I2C_IT_AF | I2C_IT_ARLO | I2C_IT_BERR);
		I2C_SoftwareResetCmd(MFRC522_I2C, ENABLE);
		I2C_SoftwareResetCmd(MFRC522_I2C, DISABLE);
		//mfrc522_i2c_init();
		driv_mfrc522_init();
	}
}

void driv_mfrc522_test(void)
{
	uint8_t c = 0;
	uint8_t buf[] = {1,2,3,4};

	if (driv_mfrc522_status.status) {
		driv_mfrc522_status.status = 0;
		I2C_ClearFlag(MFRC522_I2C, I2C_IT_SMBALERT | I2C_IT_TIMEOUT | I2C_IT_PECERR | I2C_IT_OVR |
			I2C_IT_AF | I2C_IT_ARLO | I2C_IT_BERR);
		I2C_SoftwareResetCmd(MFRC522_I2C, ENABLE);
		I2C_SoftwareResetCmd(MFRC522_I2C, DISABLE);
		mfrc522_i2c_init();
		//driv_mfrc522_init();
	}
	mfrc522_read_bytes(VersionReg, &c, 1);
return;

	mfrc522_i2c_init();
	
	/* Send data */
	while (c < sizeof(buf) / sizeof(buf[0]))
	{
		/* Send I2C1 data */
		I2C_SendData(I2C1, buf[c++]);
		/* Test on I2C1 EV8 and clear it */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
	}

	/* Send I2C1 STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);
}

#endif
