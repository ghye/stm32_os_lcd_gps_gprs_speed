unsigned char checksum_uchar(unsigned char *buf, int len)
{
	int i;
	unsigned int sum;

	sum = 0;
	for (i = 0; i < len; i++) {
		sum += *(buf + i);
	}

	while (sum >> 8) {
		sum = (sum & 0xFF) + (sum >> 8);
	}

	sum = ~sum;

	return (sum & 0xFF);
}
