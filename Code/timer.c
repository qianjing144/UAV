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

//void Timer4_Isr(void) interrupt 20
//{
//}

void Timer4_Init(void)		//1毫秒@24.000MHz
{
	TM4PS = 0x24;			//设置定时器时钟预分频 ( 注意:并非所有系列都有此寄存器,详情请查看数据手册 )
	T4T3M |= 0x20;			//定时器时钟1T模式
	T4L = 0x9F;				//设置定时初始值
	T4H = 0x02;				//设置定时初始值
	T4T3M |= 0x80;			//定时器4开始计时
	IE2 |= 0x40;			//使能定时器4中断
}
