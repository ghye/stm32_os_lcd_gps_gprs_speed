#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "core.h"
#include "app_usart.h"
#include "driv_usart.h"
#include "driv_hmc5883l_bmp085.h"
#include "app_hmc5883l_bmp085.h"

#if (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_))

struct adxl345_acc_ adxl345_acc;
struct l3g4200d_ang_ l3g4200d_ang;
struct hmc588cl_compass_ hmc588cl_compass;

//******************ADXL345计算倾斜角度************
static void adxl345_angle(struct adxl345_acc_ *adxl345_acc, int x, int y, int z)
{
	const double resolution = 31.2;
	short dx, dy, dz;
	double Q, T, K;
	double tmp;

	dx = x;
	dy = y;
	dz = z;

	Q = (double)dx * resolution;
	T = (double)dy * resolution;
	K = (double)dz * resolution;

	//Q = -Q;

	/*adxl345_acc->x = (float)((((double)atan2(K,Q)*180)/3.14159265)+180);    //X轴角度值
	adxl345_acc->y = (float)((((double)atan2(K,T)*180)/3.14159265)+180);*/    //Y轴角度值
	//adxl345_acc->z = (float)((((double)atan2(T,Q)*180)/3.14159265)+180);    //Z轴角度值

	/*adxl345_acc->x = (float)((((double)atan2(K,Q)*180)/3.14159265));    //X轴角度值
	adxl345_acc->y = (float)((((double)atan2(K,T)*180)/3.14159265));    //Y轴角度值
	adxl345_acc->z = (float)((((double)atan2(Q,K)*180)/3.14159265)); */   //Z轴角度值

	tmp  = sqrt((x*x+y*y)) / z;
	adxl345_acc->z = atan(tmp) * 180 / 3.14159265;
	tmp = x / sqrt((y*y+z*z));
	adxl345_acc->x = atan(tmp) * 180 / 3.14159265;
	tmp = y / sqrt((x*x+z*z));
	adxl345_acc->y = atan(tmp) * 180 / 3.14159265;

	adxl345_acc->ax = x * resolution;
	adxl345_acc->ay = y * resolution;
	adxl345_acc->az = z * resolution;
}

static void l3g4200d_angular(struct l3g4200d_ang_ *l3g4200d_ang, int x, int y, int z)
{
	l3g4200d_ang->x = x * 0.00875;
	l3g4200d_ang->y = y * 0.00875;
	l3g4200d_ang->z = z * 0.00875;

	l3g4200d_ang->ax = x;
	l3g4200d_ang->ay = y;
	l3g4200d_ang->az = z;
}

void app_hmc5883l_bmp085_init(void)
{
	driv_hmc5883l_bmp085_init();

	memset(&adxl345_acc, 0, sizeof(adxl345_acc));
	memset(&l3g4200d_ang, 0, sizeof(l3g4200d_ang));
	memset(&hmc588cl_compass, 0, sizeof(hmc588cl_compass));
}

static void u16ToSigned(int *x)
{
	(*x) -= 0xffff;
	(*x)--;
}

void app_hmc5883l_bmp085(void)
{
	uint8_t buf[8];
	char log[64];
	int x, y, z;
	double ax, ay, az;

#if 1
	if (driv_hmc5883l_bmp085_read_compass(buf)) {
		log[0] = '\0';
		x = (buf[0] << 8) | buf[1];
		if(x > 0x7fff)	u16ToSigned(&x);
		z = (buf[2] << 8) | buf[3];
		if(z > 0x7fff)	u16ToSigned(&z);
		y = (buf[4] << 8) | buf[5];
		if(y > 0x7fff)	u16ToSigned(&y);

		hmc588cl_compass.ax = x;
		hmc588cl_compass.ay = y;
		hmc588cl_compass.az = z;

		ax= atan2(y,x) * (180 / 3.14159265);// + 180; // angle in degrees
		ay= atan2(z,x) * (180 / 3.14159265);// + 180; // angle in degrees
		az= atan2(z,y) * (180 / 3.14159265);// + 180; // angle in degrees

		hmc588cl_compass.x = ax;
		hmc588cl_compass.y = ay;
		hmc588cl_compass.z = az;
		sprintf(log, "Cp:%lf,%lf,%lf,%d,%d,%d", hmc588cl_compass.x, hmc588cl_compass.y, hmc588cl_compass.z, 
			hmc588cl_compass.ax, hmc588cl_compass.ay, hmc588cl_compass.az);
		com_send_message(USART_GPS_NUM, (void *)log);
	}

	os_task_delayms(1);
#endif
#if 1
	/*读加速度*/
	if (driv_hmc5883l_bmp085_read_accelerometer(buf)) {
/*		x = (signed short)((buf[1] << 8) | buf[0]);
		y = (signed short)((buf[3] << 8) | buf[2]);
		z = (signed short)((buf[5] << 8) | buf[4]);
*/
		x = (buf[1] << 8) | buf[0];
		if(x > 0x7fff)	u16ToSigned(&x);
		y = (buf[3] << 8) | buf[2];
		if(y > 0x7fff)	u16ToSigned(&y);
		z = (buf[5] << 8) | buf[4];
		if(z > 0x7fff)	u16ToSigned(&z);
		
		adxl345_angle(&adxl345_acc, x, y, z);
		sprintf(log, "Ac:%lf,%lf,%lf,%d,%d,%d", adxl345_acc.x, adxl345_acc.y, adxl345_acc.z, 
			adxl345_acc.ax, adxl345_acc.ay, adxl345_acc.az);
		com_send_message(USART_GPS_NUM, (void *)log);

		/*x /= 32;
		y /= 32;
		z /= 32;
		sprintf(log, "a:%d,%d,%d", x, y, z);
		com_send_message(USART_GPS_NUM, (void *)log);*/
	}
	os_task_delayms(1);
#endif
#if 1
	if (driv_hmc5883l_bmp085_read_angular(buf)) {
/*		x = (signed short)((buf[1] << 8) | buf[0]);
		y = (signed short)((buf[3] << 8) | buf[2]);
		z = (signed short)((buf[5] << 8) | buf[4]);
*/
		x = (buf[1] << 8) | buf[0];
		if(x > 0x7fff)	u16ToSigned(&x);
		y = (buf[3] << 8) | buf[2];
		if(y > 0x7fff)	u16ToSigned(&y);
		z = (buf[5] << 8) | buf[4];
		if(z > 0x7fff)	u16ToSigned(&z);
		
		l3g4200d_angular(&l3g4200d_ang, x, y, z);
		sprintf(log, "Ag:%lf,%lf,%lf,%d,%d,%d", l3g4200d_ang.x, l3g4200d_ang.y, l3g4200d_ang.z, 
			l3g4200d_ang.ax, l3g4200d_ang.ay, l3g4200d_ang.az);
		com_send_message(USART_GPS_NUM, (void *)log);
	}
#endif
}

void app_hmc5883l_bmp085_msg(char *msg)
{
	msg[0] = '\0';
	sprintf(msg + strlen(msg), "%d,%d,%d;",hmc588cl_compass.ax, hmc588cl_compass.ay, hmc588cl_compass.az);
	sprintf(msg + strlen(msg), "%d,%d,%d;",adxl345_acc.ax, adxl345_acc.ay, adxl345_acc.az);
	sprintf(msg + strlen(msg), "%d,%d,%d",l3g4200d_ang.ax, l3g4200d_ang.ay, l3g4200d_ang.az);
}

#endif
