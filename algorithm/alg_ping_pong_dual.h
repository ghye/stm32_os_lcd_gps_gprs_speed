#ifndef __ALG_PING_PONG_DUAL_H__
#define __ALG_PING_PONG_DUAL_H__

struct alg_ppd_struct {
	char read;
};

void alg_ppd_init(struct alg_ppd_struct *pp);
char alg_ppd_get_read(struct alg_ppd_struct *pp);
char alg_ppd_get_write(struct alg_ppd_struct *pp);
void alg_ppd_swap(struct alg_ppd_struct *pp);

#endif
