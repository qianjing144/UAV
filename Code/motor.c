/*
 * motor.c
 *
 *  Created on: Jul 13, 2024
 *      Author: qian-jing
 */
#include <STC8051U.H>
#include <intrins.h>
#include "main.h"
#include "motor.h"
#include "timer.h"
#ifdef DEBUG
#include "oled.h"
#endif

#define PWM1_0 0x00
#define PWM1_1 0x01
#define PWM1_2 0x02

#define PWM2_0 0x00
#define PWM2_1 0x04
#define PWM2_2 0x08

#define PWM3_0 0x00
#define PWM3_1 0x10
#define PWM3_2 0x20

#define PWM4_0 0x00
#define PWM4_1 0x40
#define PWM4_2 0x80
#define PWM4_3 0xC0

#define ENO1P  0x01
#define ENO1N  0x02
#define ENO2P  0x04
#define ENO2N  0x08
#define ENO3P  0x10
#define ENO3N  0x20
#define ENO4P  0x40
#define ENO4N  0x80

void Delay300ms(void)	//@24.000MHz
{
	unsigned long edata i;

	_nop_();
	_nop_();
	i = 1799998UL;
	while (i) i--;
}


void motor_init()
{
	
	PWMA_PS=0x55;//01010101
	PWMA_CCER1=0x00;
	PWMA_CCER2=0x00;
//	
	PWMA_CCMR1=0x60;
	PWMA_CCMR2=0x60;
	PWMA_CCMR3=0x60;
	PWMA_CCMR4=0x60;
//	
	PWMA_CCER1=0x11;//高电平有效
	PWMA_CCER2=0x11;//高电平有效
	
	//clkdiv 480  freq 50hz
	PWMA_PSCRH=(480-1)>>8;
	PWMA_PSCRL=(480-1);
	PWMA_ARRH=1000>>8;
	PWMA_ARRL=1000;
	
	PWMA_ENO=0x55;//0101 0101
	PWMA_BKR=0x80;
	PWMA_CR1=0x01;
	{
		unsigned char i=0;
		for(i=30;i<45;i++)
		{
			motor_control(i,i,i,i);
			Delay300ms();
		}
	}
}

void motor_control(int val1,int val2,int val3,int val4)
{
	if(val1>1000)//防止值超过1000
	{
		val1=1000;//将值限定在1000
	}
	else if(val1<0)
	{
		val1=0;
	}
	
	if(val2>1000)//防止值超过1000
	{
		val2=1000;//将值限定在1000
	}
	else if(val2<0)
	{
		val2=0;
	}
	
	if(val3>1000)//防止值超过1000
	{
		val3=1000;//将值限定在1000
	}
	else if(val3<0)
	{
		val3=0;
	}
	
	if(val4>1000)//防止值超过1000
	{
		val4=1000;//将值限定在1000
	}
	else if(val4<0)
	{
		val4=0;
	}
	PWMA_CCR1H=val1>>8;
	PWMA_CCR1L=val1;
	PWMA_CCR2H=val2>>8;
	PWMA_CCR2L=val2;
	PWMA_CCR3H=val3>>8;
	PWMA_CCR3L=val3;
	PWMA_CCR4H=val4>>8;
	PWMA_CCR4L=val4;
}
void motor_1(int val)
{
	if(val>1000)//防止值超过1000
	{
		val=1000;//将值限定在1000
	}
	if(val<0)
	{
		val=0;
	}
	PWMA_CCR1H=val>>8;
	PWMA_CCR1L=val;
//	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,val);//设置占空比
}
void motor_2(int val)
{
	if(val>1000)//防止值超过1000
	{
		val=1000;//将值限定在1000
	}
	if(val<0)
	{
		val=0;
	}
	PWMA_CCR2H=val>>8;
	PWMA_CCR2L=val;
//	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,val);//设置占空比
}
void motor_3(int val)
{
	if(val>1000)//防止值超过1000
	{
		val=1000;//将值限定在1000
	}
	if(val<0)
	{
		val=0;
	}
	PWMA_CCR3H=val>>8;
	PWMA_CCR3L=val;
//	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,val);//设置占空比
}
void motor_4(int val)
{
	if(val>1000)//防止值超过1000
	{
		val=1000;//将值限定在1000
	}
	if(val<0)
	{
		val=0;
	}
	PWMA_CCR4H=val>>8;
	PWMA_CCR4L=val;
//	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,val);//设置占空比
}
