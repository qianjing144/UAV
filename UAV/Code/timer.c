#include <STC8051U.H>

//void Timer3_Isr(void) interrupt 19
//{
//}

void timer3_init(void)		//1毫秒@24.000MHz
{
	T4T3M |= 0x02;			//定时器时钟1T模式
	T3L = (65535-24000);				//设置定时初始值
	T3H = (65535-24000)>>8;				//设置定时初始值
	T4T3M |= 0x08;			//定时器3开始计时
	IE2 |= 0x20;			//使能定时器3中断
}
