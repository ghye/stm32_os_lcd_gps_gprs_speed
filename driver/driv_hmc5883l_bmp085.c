#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stm32f10x_gpio.h>
#include <stm32f10x_i2c.h>
#include <stm32f10x_rcc.h>

#include "core.h"

#include "driv_usart.h"
#include "app_usart.h"
#include "driv_hmc5883l_bmp085.h"

#define ADDR_HMC5883L	0x3C
#define ADDR_BMP085	0xEE
#define ADDR_ADXL345	0xA6

#define TIMEOUT	0x20

static void driv_hmc5883l_bmp085_i2c_init(void)
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

static void driv_hmc5883l_bmp085_gpio_init(void)
{

}

static bool driv_hmc5883l_bmp085_read_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buf, uint32_t len)
{
	uint32_t I2C_TimeOut = TIMEOUT;
	uint64_t i;

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	I2C_TimeOut = TIMEOUT;
	while ((I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) && I2C_TimeOut)  /*!< EV5 */
	{ 
		os_task_delayms(1);
		I2C_TimeOut--;
	}
	if (I2C_TimeOut == 0)
	{ 
		I2C_SoftwareResetCmd(I2C1, ENABLE);
		I2C_SoftwareResetCmd(I2C1, DISABLE);
		driv_hmc5883l_bmp085_i2c_init();
	}
	
	/*!< Send LM75_I2C START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	/*!< Test on LM75_I2C EV5 and clear it */
	I2C_TimeOut = TIMEOUT;
	while ((!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB)) && I2C_TimeOut)  /*!< EV5 */
	{ 
		os_task_delayms(1);	
		I2C_TimeOut--;
	}
	if (I2C_TimeOut == 0)
	{ 
		//com_send_message(USART_GPS_NUM, "r s1");
		return false;
	}

	/*!< Send STLM75 slave address for write */
	I2C_Send7bitAddress(I2C1, i2c_addr, I2C_Direction_Transmitter);
	I2C_TimeOut = TIMEOUT;
	while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && I2C_TimeOut)/* EV6 */
	{
		os_task_delayms(1);	
		I2C_TimeOut--;
	}

	if ((I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) != 0x00) || (I2C_TimeOut == 0))
	{
		com_send_message(USART_GPS_NUM, "r s2");
		return false;
	}

	/* Send the device's internal address to write to */
	I2C_SendData(I2C1, reg_addr);

	/* Test on TXE FLag (data sent) */
	I2C_TimeOut = TIMEOUT;
	while ((!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)))
	{
		os_task_delayms(1);
		if((I2C_TimeOut--) == 0) {
			com_send_message(USART_GPS_NUM, "r s3");
			return false;
		}
	}


	/* Send START condition a second time */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on SB Flag */
	I2C_TimeOut = TIMEOUT;
	while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB))
	{
		os_task_delayms(1);	
		if((I2C_TimeOut--) == 0) {
			com_send_message(USART_GPS_NUM, "r s4");
			return false;
		}
	}

	/* Send LM75 address for read */
	I2C_Send7bitAddress(I2C1, i2c_addr, I2C_Direction_Receiver);

	/* Test on ADDR Flag */
	I2C_TimeOut = TIMEOUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
		os_task_delayms(1);		
		if((I2C_TimeOut--) == 0) {
			com_send_message(USART_GPS_NUM, "r s5");
			return false;
		}
	}

	while (len) {
		if (len == 1) {
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}

		I2C_TimeOut = TIMEOUT;
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			os_task_delayms(1);	
			if((I2C_TimeOut--) == 0) {
				com_send_message(USART_GPS_NUM, "r s6");
				return false;
			}
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

static bool driv_hmc5883l_bmp085_write_byte(uint8_t i2c_addr, uint8_t reg_addr, uint8_t value)
{
	uint32_t I2C_TimeOut = TIMEOUT;
	uint64_t i;

	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	I2C_TimeOut = TIMEOUT;
	while ((I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY)) && I2C_TimeOut)  /*!< EV5 */
	{ 
		os_task_delayms(1);
		I2C_TimeOut--;
	}
	if (I2C_TimeOut == 0)
	{ 
		I2C_SoftwareResetCmd(I2C1, ENABLE);
		I2C_SoftwareResetCmd(I2C1, DISABLE);
		driv_hmc5883l_bmp085_i2c_init();
	}
	
	/*!< Send LM75_I2C START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	/*!< Test on LM75_I2C EV5 and clear it */
	I2C_TimeOut = TIMEOUT;
	while ((!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB)) && I2C_TimeOut)  /*!< EV5 */
	{ 
		os_task_delayms(1);
		I2C_TimeOut--;
	}
	if (I2C_TimeOut == 0)
	{ 
		//com_send_message(USART_GPS_NUM, "w s1");
		return false;
	}

	/*!< Send STLM75 slave address for write */
	I2C_Send7bitAddress(I2C1, i2c_addr, I2C_Direction_Transmitter);

	I2C_TimeOut = TIMEOUT;
	while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && I2C_TimeOut)/* EV6 */
	{
		os_task_delayms(1);
		I2C_TimeOut--;
	}

	if ((I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) != 0x00) || (I2C_TimeOut == 0))
	{
		//com_send_message(USART_GPS_NUM, "w s2");
		return false;
	}
	
	I2C_SendData(I2C1, reg_addr);
	I2C_TimeOut = TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && I2C_TimeOut)
	{
		os_task_delayms(1);
		I2C_TimeOut--;
	}

	I2C_SendData(I2C1, value);
	I2C_TimeOut = TIMEOUT;
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) && I2C_TimeOut)
	{
		os_task_delayms(1);
		I2C_TimeOut--;
	}

	/* Send STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);

	return true;
}

/*加速度计初始化*/
static bool driv_hmc5883l_bmp085_accelerometer_init(void)
{
	if (driv_hmc5883l_bmp085_write_byte(ADDR_ADXL345, 0x31, 0x0B) == false) {   //测量范围,正负16g，13位模式
		com_send_message(USART_GPS_NUM, "ADXL345 init err1");
		return false;
	}
	// Single_Write(ADXL345_Addr,0x2C,0x0e);   //速率设定为100hz 参考pdf13页
	if (driv_hmc5883l_bmp085_write_byte(ADDR_ADXL345, 0x2D, 0x08) == false) {   //选择电源模式   参考pdf24页
		com_send_message(USART_GPS_NUM, "ADXL345 init err2");
		return false;
	}
	if (driv_hmc5883l_bmp085_write_byte(ADDR_ADXL345, 0x2E, 0x80) == false) {   //使能 DATA_READY 中断
		com_send_message(USART_GPS_NUM, "ADXL345 init err3");
		return false;
	}
	// Single_Write(ADXL345_Addr,0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
	// Single_Write(ADXL345_Addr,0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
	// Single_Write(ADXL345_Addr,0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页

	return true;
}

void driv_hmc5883l_bmp085_init(void)
{
	driv_hmc5883l_bmp085_i2c_init();
	driv_hmc5883l_bmp085_gpio_init();

	//driv_hmc5883l_bmp085_accelerometer_init();
}

/*读罗盘*/
bool driv_hmc5883l_bmp085_read_compass(uint8_t *buf)
{
	if (driv_hmc5883l_bmp085_write_byte(ADDR_HMC5883L, 0x02, 0x00) == false) {
		com_send_message(USART_GPS_NUM, "5883 write err");
		return false;
	}

	os_task_delayms(10);

	if (driv_hmc5883l_bmp085_read_byte(ADDR_HMC5883L, 0x03, buf, 6) == false) {
		com_send_message(USART_GPS_NUM, "5883 read err");
		return false;
	}

	return true;
}

/*读加速度*/
bool driv_hmc5883l_bmp085_read_accelerometer(uint8_t *buf)
{
	static bool f = false;
	uint32_t i;

	if (!f) {
		driv_hmc5883l_bmp085_accelerometer_init();
		f = true;
		os_task_delayms(10);
	}

	#if 0
	i = 0;
	while (i++ < 6) {
		if (driv_hmc5883l_bmp085_read_byte(ADDR_ADXL345, 0x32 + i, buf + i, 1) == false) {
			com_send_message(USART_GPS_NUM, "ADXL345 read err");
			return false;
		}
	}
	#else
	if (driv_hmc5883l_bmp085_read_byte(ADDR_ADXL345, 0x32, buf, 6) == false) {
		com_send_message(USART_GPS_NUM, "ADXL345 read err");
		return false;
	}
	#endif

	return true;
}

