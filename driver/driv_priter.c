#define CMD_INIT		"\x1B\x40"
#define CAR_NO		"���ƺ��룺��A26788\x0A"
#define CAR_TYPE	"���Ʒ��ࣺ����\x0A"
#define DRV_ALIA		"������ʻԱ���룺0000001\x0A"
#define DRV_NO		"��ʻ֤���룺440106201001311858\x0A"
#define PRI_TIME		"��ӡʱ�䣺2012/12/11 15:46:30\x0A"
#define POSTER_PARK	"ͣ��ǰ15������ÿ����ƽ���ٶȣ�\x0A"
#define PARK_TIME	"ͣ�����ڣ�2012/12/11\x0A"
#define SPEED_1		"	15:45		60km/h\x0A"
#define POSTER_OT	"\x0Aƣ�ͼ�ʻ��¼��\x0A"
#define RECORD		"��¼00: ��ʻԱ���� 100005\x0A"
#define RECORD_STIME	"	��ʼʱ�� 2012/12/11 09:12:04\x0A"
#define RECORD_ETIME	"	����ʱ�� 2012/12/11 15:18:19\x0A"
#define TOTAL_KM	"\x0A�����		367.3km\x0A"
#define TODAY_KM	"�������	344.5km\x0A"
#define TO_NAME		"\x0A\x0A\x0A\x0A��ʻԱǩ�� ________\x0A"

void driv_printer(void)
{
	uint8_t data[64];
	
	sprintf(data, "%s", CMD_INIT);
	fwrite(data, 2, 1, portfp);

	sprintf(data, "%s", CAR_NO);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", CAR_TYPE);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", DRV_ALIA);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", DRV_NO);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", PRI_TIME);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", POSTER_PARK);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", PARK_TIME);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", SPEED_1);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", POSTER_OT);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", RECORD);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", RECORD_STIME);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", RECORD_ETIME);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", TOTAL_KM);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", TODAY_KM);
	fwrite(data, strlen(data), 1, portfp);
	sprintf(data, "%s", TO_NAME);
	fwrite(data, strlen(data), 1, portfp);
}
