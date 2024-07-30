/*
 * SBUS.c
 *
 *  Created on: Jul 15, 2024
 *      Author: qian-jing
 */
#include <STC8051U.H>
#include <intrins.h>
#include "main.h"
#include "SBUS.h"

#ifdef DEBUG
#include "oled.h"
#endif

#include "UART.h"

#define START_BYTE 0x0F
#define END_BYTE   0x00

static int ch[16];
unsigned char rx_cnt=0;
unsigned char rx2_timeOut=0;
static unsigned char xdata sbusBuf[25];

void sbus_init()
{
	
	S2CON=0xD8;//1101 1100模式三
	S2CFG|=0x01;//W1=1
	AUXR |= 0x04;		//定时器时钟1T模式
	T2H = (65536-60)/256;
	T2L = (65536-60)%256;
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0x01;		//使能串口2中断
	INTCLKO &= ~0x02;//不输出时钟
	
	DMA_UR2R_CFG=0x80;
	DMA_UR2R_STA=0x00;
	DMA_UR2R_AMT=25-1; //传输总字节数减一
	DMA_UR2R_RXAH=(unsigned char)(((unsigned int)sbusBuf)>>8);
	DMA_UR2R_RXAL=(unsigned char)((unsigned int)sbusBuf);
	
	sbusBuf[0]=0x00;
	sbusBuf[24]=0xFF;
	DMA_UR2R_CR=0xa1;
}

void sbus_inirq()
{
	if(sbusBuf[0]==START_BYTE && sbusBuf[24]==END_BYTE)
	{
		ch[ 0] = ((int)sbusBuf[ 2-1] >> 0 | ((int)sbusBuf[ 3-1] << 8 )) & 0x07FF;
		ch[ 1] = ((int)sbusBuf[ 3-1] >> 3 | ((int)sbusBuf[ 4-1] << 5 )) & 0x07FF;
		ch[ 2] = ((int)sbusBuf[ 4-1] >> 6 | ((int)sbusBuf[ 5-1] << 2 )  | (int)sbusBuf[ 6-1] << 10 ) & 0x07FF;
		ch[ 3] = ((int)sbusBuf[ 6-1] >> 1 | ((int)sbusBuf[ 7-1] << 7 )) & 0x07FF;
		ch[ 4] = ((int)sbusBuf[ 7-1] >> 4 | ((int)sbusBuf[ 8-1] << 4 )) & 0x07FF;
		ch[ 5] = ((int)sbusBuf[ 8-1] >> 7 | ((int)sbusBuf[ 9-1] << 1 )  | (int)sbusBuf[10-1] <<  9 ) & 0x07FF;
		ch[ 6] = ((int)sbusBuf[10-1] >> 2 | ((int)sbusBuf[11-1] << 6 )) & 0x07FF;
		ch[ 7] = ((int)sbusBuf[11-1] >> 5 | ((int)sbusBuf[12-1] << 3 )) & 0x07FF;

		ch[ 8] = ((int)sbusBuf[13-1] << 0 | ((int)sbusBuf[14-1] << 8 )) & 0x07FF;
		ch[ 9] = ((int)sbusBuf[14-1] >> 3 | ((int)sbusBuf[15-1] << 5 )) & 0x07FF;
		ch[10] = ((int)sbusBuf[15-1] >> 6 | ((int)sbusBuf[16-1] << 2 )  | (int)sbusBuf[17-1] << 10 ) & 0x07FF;
		ch[11] = ((int)sbusBuf[17-1] >> 1 | ((int)sbusBuf[18-1] << 7 )) & 0x07FF;
		ch[12] = ((int)sbusBuf[18-1] >> 4 | ((int)sbusBuf[19-1] << 4 )) & 0x07FF;
		ch[13] = ((int)sbusBuf[19-1] >> 7 | ((int)sbusBuf[20-1] << 1 )  | (int)sbusBuf[21-1] <<  9 ) & 0x07FF;
		ch[14] = ((int)sbusBuf[21-1] >> 2 | ((int)sbusBuf[22-1] << 6 )) & 0x07FF;
		ch[15] = ((int)sbusBuf[22-1] >> 5 | ((int)sbusBuf[23-1] << 3 )) & 0x07FF;
		sbusBuf[0]=0x00;
		sbusBuf[24]=0xFF;
	}
}

int sbus_getData(int ch_index)
{
	return ch[ch_index];
}

unsigned char sbus_getRxCnt()
{
	return rx_cnt;
}

void sbus_setRxCnt(unsigned char val)
{
	rx_cnt=val;
}

unsigned char sbus_getRx2TimeOut()
{
	return rx2_timeOut;
}

void sbus_setRx2TimeOut(unsigned char val)
{
	rx2_timeOut=val;
}