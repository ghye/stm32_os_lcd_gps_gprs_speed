#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "alg_ping_pong_dual.h"
#include "alg_ping_pong_dual_buf.h"

void alg_ppdb_init(struct alg_ppdb_struct *ppdb, unsigned char *base, int max)
{
	alg_ppd_init(&ppdb->ppd);
	memset(ppdb->tail, 0, sizeof(ppdb->tail));
	ppdb->base = base;
	ppdb->max = max;
}

void alg_ppdb_write_inf(struct alg_ppdb_struct *ppdb, unsigned char **buf, int *len)
{
	char write;
	
	write = alg_ppd_get_write(&ppdb->ppd);
	*buf = ppdb->base + write * ppdb->max;
	*len = (ppdb->tail)[write];
}

void alg_ppdb_swap(struct alg_ppdb_struct *ppdb)
{
	char read;
	
	read = alg_ppd_get_read(&ppdb->ppd);
	(ppdb->tail)[read] = 0;
	
	alg_ppd_swap(&ppdb->ppd);
}

int alg_ppdb_write(struct alg_ppdb_struct *ppdb, unsigned char *buf, int len)
{
	char write;

	write = alg_ppd_get_write(&ppdb->ppd);
	if (ppdb->max - (ppdb->tail)[write] < len)
		return 0 ;
	memcpy(ppdb->base + write * ppdb->max + (ppdb->tail)[write], buf, len);
	(ppdb->tail)[write] += len;
	return len;
}

