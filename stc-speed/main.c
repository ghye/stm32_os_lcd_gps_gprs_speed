#include <reg51.h>

#define FOSC	11059200
#define BAUD	1800

sfr WAKE_CLKO = 0x8f;

sbit port_out	=	P3^2;

struct speed_st_ {
	unsigned char flag;
	unsigned short speed;
};
struct speed_st_ speed_st;

static void usart_send_short(unsigned short val)
{
	//SBUF = val>>8;
	SBUF = (val>>7) & 0x7F;	/*第一个字节最高位为0*/
	while (!TI);
	TI = 0;
	//SBUF = val;
	SBUF = val | 0x80;		/*第二个字节最高位为1*/
	while (!TI);
	TI = 0;
}

void board_init(void)
{
	speed_st.flag = 0;
}

void main()
{
	board_init();

	//AUXR = 0x80;        //timer0 work in 1T mode
	WAKE_CLKO = 0x01;	//P3.4 OUT
	TMOD = 0x02;        //set timer0 as counter mode2 (8-bit auto-reload)
	TL0 = TH0 = 0;//256-(FOSC/12/32/BAUD);   //fill with 0xff to count one time
	TR0 = 1;            //timer0 start run
	ET0 = 1;            //enable T0 interrupt


	IT1 = 1;                    //set INT1 interrupt type (1:Falling only 0:Low level)
	EX1 = 1;                    //enable INT1 interrupt

	TI = 0;
	SCON = 0x50;
	TMOD          = TMOD | 0x20;                                      //Set Timer1 as 8-bit auto reload mode
	TH1            = TL1 = 256-(FOSC/12/32/9600);            //Set auto-reload vaule
	TR1            = 1;                                            //Timer1 start run
	//ES          = 1;                                           //Enable UART interrupt

	EA = 1;             //open global interrupt switch

	
	while (1) {
		if (speed_st.flag == 1) {
			usart_send_short(speed_st.speed);
			speed_st.flag = 0;
		}
	};
}


volatile unsigned short ext_speed = 0;
//T0 interrupt service routine
void t0int( ) interrupt 1      //T0 interrupt (location at 000BH)
{
	static unsigned short cnt = 0;

	cnt++;
	if (cnt >= BAUD*2/5) {
		speed_st.flag = 1;
		speed_st.speed= ext_speed;
		ext_speed = 0;
		cnt = 0;
	}
}

//External interrupt1 service routine
void exint1() interrupt 2             //INT1, interrupt 2 (location at 0013H)
{
	#if 0
	EA = 0;
	ext_speed++;
	EA = 1;

	#else
	if (port_out == 1) {
		port_out = 0;
		EA = 0;
		ext_speed++;
		EA = 1;
	}
	else
		port_out = 1;
	#endif
}

