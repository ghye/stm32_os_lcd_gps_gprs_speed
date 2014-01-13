#define CMD_INIT		"\x1B\x40"
#define CAR_NO		"车牌号码：粤A26788\x0A"
#define CAR_TYPE	"车牌分类：大型\x0A"
#define DRV_ALIA		"汽车驾驶员代码：0000001\x0A"
#define DRV_NO		"驾驶证号码：440106201001311858\x0A"
#define PRI_TIME		"打印时间：2012/12/11 15:46:30\x0A"
#define POSTER_PARK	"停车前15分钟内每分钟平均速度：\x0A"
#define PARK_TIME	"停车日期：2012/12/11\x0A"
#define SPEED_1		"	15:45		60km/h\x0A"
#define POSTER_OT	"\x0A疲劳驾驶记录：\x0A"
#define RECORD		"记录00: 驾驶员代码 100005\x0A"
#define RECORD_STIME	"	开始时间 2012/12/11 09:12:04\x0A"
#define RECORD_ETIME	"	结束时间 2012/12/11 15:18:19\x0A"
#define TOTAL_KM	"\x0A总里程		367.3km\x0A"
#define TODAY_KM	"当天里程	344.5km\x0A"
#define TO_NAME		"\x0A\x0A\x0A\x0A驾驶员签名 ________\x0A"

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
