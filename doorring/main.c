/*******************************************************************************
* 实验名			   :温度显示实验
* 使用的IO	     : 
* 实验效果       :1602显示温度
*	注意					 ：
*******************************************************************************/

#include<reg51.h>
#include"lcd.h"
#include"temp.h"

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;


uchar CNCHAR[6] = "摄氏度";
char LcdDisplay(int);
void UsartConfiguration();
char Disp[14] = "Welcome~!!!";
sbit k1=P3^1;


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



/*******************************************************************************
* 函数名         : main
* 函数功能		   : 主函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/

void main()
{
	int i;
	char flag;
	UsartConfiguration();
	LcdInit();			 //初始化LCD1602
	LcdWriteCom(0x88);	//写地址 80表示初始地址
	LcdWriteData('C'); 
	while(1)
	{
		delayms(500);
		LcdInit();
		for(i=0;i<11;i++)
		{
			LcdWriteData(Disp[i]);
		}
		if(k1==0)
		{
			UsartConfiguration();
			delayl(1000);  //??
			if(k1==0)
			{
				LcdInit();
				flag = LcdDisplay(Ds18b20ReadTemp());
			}
			LcdWriteData(flag);
		}
	}
}

/*******************************************************************************
* 函数名         : LcdDisplay()
* 函数功能		   : LCD显示读取到的温度
* 输入           : v
* 输出         	 : 无
*******************************************************************************/

char LcdDisplay(int temp) 	 //lcd显示
{
	char flag1;
  	unsigned char i, datas[] = {0, 0, 0, 0, 0}; //定义数组
	float tp;  
	if(temp< 0)				//当温度值为负数
  	{
	  	LcdWriteCom(0x80);		//写地址 80表示初始地址
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
	  	LcdWriteCom(0x80);		//写地址 80表示初始地址
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

	LcdWriteCom(0x82);		  //写地址 80表示初始地址
	LcdWriteData('0'+datas[0]); //百位 
	SBUF = '0'+datas[0];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	
	LcdWriteCom(0x83);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[1]); //十位
	flag1 =datas[1];
	SBUF = '0'+datas[1];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x84);		//写地址 80表示初始地址
	LcdWriteData('0'+datas[2]); //个位 
	SBUF = '0'+datas[2];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x85);		//写地址 80表示初始地址
	LcdWriteData('.'); 		//显示 ‘.’
	SBUF = '.';//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x86);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[3]); //显示小数点  
	SBUF = '0'+datas[3];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;

	LcdWriteCom(0x87);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[4]); //显示小数点 
	SBUF = '0'+datas[4];//将接收到的数据放入到发送寄存器
	while (!TI);			         //等待发送数据完成
	TI = 0;
	for(i=0; i<6; i++)
	{
	 	SBUF = CNCHAR[i];//将接收到的数据放入到发送寄存器
		while (!TI);			         //等待发送数据完成
		TI = 0;
	}
	return flag1;
	 
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
