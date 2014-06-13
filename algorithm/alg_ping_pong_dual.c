#include "alg_ping_pong_dual.h"

void alg_ppd_init(struct alg_ppd_struct *pp)
{
	pp->read = 0;
}

char alg_ppd_get_read(struct alg_ppd_struct *pp)
{
	return pp->read;
}

char alg_ppd_get_write(struct alg_ppd_struct *pp)
{
	return (1 - pp->read);
}

void alg_ppd_swap(struct alg_ppd_struct *pp)
{
	pp->read = 1 - pp->read;
}
