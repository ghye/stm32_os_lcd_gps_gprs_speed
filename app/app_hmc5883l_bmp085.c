#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "core.h"
#include "app_usart.h"
#include "driv_usart.h"
#include "driv_hmc5883l_bmp085.h"
#include "app_hmc5883l_bmp085.h"

struct adxl345_acc_ {
	float x;
	float y;
	float z;
};

//******************ADXL345计算倾斜角度************
static void adxl345_angle(struct adxl345_acc_ *adxl345_acc, int x, int y, int z)
{
	short dx, dy, dz;
	double Q, T, K;

	dx = x;
	dy = y;
	dz = z;

	Q = (double)dx * 3.9;
	T = (double)dy * 3.9;
	K = (double)dz * 3.9;

	Q = -Q;

	adxl345_acc->x = (float)((((double)atan2(K,Q)*180)/3.14159265)+180);    //X轴角度值
	adxl345_acc->y = (float)((((double)atan2(K,T)*180)/3.14159265)+180);    //Y轴角度值
	//adxl345_acc->z = (float)((((double)atan2(T,Q)*180)/3.14159265)+180);    //Z轴角度值
}

void app_hmc5883l_bmp085_init(void)
{
	driv_hmc5883l_bmp085_init();
}

void app_hmc5883l_bmp085(void)
{
	uint8_t buf[8];
	char log[64];
	int32_t x, y, z;
	double ax, ay, az;

	struct adxl345_acc_ adxl345_acc;

	/*if (driv_hmc5883l_bmp085_read_compass(buf)) {
		log[0] = '\0';
		x = (buf[0] << 8) | buf[1];
		if(x > 0x7fff)	x -= 0xffff;
		y = (buf[2] << 8) | buf[3];
		if(y > 0x7fff)	y -= 0xffff;
		z = (buf[4] << 8) | buf[5];
		if(z > 0x7fff)	z -= 0xffff;

		ax= atan2(y,x) * (180 / 3.14159265) + 180; // angle in degrees
		ay= atan2(z,x) * (180 / 3.14159265) + 180; // angle in degrees
		az= atan2(z,y) * (180 / 3.14159265) + 180; // angle in degrees
		sprintf(log, "C:%lf,%lf,%lf", ax, ay, az);
		com_send_message(USART_GPS_NUM, (void *)log);
	}

	os_task_delayms(1);*/

	if (driv_hmc5883l_bmp085_read_accelerometer(buf)) {
		x = (buf[1] << 8) | buf[0];
		y = (buf[3] << 8) | buf[2];
		z = (buf[5] << 8) | buf[4];

		/*sprintf(log, "A:%d,%d,%d", x, y, z);
		com_send_message(USART_GPS_NUM, (void *)log);*/

		adxl345_angle(&adxl345_acc, x, y, z);
		sprintf(log, "A:%f,%f,%f", adxl345_acc.x, adxl345_acc.y, adxl345_acc.z);
		com_send_message(USART_GPS_NUM, (void *)log);
	}
}
