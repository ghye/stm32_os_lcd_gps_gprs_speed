#include "stm32f10x_gpio.h"

#define uint  unsigned int
#define uchar unsigned char
#define x     0x80
#define y     0x80
#define comm  0
#define dat   1

#define std_set()		GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define std_reset()	GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define sclk_set()		GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define sclk_reset()	GPIO_ResetBits(GPIOA, GPIO_Pin_5)

void wr_lcd (uchar dat_comm,uchar content);
void delay (uint us);
void delayu (uint us);   //delay time

uchar tab1[]={
"联物网络置二级字库，"
"可在任意位置反白显示"
};

uchar tab3[]={
"肇庆金鹏科技有限公司"
"TEL:2223196 2263858 "
};

uchar tab5[]={
/*--  调入了一幅图像：F:\梁\画图\HOCO16032.bmp  --*/
/*--  宽度x高度=160x32  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xE0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x30,0x18,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x03,0xFF,0x00,
0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x04,0xC4,0x00,0xFF,0x80,0x00,
0x00,0x07,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0xB0,0x00,0x00,0x00,0x00,0x0F,0xCC,
0x07,0xFF,0xF0,0x00,0x00,0x07,0xFF,0xC0,0x00,0x00,0x00,0x00,0x01,0x18,0x07,0xE0,
0x00,0x00,0x1F,0xF8,0x1F,0x00,0x78,0x00,0x00,0x07,0xFF,0xE0,0x00,0x00,0x00,0x00,
0x02,0x6C,0x0D,0x80,0x00,0x00,0x17,0xF0,0x3F,0xFF,0x18,0x00,0x00,0x03,0xFF,0xE0,
0x00,0x00,0x00,0x00,0x04,0x36,0x1F,0x00,0x00,0x00,0x3F,0xC0,0x7F,0x83,0xCC,0x00,
0x00,0x01,0xDF,0xE0,0x00,0x00,0x00,0x00,0x04,0xDB,0x3E,0x00,0x00,0x00,0x0F,0x80,
0xFF,0xFC,0x64,0x00,0x00,0x01,0xFF,0xF0,0x00,0x00,0x00,0x00,0x08,0x6D,0xFC,0x00,
0x00,0x00,0x7F,0xC1,0xFF,0xFF,0x24,0x00,0x00,0x01,0xDF,0xFF,0xFF,0x00,0x00,0x00,
0x09,0xB6,0xFC,0x00,0x00,0x00,0xFF,0xE1,0xFF,0xFF,0x34,0x00,0x00,0x00,0x1F,0xFF,
0xFF,0x80,0x00,0x00,0x08,0xDB,0xF8,0x00,0x00,0x01,0xFF,0xE3,0xFF,0xFF,0x94,0x00,
0x00,0x00,0x1F,0xFF,0xFF,0xC0,0x00,0x00,0x08,0x6D,0xF8,0x20,0x00,0x01,0xFF,0xE3,
0xFF,0xFF,0xA4,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xC0,0x00,0x00,0x08,0x37,0xF0,0x20,
0x00,0x01,0xFF,0xF3,0xFF,0xFF,0xA0,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xE0,0x00,0x00,
0x08,0x1F,0xF0,0x20,0x00,0x01,0xFF,0xFF,0xFF,0xFE,0x80,0x00,0x00,0x00,0x0F,0xFF,
0xFF,0xE0,0x00,0x00,0x08,0x0F,0xE0,0x20,0x00,0x01,0xFF,0xFF,0xFF,0xFE,0x80,0x00,
0x00,0x01,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x04,0x07,0xE0,0x40,0x00,0x01,0xFF,0xFF,
0xFF,0xEE,0x80,0x00,0x00,0x01,0xFB,0xFF,0xFF,0xF8,0x00,0x00,0x04,0x07,0xE0,0x40,
0x00,0x00,0xFF,0xFF,0xF7,0xEE,0x00,0x00,0x00,0x01,0x83,0x80,0x7F,0xF8,0x00,0x00,
0x02,0x03,0xC0,0x80,0x00,0x00,0x3F,0xFF,0xF7,0xEC,0x00,0x00,0x00,0x01,0x03,0x80,
0x7F,0x9C,0x00,0x00,0x01,0x03,0xC1,0x00,0x00,0x00,0x1F,0xFF,0xF7,0xE8,0x00,0x00,
0x00,0x01,0x03,0x00,0x3B,0x9E,0x00,0x00,0x00,0x83,0xC2,0x00,0x00,0x00,0x0F,0xFF,
0xE6,0xC0,0x00,0x00,0x00,0x03,0x07,0x00,0x3F,0x8F,0x00,0x00,0x00,0x43,0xC4,0x00,
0x00,0x00,0x07,0xFF,0x6D,0x80,0x00,0x00,0x00,0x03,0x86,0x00,0x1D,0xCD,0x00,0x00,
0x00,0x33,0xD8,0x00,0x00,0x00,0x01,0xFF,0x88,0x00,0x00,0x00,0x00,0x01,0x86,0x00,
0x0E,0xCE,0x80,0x00,0x00,0x0F,0xE0,0x00,0x00,0x00,0x00,0xCF,0x80,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x07,0xCF,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA3,
0x80,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x07,0x06,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0xC0,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x0E,0x06,0x00,0x00,
0xEE,0x8C,0xE8,0x49,0x00,0x00,0x01,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,
0x0C,0x00,0x00,0x00,0xAA,0x8A,0xA8,0xAF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x00,0x1C,0x00,0x00,0x00,0x8A,0x8A,0xE8,0xAF,0x00,0x00,0x02,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0xAA,0x8A,0x88,0xE9,
0x00,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x00,0x00,0x00,
0xEE,0xEC,0x8E,0xA9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


/*------------------初始化-----------------*/
void init_lcd (void)
{
  //rst=1;
  //psb=0;
  wr_lcd (comm,0x30);  /*30---基本指令动作*/
  wr_lcd (comm,0x01);  /*清屏，地址指针指向00H*/
  delay (80);
  wr_lcd (comm,0x06);  /*光标的移动方向*/
  wr_lcd (comm,0x0c);  /*开显示，关游标*/
}
/*---------------显示汉字或字符----------------*/
void chn_disp (uchar *chn)
{
  uchar i,j;
  wr_lcd (comm,0x30);
  wr_lcd (comm,0x80);
  j=0;
  for (i=0;i<20;i++)
  wr_lcd (dat,chn[j*20+i]);
  wr_lcd (comm,0x90);
  j=1;
  for (i=0;i<20;i++)
  wr_lcd (dat,chn[j*20+i]);
}
/*----------------显示图形-----------------*/
void img_disp (uchar *img)
{
  uchar i,j;
  for(j=0;j<32;j++)
  {
    for(i=0;i<10;i++)
    {
      wr_lcd (comm,0x34);
      wr_lcd (comm,y+j);
      wr_lcd (comm,x+i);
      wr_lcd (comm,0x30);
      wr_lcd (dat,img[j*20+i*2]);
      wr_lcd (dat,img[j*20+i*2+1]);
    }
  }
  wr_lcd (comm,0x36);
}
/*--------------显示点阵----------------*/
void lat_disp (uchar data1,uchar data2)
{
  uchar i,j;
  for(j=0;j<16;j++)
  {
    for(i=0;i<10;i++)
    {
      wr_lcd (comm,0x34);
      wr_lcd (comm,y+j*2);
      wr_lcd (comm,x+i);
      wr_lcd (comm,0x30);
      wr_lcd (dat,data1);
      wr_lcd (dat,data1);
    }
    for(i=0;i<10;i++)
    {
      wr_lcd (comm,0x34);
      wr_lcd (comm,y+j*2+1);
      wr_lcd (comm,x+i);
      wr_lcd (comm,0x30);
      wr_lcd (dat,data2);
      wr_lcd (dat,data2);
    }
  }
  wr_lcd (comm,0x36);
}
/*-----------------------------------------------*/
//当data1=0xff,data2=0xff时,在x0,y0处反白显示16xl*yl.
void con_disp (uchar data1,uchar data2,uchar x0,uchar y0,uchar xl,uchar yl)
{
  uchar i,j;
  for(j=0;j<yl;j++)
  {
    for(i=0;i<xl;i++)
    {
      wr_lcd (comm,0x34);
      wr_lcd (comm,y0+j);
      wr_lcd (comm,x0+i);
      wr_lcd (comm,0x30);
      wr_lcd (dat,data1);
      wr_lcd (dat,data2);
    }
  }
  wr_lcd (comm,0x36);
}
/*--------------清DDRAM------------------*/
void clrram (void)
{
  wr_lcd (comm,0x30);
  wr_lcd (comm,0x01);
  delay (120);
}
/*---------------------------------------*/
void wr_lcd (uchar dat_comm,uchar content)
{
  uchar a,i,j;
  //delay (40);
  a=content;
  delayu (1);
  sclk_reset();
  delayu (1);
  std_set();
  delayu (1);
  for(i=0;i<5;i++)
  {
    sclk_set();
  delayu (1);
    sclk_reset();
  delayu (1);
  }
  std_reset();
  delayu (1);
  sclk_set();
  delayu (1);
  sclk_reset();
  delayu (1);
  if(dat_comm)
    std_set();   //data
  else
    std_reset();   //command
  delayu (1);
  sclk_set();
  delayu (1);
  sclk_reset();
  delayu (1);
  std_reset();
  delayu (1);
  sclk_set();
  delayu (1);
  sclk_reset();
  delayu (1);
  for(j=0;j<2;j++)
  {
    for(i=0;i<4;i++)
    {
    	if (a & 0x80)
		std_set();
	else
		std_reset();
      a=a<<1;
      //std=CY;
  delayu (1);
      sclk_set();
  delayu (1);
      sclk_reset();
  delayu (1);
    }
    std_reset();
  delayu (1);
    for(i=0;i<4;i++)
    {
      sclk_set();
  delayu (1);
      sclk_reset();
  delayu (1);
    }
  }
}

void r_lcd (void)
{
  uchar a,i,j;
  //delay (40);
  //a=content;
  delayu (1);
  sclk_reset();
  delayu (1);
  std_set();
  delayu (1);
  for(i=0;i<5;i++)
  {
    sclk_set();
  delayu (1);
    sclk_reset();
  delayu (1);
  }
  std_set();//0;
  delayu (1);
  sclk_set();
  delayu (1);
  sclk_reset();
  delayu (1);
  if(0)
    std_set();   //data
  else
    std_reset();   //command
  delayu (1);
  sclk_set();
  delayu (1);
  sclk_reset();
  delayu (1);
  std_reset();
  delayu (1);
  sclk_set();
  delayu (1);
  sclk_reset();
  delayu (1);
  for(j=0;j<2;j++)
  {
    for(i=0;i<4;i++)
    {
      //a=a<<1;
      std_set();//CY;
  delayu (1);
      sclk_set();
  delayu (1);
      sclk_reset();
  delayu (1);
    }
    std_set();//0;
  delayu (1);
    for(i=0;i<4;i++)
    {
      sclk_set();
  delayu (1);
      sclk_reset();
  delayu (1);
    }
  }
}
/*---------------------------------------*/
void delayu (uint us)   //delay time
{
	us *= 10*6;
  while(us--);
}
void delay (uint us)   //delay time
{
	uchar i;
  while(us--)
	for (i=0; i<200; i++);
}
void delay1 (uint ms)
{
  uint i,j;
  for(i=0;i<ms;i++)
  for(j=0;j<15;j++)
  delay(1);
}

uchar tab_[]={
"网"
};
void chn_disp_ (void)
{
  uchar i,j;
  wr_lcd (comm,0x30);
  wr_lcd (comm,0x80);
  j=0;
  for (i=0;i<2;i++)
  wr_lcd (dat,tab_[j*20+i]);
}


