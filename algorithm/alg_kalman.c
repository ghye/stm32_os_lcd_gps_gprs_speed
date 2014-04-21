/*仅算一维*/
void alg_kalman(int m, int n, int len, double f[], double d[], double u[], double b[], double q[], double h[], double r, double z[], double x[], double p[], double g[])
{
	int i, j, k, k1;
	double y;
	double p1[1];
	double c[1];
	double s[1];
	double x1[1];
	double cc[1];

	for (k1 = 0; k1 < len; k1++) {
		for (i = 0; i < n; i++) {
			y = (double)0.0;
			for (j = 0; j < n; j++)
				y = y + f[i * n + j] * x[j];
			x1[i] = y + d[i] * u[k1];
		}
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				y = (double)0.0;
				for (k = 0; k < n; k++)
					y = y + f[i *n + k] * p[k * n + j];
				c[i * n + j] = y;
			}
		}
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				y = (double)0.0;
				for (k = 0; k < n; k++)
					y = y + c[i * n + k] * f[j *n + k];
				p1[i * n + j] = y;
			}
		}
		for (i = 0; i < n; i++) {
			for (j = 0; j < m; j++) {
				y = (double)0.0;
				for (k = 0; k < m; k++)
					y = y + b[i * m + k] * q[k * m + j];
				s[i * m + j] = y;
			}
		}
		for (i = 0; i <n; i++) {
			for (j = 0; j < n; j++) {
				y = (double)0.0;
				for (k = 0; k < m; k++)
					y = y + s[i * m + k] * b[j * m + k];
				p1[i * n + j] = y + p1[i * n + j];
			}
		}
		for (i = 0; i < n; i++) {
			y = (double)0.0;
			for (k = 0; k < n; k++)
				y = y + p1[i * n + k] * h[k];
			cc[i] = y;
		}
		y = (double)0.0;
		for (i = 0; i < n; i++)
			y = y + cc[i] * h[i];
		y = r + y;
		for (i = 0; i < n; i++)
			g[i] = cc[i] / y;
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				p[i * n + j] = p1[i * n + j] - g[i] * cc[j];
		y = (double)0.0;
		for (i = 0; i < n; i++)
			y = y + h[i] * x1[i];
		y = z[k1] - y;
		for (i = 0; i < n; i++)
			x[i] = x1[i] + g[i] * y;
	}
}

#if 0
int main(void)
{
	int i, j, m, n, len;
	double f[1] = {1};//系统状态转移矩阵Ok,k-1
	double b[1] = {0.1};//动态噪声的系数矩阵Bk
	double d[1] = {0};//系统输入信号的系数向量Dk
	double h[1] = {1};//系统的观测矩阵Hk
	double q[1] = {0.1};//动态噪声的协方差矩阵Qk
	double x[1] = {0};//系统的状态向量Xk
	double p[1] = {5};//系统的滤波误差方差矩阵Pk
	double u[1] = {0};//系统的输入信号Vk
	double z[] = {120, 121, 122, 123, 144, 145, 146, 120};//系统的观测值Zk
	double g[1], r = 0.1;//g系统的卡尔曼滤波增益Kk，r观测噪声的协方差Rk

	m = 1;
	n = 1;
	len = sizeof(z) / sizeof(double);
	double *pz;
	int c;

	for (c = 0; c < len; c++) {
		pz = z + c;
		kalman(n, m, 1/*len*/, f, d, u, b, q, h, r, pz/*z*/, x, p, g);
		printf("%10.8lf\n",x[0]);
	}

	return 0;

}
#endif
