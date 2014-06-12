static unsigned int g_cpu_id[3];

void driv_cpu_id_init(void)
{
	//ªÒ»°CPUŒ®“ªID
	g_cpu_id[0] = *(unsigned int *)(0x1ffff7e8);
	g_cpu_id[1] = *(unsigned int *)(0x1ffff7ec);
	g_cpu_id[2] = *(unsigned int *)(0x1ffff7f0);
}

void driv_cpu_id_get(unsigned int **buf)
{
	*buf = g_cpu_id;
}
