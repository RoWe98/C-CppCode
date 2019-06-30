/* 下载程序后按键K1按下就会触发蜂鸣器门铃“叮咚”响  */

#include "reg52.h"
#include "LCD.h"
#include "stdlib.h"
#include "stdio.h"
#include "temp.h"

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

int pressflag = 0;

u8 Disp[11] = "New Guests";
u8 Disp_Hello[14] = "Welcome~!!!";
u8 Disp_Test[9] = "Guests:";

sbit beep=P1^5;
sbit k1=P3^1;
sbit k2=P3^0;
sbit k3=P3^2;

u8 ding,dong,flag,stop;
u16 n;


u16 j=0;
u32 q=0;
u16 k;

uchar CNCHAR[7] = "Celsius";
void LcdDisplay(int);
void UsartConfiguration();


void delayl(u16 k)
{
	while(k--);
}


void delayus(u16 k)
{
   	while(k--);
}
 
 
void delayms(u16 k)//
{
     for(q=0;q<k;q++)
     {
			delayus(100);
			}	
}

void delays(u16 k) //i=1?,???1s
{	 
     for(q=0;q<k;q++)
     {
				delayms(1000);
     }
}

void time0init()	  //定时器0初始化
{
	TMOD=0X01;	 //定时器0 方式1
	TH0=0Xff;
	TL0=0X06; //定时250us
//	TR0=1;
	EA=1;
	ET0=1;
}
void biaohaoinit()	   //各个标号初始化
{
	ding=0;		//叮声音  计数标志
	dong=0;		//咚声音  计数标志
	n=0;		//定时0.5s标志
	flag=0;
	stop=0;	   //结束标志
}
void main()
{
	u8 i;
	char temp_now[3];

	biaohaoinit();
	LcdInit();
	LcdWriteCom(0x88);	//Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
	LcdWriteData('C'); 
	while(1)
	{
		time0init();
		delayms(500);
		LcdInit();
		for(i=0;i<11;i++)
		{
			LcdWriteData(Disp_Hello[i]);	
		}
		if(k1==0)	   //判断按键是否按下
		{
			delayl(1000);  //消抖
			if(k1==0)
			{
				pressflag = pressflag + 1;
				LcdInit();
				TR0=1;	//打开定时器0
				for(i=0;i<10;i++)
				{
					LcdWriteData(Disp[i]);	
				}
				delays(8);
				LcdInit();
				while(!stop);
			}
		}
		if(k2==0)
		{
			delayl(1000);  //消抖
			if(k2==0)
			{
				sprintf(temp_now,"%d",pressflag);
				LcdInit();
				for(i=0;i<7;i++)
				{
					LcdWriteData(Disp_Test[i]);	
				}
				for(i=0;i<2;i++)
				{
					LcdWriteData(temp_now[i]);	
				}
				delays(5);
				LcdInit();
			}
		}
		if(k3==0)
		{
			UsartConfiguration();
			delayl(1000);  //消抖
			if(k3==0)
			{
				LcdInit();
				LcdDisplay(Ds18b20ReadTemp());
			}
		}
	}	
}

void time0() interrupt 1
{
	n++;
	TH0=0Xff;
	TL0=0X06; //250us
	if(n==2000)		//定时0.5s  叮响0.5秒，咚响0.5秒
	{
		n=0;
		if(flag==0)
		{
			flag=~flag;
		}
		else
		{
			flag=0;
			stop=1;
			TR0=0;	  //关闭定时器0
		}
	}
	if(flag==0)
	{					   //通过改变定时计数时间可以改变门铃的声音
		ding++;			  //叮
		if(ding==1)
		{
			ding=0;
			beep=~beep;
		}
	}
	else
	{
		dong++;
		if(dong==2)		  //咚
		{
			dong=0;
			beep=~beep;
		}	
	}
}

/*******************************************************************************
* º¯ÊýÃû         : LcdDisplay()
* º¯Êý¹¦ÄÜ		   : LCDÏÔÊ¾¶ÁÈ¡µ½µÄÎÂ¶È
* ÊäÈë           : v
* Êä³ö         	 : ÎÞ
*******************************************************************************/

void LcdDisplay(int temp) 	 //lcdÏÔÊ¾
{
    
  	unsigned char i, datas[] = {0, 0, 0, 0, 0}; //¶¨ÒåÊý×é
		float tp;  
		if(temp< 0)				//µ±ÎÂ¶ÈÖµÎª¸ºÊý
  	{
	  	LcdWriteCom(0x80);		//Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
			SBUF='-';//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
			while(!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
			TI=0;						 //Çå³ý·¢ËÍÍê³É±êÖ¾Î»
			LcdWriteData('-');  		//ÏÔÊ¾¸º
		//ÒòÎª¶ÁÈ¡µÄÎÂ¶ÈÊÇÊµ¼ÊÎÂ¶ÈµÄ²¹Âë£¬ËùÒÔ¼õ1£¬ÔÙÈ¡·´Çó³öÔ­Âë
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//ÁôÁ½¸öÐ¡Êýµã¾Í*100£¬+0.5ÊÇËÄÉáÎåÈë£¬ÒòÎªCÓïÑÔ¸¡µãÊý×ª»»ÎªÕûÐÍµÄÊ±ºò°ÑÐ¡Êýµã
		//ºóÃæµÄÊý×Ô¶¯È¥µô£¬²»¹ÜÊÇ·ñ´óÓÚ0.5£¬¶ø+0.5Ö®ºó´óÓÚ0.5µÄ¾ÍÊÇ½ø1ÁË£¬Ð¡ÓÚ0.5µÄ¾Í
		//ËãÓÉÏ0.5£¬»¹ÊÇÔÚÐ¡ÊýµãºóÃæ¡£
 
  	}
 	else
  	{			
	  	LcdWriteCom(0x80);		//Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
	    LcdWriteData('+'); 		//ÏÔÊ¾Õý
		SBUF='+';//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
		while(!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
		TI=0;						 //Çå³ý·¢ËÍÍê³É±êÖ¾Î»
		tp=temp;//ÒòÎªÊý¾Ý´¦ÀíÓÐÐ¡ÊýµãËùÒÔ½«ÎÂ¶È¸³¸øÒ»¸ö¸¡µãÐÍ±äÁ¿
		//Èç¹ûÎÂ¶ÈÊÇÕýµÄÄÇÃ´£¬ÄÇÃ´ÕýÊýµÄÔ­Âë¾ÍÊÇ²¹ÂëËü±¾Éí
		temp=tp*0.0625*100+0.5;	
		//ÁôÁ½¸öÐ¡Êýµã¾Í*100£¬+0.5ÊÇËÄÉáÎåÈë£¬ÒòÎªCÓïÑÔ¸¡µãÊý×ª»»ÎªÕûÐÍµÄÊ±ºò°ÑÐ¡Êýµã
		//ºóÃæµÄÊý×Ô¶¯È¥µô£¬²»¹ÜÊÇ·ñ´óÓÚ0.5£¬¶ø+0.5Ö®ºó´óÓÚ0.5µÄ¾ÍÊÇ½ø1ÁË£¬Ð¡ÓÚ0.5µÄ¾Í
		//Ëã¼ÓÉÏ0.5£¬»¹ÊÇÔÚÐ¡ÊýµãºóÃæ¡£
	}
	datas[0] = temp / 10000;
	datas[1] = temp % 10000 / 1000;
	datas[2] = temp % 1000 / 100;
	datas[3] = temp % 100 / 10;
	datas[4] = temp % 10;

	LcdWriteCom(0x82);		  //Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
	LcdWriteData('0'+datas[0]); //°ÙÎ» 
	SBUF = '0'+datas[0];//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
	while (!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
	TI = 0;
	
	LcdWriteCom(0x83);		 //Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
	LcdWriteData('0'+datas[1]); //Ê®Î»
	SBUF = '0'+datas[1];//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
	while (!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
	TI = 0;

	LcdWriteCom(0x84);		//Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
	LcdWriteData('0'+datas[2]); //¸öÎ» 
	SBUF = '0'+datas[2];//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
	while (!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
	TI = 0;

	LcdWriteCom(0x85);		//Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
	LcdWriteData('.'); 		//ÏÔÊ¾ ¡®.¡¯
	SBUF = '.';//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
	while (!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
	TI = 0;

	LcdWriteCom(0x86);		 //Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
	LcdWriteData('0'+datas[3]); //ÏÔÊ¾Ð¡Êýµã  
	SBUF = '0'+datas[3];//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
	while (!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
	TI = 0;

	LcdWriteCom(0x87);		 //Ð´µØÖ· 80±íÊ¾³õÊ¼µØÖ·
	LcdWriteData('0'+datas[4]); //ÏÔÊ¾Ð¡Êýµã 
	SBUF = '0'+datas[4];//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
	while (!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
	TI = 0;
	for(i=0; i<6; i++)
	{
	 	SBUF = CNCHAR[i];//½«½ÓÊÕµ½µÄÊý¾Ý·ÅÈëµ½·¢ËÍ¼Ä´æÆ÷
		while (!TI);			         //µÈ´ý·¢ËÍÊý¾ÝÍê³É
		TI = 0;
	}

	 
}
/*******************************************************************************
* º¯ Êý Ãû         :UsartConfiguration()
* º¯Êý¹¦ÄÜ		   :ÉèÖÃ´®¿Ú
* Êä    Èë         : ÎÞ
* Êä    ³ö         : ÎÞ
*******************************************************************************/

void UsartConfiguration()
{
	SCON=0X50;			//ÉèÖÃÎª¹¤×÷·½Ê½1
	TMOD=0X20;			//ÉèÖÃ¼ÆÊýÆ÷¹¤×÷·½Ê½2
	PCON=0X80;			//²¨ÌØÂÊ¼Ó±¶
	TH1=0XF3;				//¼ÆÊýÆ÷³õÊ¼ÖµÉèÖÃ£¬×¢Òâ²¨ÌØÂÊÊÇ4800µÄ
	TL1=0XF3;
//	ES=1;						//´ò¿ª½ÓÊÕÖÐ¶Ï
//	EA=1;						//´ò¿ª×ÜÖÐ¶Ï
	TR1=1;					//´ò¿ª¼ÆÊýÆ÷
}
