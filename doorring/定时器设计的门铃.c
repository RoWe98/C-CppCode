/* ���س���󰴼�K1���¾ͻᴥ�����������塰���ˡ���  */

#include "reg52.h"
#include "LCD.h"

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

u8 Disp[] = "New Guests";
u8 Disp_Hello[] = "Welcome";
sbit beep=P1^5;
sbit k1=P3^1;
sbit k2=P3^2;
u8 ding,dong,flag,stop;
u16 n;


u16 j=0;
u32 q=0;
u16 k;


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

void time0init()	  //��ʱ��0��ʼ��
{
	TMOD=0X01;	 //��ʱ��0 ��ʽ1
	TH0=0Xff;
	TL0=0X06; //��ʱ250us
//	TR0=1;
	EA=1;
	ET0=1;
}
void biaohaoinit()	   //������ų�ʼ��
{
	ding=0;		//������  ������־
	dong=0;		//������  ������־
	n=0;		//��ʱ0.5s��־
	flag=0;
	stop=0;	   //������־
}
void main()
{
	u8 i;
	time0init();
	biaohaoinit();
	LcdInit();
	while(1)
	{
		LcdInit();
		for(i=0;i<16;i++)
		{
			LcdWriteData(Disp_Hello[i]);	
		}
		if(k1==0)	   //�жϰ����Ƿ���
		{
			delayl(1000);  //����
			if(k1==0)
			{
				LcdInit();
				TR0=1;	//�򿪶�ʱ��0
				for(i=0;i<16;i++)
				{
					LcdWriteData(Disp[i]);	
				}
				delays(8);
				LcdInit();
				while(!stop);
			}
		}
	}	
}

void time0() interrupt 1
{
	n++;
	TH0=0Xff;
	TL0=0X06; //250us
	if(n==2000)		//��ʱ0.5s  ����0.5�룬����0.5��
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
			TR0=0;	  //�رն�ʱ��0
		}
	}
	if(flag==0)
	{					   //ͨ���ı䶨ʱ����ʱ����Ըı����������
		ding++;			  //��
		if(ding==1)
		{
			ding=0;
			beep=~beep;
		}
	}
	else
	{
		dong++;
		if(dong==2)		  //��
		{
			dong=0;
			beep=~beep;
		}	
	}
}