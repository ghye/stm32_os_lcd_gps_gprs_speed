#ifndef __ALG_PING_PONG_DUAL_BUF_H__
#define __ALG_PING_PONG_DUAL_BUF_H__

#include "alg_ping_pong_dual.h"

struct alg_ppdb_struct {
	struct alg_ppd_struct ppd;
	int max;
	int tail[2];
	unsigned char *base;
};

void alg_ppdb_init(struct alg_ppdb_struct *ppdb, unsigned char *base, int max);
void alg_ppdb_write_inf(struct alg_ppdb_struct *ppdb, unsigned char **buf, int *len);
void alg_ppdb_swap(struct alg_ppdb_struct *ppdb);
int alg_ppdb_write(struct alg_ppdb_struct *ppdb, unsigned char *buf, int len);

#endif
