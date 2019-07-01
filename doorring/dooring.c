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
u8 Disp_Hello[14] = "Welcome!!!!!";
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
	u8 pressk3_flag = 0;
	char temp_now[3];
	
	biaohaoinit();
	
	LcdInit();
	LcdWriteCom(0x88);	
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
		LcdDisplay(Ds18b20ReadTemp());
		delays(1); //延时1秒
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
			pressk3_flag = pressk3_flag + 1;
			UsartConfiguration();
			delayl(1000);  //消抖
			if(k3==0)
			{
				LcdInit();
				//LcdDisplay(Ds18b20ReadTemp());
				delays(1);
			}
		}
	}	
}

void time0() interrupt 1 //定时器0中断1
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
* 函数名         : LcdDisplay()
* 函数功能		   : LCD显示读取到的温度
* 输入           : v
* 输出         	 : 无
*******************************************************************************/

void LcdDisplay(int temp) 	 //lcd显示
{
  	unsigned char i, datas[] = {0, 0, 0, 0, 0, 0, 0}; //定义数组
	float tp;  
	if(temp< 0)				//当温度值为负数
  	{
	  	LcdWriteCom(0x80+0x40);		//写地址 80表示初始地址 +
		SBUF='-';//将接收到的数据放入到发送寄存器
		while(!TI);			         //等待发送数据完成
		TI=0;						 //清除发送完成标志位
	    LcdWriteData('-');  		//显示负
		//因为读取的温度是实际温度的补码，所以减1，再取反求出原码
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算由�0.5，还是在小数点后面。
 
  	}
 	else
  	{			
	  	LcdWriteCom(0x80+0x40);		//写地址 80表示初始地址
	    LcdWriteData('+'); 		//显示正
		SBUF='+';//将接收到的数据放入到发送寄存器
		while(!TI);			         //等待发送数据完成
		TI=0;						 //清除发送完成标志位
		tp=temp;//因为数据处理有小数点所以将温度赋给一个浮点型变量
		//如果温度是正的那么，那么正数的原码就是补码它本身
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算加上0.5，还是在小数点后面。
	}
	datas[0] = temp / 10000;
	datas[1] = temp % 10000 / 1000;
	datas[2] = temp % 1000 / 100;
	datas[3] = temp % 100 / 10;
	datas[4] = temp % 10;
	datas[5] = 223;
	datas[6] = 'C';

	LcdWriteCom(0x82+0x40);		  //写地址 80表示初始地址
	LcdWriteData('0'+datas[0]); //百位 
	SBUF = '0'+datas[0];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	
	LcdWriteCom(0x83+0x40);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[1]); //十位
	SBUF = '0'+datas[1];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x84+0x40);		//写地址 80表示初始地址
	LcdWriteData('0'+datas[2]); //个位 
	SBUF = '0'+datas[2];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x85+0x40);		//写地址 80表示初始地址
	LcdWriteData('.'); 		//显示 ‘.’
	SBUF = '.';//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x86+0x40);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[3]); //显示小数点  
	SBUF = '0'+datas[3];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x87+0x40);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[4]); //显示小数点 
	SBUF = '0'+datas[4];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	
	LcdWriteCom(0x88+0x40);		 //写地址 80表示初始地址
	LcdWriteData(datas[5]); //显示小数点 
	SBUF = '0'+datas[5];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	
	LcdWriteCom(0x89+0x40);		 //写地址 80表示初始地址
	LcdWriteData(datas[6]); //显示小数点 
	SBUF = '0'+datas[6];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	
	for(i=0; i<8; i++)
	{
	 	SBUF = CNCHAR[i];//将接收到的数据放入到发送寄存器
		while (!TI);			         //等待发送数据完成
		TI = 0;
	}
}
/*******************************************************************************
* 函 数 名         :UsartConfiguration()
* 函数功能		   :设置串口
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void UsartConfiguration() 
{
	SCON=0X50;			//设置为工作方式1
	TMOD=0X20;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XF3;				//计数器初始值设置，注意波特率是4800的
	TL1=0XF3;
//	ES=1;						//打开接收中断
//	EA=1;						//打开总中断
	TR1=1;					//打开计数器
}