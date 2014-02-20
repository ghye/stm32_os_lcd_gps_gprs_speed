#ifndef __APP_HMC5883L_BMP085_H__
#define __APP_HMC5883L_BMP085_H__

#include <stdint.h>
#include <stdbool.h>
#include "public.h"

struct adxl345_acc_ {
	float x;/*角度*/
	float y;
	float z;
	int ax;/*速度mg*/
	int ay;
	int az;
};

struct l3g4200d_ang_ {
	float x;/*角度*/
	float y;
	float z;
	int ax;	/*原数据*/
	int ay;
	int az;
};

struct hmc588cl_compass_ {
	float x;	/*角度*/
	float y;
	float z;
	int ax;	/*原数据*/
	int ay;
	int az;
};

void app_hmc5883l_bmp085_init(void);
void app_hmc5883l_bmp085(void);
void app_hmc5883l_bmp085_msg(char *msg);

#endif
