#include <STC8051U.H>
#include <stdio.h>

void uart1_init() //921600
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器时钟1T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xF9;			//设置定时初始值
	TH1 = 0xFF;			//设置定时初始值
	ET1 = 0;			//禁止定时器中断
	TR1 = 1;			//定时器1开始计时
}

void uart1_sendByte(unsigned char dat){
   SBUF = dat;     //写入发送缓冲寄存器
   while(!TI);    //等待发送完成，TI发送溢出标志位 置1
   TI = 0;      //对溢出标志位清零	
}
void uart1_sendString(unsigned char *str){
	while(*str!='\0'){    //指针的数据没到最后一位一直执行循环体
		uart1_sendByte(*str++); //发送指针指向的数据(字节byte)
							 //指针自增，指向下一个数据 
	}
}

//void Uart2_Isr(void) interrupt 8
//{
//	if (S2CON & 0x02)	//检测串口2发送中断
//	{
//		S2CON &= ~0x02;	//清除串口2发送中断请求位
//	}
//	if (S2CON & 0x01)	//检测串口2接收中断
//	{
//		S2CON &= ~0x01;	//清除串口2接收中断请求位
//	}
//}
