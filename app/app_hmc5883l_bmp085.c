#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "core.h"
#include "app_usart.h"
#include "driv_usart.h"
#include "driv_hmc5883l_bmp085.h"
#include "app_hmc5883l_bmp085.h"
#include "app_sys.h"


#if (defined(CAR_DB44_V1_0_20130315_) || defined(DouLunJi_CAR_GBC_V1_2_130511_) || defined(DouLunJi_AIS_BASE_STATION_V1_0_130513_) || defined(DouLunJi_CAR_TRUCK_1_3_140303_) || defined(CAR_TRUCK_1_5_140325_))

#define IF_CHECK_DIM_ERR	0	/*当发生读取失败后是否做相关处理*/

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

	#if 0
	adxl345_acc->ax = kalman_acx_proc(x);
 	adxl345_acc->ay = kalman_acy_proc(y);
	adxl345_acc->az = kalman_acz_proc(z);
	adxl345_acc->ax *= resolution;
	adxl345_acc->ay *= resolution;
	adxl345_acc->az *= resolution;
	#else
	adxl345_acc->ax = x * resolution;
	adxl345_acc->ay = y * resolution;
	adxl345_acc->az = z * resolution;
	#endif
}

static void l3g4200d_angular(struct l3g4200d_ang_ *l3g4200d_ang, int x, int y, int z)
{
	l3g4200d_ang->x = x * 0.00875;
	l3g4200d_ang->y = y * 0.00875;
	l3g4200d_ang->z = z * 0.00875;

	#if 0
	l3g4200d_ang->ax = kalman_anx_proc(x);
	l3g4200d_ang->ay = kalman_any_proc(y);
	l3g4200d_ang->az = kalman_anz_proc(z);
	#else
	l3g4200d_ang->ax = x;
	l3g4200d_ang->ay = y;
	l3g4200d_ang->az = z;
	#endif
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

#if 0
static int kalman_cx_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);
}

static int kalman_cy_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);
}

static int kalman_cz_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);
}

static int kalman_acx_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);
}

static int kalman_acy_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);


}

static int kalman_acz_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);
}

static int kalman_anx_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);
}

static int kalman_any_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);


}

static int kalman_anz_proc(int vi)
{
	double vd;

	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	static double x[1] = {0};//系统的状态向量Xk
	static double p[1] = {10};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	//double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	vd = (double)vi;
	alg_kalman(1/*n*/, 1/*m*/, 1/*len*/, f, d, u, b, q, h, r, &vd/*z*/, x, p, g);

	return (int)(x[0]);
}
#endif

void app_hmc5883l_bmp085(void)
{
	unsigned char fail = 0;
	uint8_t buf[8];
	char log[64];
	int x, y, z;
	double ax, ay, az;

#if 1
	#if (COMPASS_MODEL == HMC5883L)
	
	if (driv_hmc5883l_bmp085_read_compass(buf)) {
		log[0] = '\0';
		x = (buf[0] << 8) | buf[1];
		if(x > 0x7fff)	u16ToSigned(&x);
		z = (buf[2] << 8) | buf[3];
		if(z > 0x7fff)	u16ToSigned(&z);
		y = (buf[4] << 8) | buf[5];
		if(y > 0x7fff)	u16ToSigned(&y);

		#if 0
		x = kalman_cx_proc(x);
		hmc588cl_compass.ax = x;
		x = kalman_cy_proc(y);
		hmc588cl_compass.ay = x;
		x = kalman_cz_proc(z);
		hmc588cl_compass.az = x;
		#else
		hmc588cl_compass.ax = x;
		hmc588cl_compass.ay = y;
		hmc588cl_compass.az = z;
		#endif

		ax= atan2(y,x) * (180 / 3.14159265);// + 180; // angle in degrees
		ay= atan2(z,x) * (180 / 3.14159265);// + 180; // angle in degrees
		az= atan2(z,y) * (180 / 3.14159265);// + 180; // angle in degrees

		hmc588cl_compass.x = ax;
		hmc588cl_compass.y = ay;
		hmc588cl_compass.z = az;
		sprintf(log, "Cp:%lf,%lf,%lf,%d,%d,%d", hmc588cl_compass.x, hmc588cl_compass.y, hmc588cl_compass.z, 
			hmc588cl_compass.ax, hmc588cl_compass.ay, hmc588cl_compass.az);
		com_send_message(USART_GPS_NUM, (void *)log);
	} else {
		fail++;
	}

	#elif (COMPASS_MODEL == GY26)

	if (driv_hmc5883l_bmp085_read_compass_gy26(buf)) {
		hmc588cl_compass.ax = (buf[0] << 8) | buf[1];
	} else {
		fail++;
	}

	#endif

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
	} else {
		fail++;
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
	} else {
		fail++;
	}
#endif

	#if (IF_CHECK_DIM_ERR == 1)
	if (fail >= 3) {
		app_sys_reboot();
	}
	#endif
}

void app_hmc5883l_bmp085_msg(char *msg)
{
	msg[0] = '\0';

	#if (COMPASS_MODEL == HMC5883L)
	sprintf(msg + strlen(msg), "%d,%d,%d;",hmc588cl_compass.ax, hmc588cl_compass.ay, hmc588cl_compass.az);
	#elif (COMPASS_MODEL == GY26)
	sprintf(msg + strlen(msg), "%d;", hmc588cl_compass.ax);
	#endif
	
	sprintf(msg + strlen(msg), "%d,%d,%d;",adxl345_acc.ax, adxl345_acc.ay, adxl345_acc.az);
	sprintf(msg + strlen(msg), "%d,%d,%d",l3g4200d_ang.ax, l3g4200d_ang.ay, l3g4200d_ang.az);
}

#endif
