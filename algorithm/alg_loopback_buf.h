#ifndef __ALG_LOOPBACK_BUF_H__
#define __ALG_LOOPBACK_BUF_H__

void alg_lbb_init(int *insert, int *Read);
void alg_lbb_insert(char raw[], int rawMAX, int *insert, char v);
void alg_lbb_get_single(char raw[], int rawMAX, int insert, int *Read, char *buf, int *len, int maxlen, unsigned char endflag);
int alg_lbb_get_char(char raw[], int rawMAX, int insert, int *Read, char *c);

#endif
