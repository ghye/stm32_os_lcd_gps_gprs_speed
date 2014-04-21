#ifndef __ALG_KALMAN_H__
#define __ALG_KALMAN_H__

void alg_kalman(int m, int n, int len, double f[], double d[], double u[], double b[], double q[], double h[], double r, double z[], double x[], double p[], double g[]);

#endif
