#ifndef __APP_HMC5883L_BMP085_H__
#define __APP_HMC5883L_BMP085_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

#define HMC5883L	1
#define GY26			2	/*ģ��:����PIC��Ƭ����HMC1022*/

#define COMPASS_MODEL	GY26

struct adxl345_acc_ {
	float x;/*�Ƕ�*/
	float y;
	float z;
	int ax;/*�ٶ�mg*/
	int ay;
	int az;
};

struct l3g4200d_ang_ {
	float x;/*�Ƕ�*/
	float y;
	float z;
	int ax;	/*ԭ����*/
	int ay;
	int az;
};

struct hmc588cl_compass_ {
	float x;	/*�Ƕ�*/
	float y;
	float z;
	int ax;	/*ԭ����*/
	int ay;
	int az;
};

void app_hmc5883l_bmp085_init(void);
void app_hmc5883l_bmp085(void);
void app_hmc5883l_bmp085_msg(char *msg);

#endif
