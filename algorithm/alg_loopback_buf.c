#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "alg_loopback_buf.h"

void alg_lbb_init(int *insert, int *Read)
{
	*insert = 0;
	*Read = 0;
}

void alg_lbb_insert(char raw[], int rawMAX, int *insert, char v)
{
	raw[(*insert)++] = v;
	if (*insert >= rawMAX)
		*insert = 0;
}

void alg_lbb_get_single(char raw[], int rawMAX, int insert, int *Read, char *buf, int *len, int maxlen, unsigned char endflag)
{
	char flag = 0;
	int i;
	int linsert, lread;

	*len = 0;
	linsert = insert;
	lread = *Read;
	
	for (i = lread; i != linsert; ) {
		if (raw[i] == endflag /*'#'*/) {
			flag = 1;
			break;
		}
		i++;
		if (i >= rawMAX)
			i = 0;
	}

	if (flag == 1) {
		if (i >= lread) {
			if (maxlen >= i - lread + 1) {
				memcpy(buf, raw + lread, i - lread + 1);
				*len = i - lread + 1;
			}
		} else {
			if (maxlen >= rawMAX - lread + i + 1) {
				memcpy(buf, raw + lread, rawMAX - lread);
				*len = rawMAX - lread;
				memcpy(buf + *len, raw, i + 1);
				*len += i + 1;
			}
		}

		lread = i + 1;
		if (lread >= rawMAX) {
			lread = 0;
		}
		*Read = lread;
	} else {
		*len = 0;
	}
}

/*
返回值: 0 -没有数据
			1- 有数据
*/
int alg_lbb_get_char(char raw[], int rawMAX, int insert, int *Read, char *c)
{
	int r;
	int linsert, lread;

	linsert = insert;
	lread = *Read;

	r = 0;
	if (linsert != lread) {
		*c = raw[lread];
		r = 1;
		lread++;
		if (lread >= rawMAX)
			lread = 0;
		*Read = lread;
	}
	
	return r;
}
