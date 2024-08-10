#include <STC8051U.H>
#include "I2C.h"
#include "intrins.h"
#include "uart.h"
#define NO_ACTION 			0000
#define START     			0001
#define WRITE	  			0010
#define RACK     			0011
#define READ   				0100
#define WACK     			0101
#define STOP      			0110
#define START_WRITE_RACK  	1001
#define WRITE_RACK         	1010
#define READ_WACK			1011
#define READ_WNAK         	1100

sbit SDA = P2^3;
sbit SCL = P2^4;

static void delayxms(unsigned int xms)	//@11.0592MHz
{
	unsigned long edata i;
	unsigned char j=0;
	while(xms--)
	{
		for(j=0;j<13;j++)
		{
			_nop_();
			_nop_();
			i = 2763UL;
			while (i) i--;
		}
	}
}

void i2c_init()
{	
	P_SW2 &= 0xCF;//11001111
//	I2CCFG=0xFD;//11 001101 低六位
//	I2CPSCR=0x00;//高八位  0 13
//	I2CMSCR=0x00;//0 000 0000 
//	I2CMSAUX=0x00;//0000 000 0
//	I2CMSST=0x00;//00 0000 0 1
	
	I2CCFG = 0xCD;//1100 1101
	I2CMSST= 0x00;
}

void Wait()
{
	
	while(!(I2CMSST&0x40));
	I2CMSST &= ~0x40;
}

void Start()
{
	I2CMSCR=0x01; //发送START命令
	Wait();
}

void SendData(unsigned dat)
{
	I2CTXD=dat;	 //写数据到数据缓冲区
	I2CMSCR=0x02;//发送SEND命令
	Wait();
}

unsigned char RecvACK()
{
	I2CMSCR=0x03; //发送读ACK命令
	
	Wait();
	return I2CMSST=1?0:1;
}

unsigned char RecvData()
{
	I2CMSCR=0x04; //发送RECV命令
	Wait();
	return I2CRXD;
}

void SendACK()
{
	I2CMSST=0x00;//设置ACK信号
	I2CMSCR=0x05;//发送ACK命令
	Wait();
}

void SendNAK()
{
	I2CMSST = 0x01;//设置NAK信号
	I2CMSCR = 0x05;//发送ACK命令
	Wait();
}

void Stop()
{
	I2CMSCR=0x06; //发送STOP命令
	Wait();
}

void Delay()
{
	int i;
	
	for(i=0;i<3000;i++)
	{
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}
}

unsigned char i2c_mem_write(unsigned char devAddr,unsigned char reg,unsigned char *pData,unsigned char amount)
{
	unsigned char i;
	if((I2CMSST&0x80)==0x80)//总线忙碌等待释放
	{
		return 1;
	}
//	while((I2CMSST&0x80)==0x80);
	Start();//发送ST
	SendData(devAddr<<1);//发送设备地址加写命令
	if(RecvACK())//接收应答
	{
		Stop();//通信结束
		return 1;
	}
	SendData(reg);//发送存储地址
	if(RecvACK())//接收应答
	{
		Stop();//通信结束
		return 1;
	}
	for(i=0;i<amount;i++)
	{
		SendData(*(pData+i)); //发送数据
		if(RecvACK())
		{
			Stop();//通信结束
			return 1;
		}
	}
	Stop();//通信结束
	return 0;
}

unsigned char i2c_mem_read(unsigned char devAddr,unsigned char reg,unsigned char *pData,unsigned char amount)
{
	unsigned char i;
	if((I2CMSST&0x80)==0x80)//总线忙碌等待释放
	{
		return 1;
	}
//	while((I2CMSST&0x80)==0x80);
	Start();//发送ST
	SendData(devAddr<<1);//发送设备地址加写命令
	if(RecvACK())//接收应答
	{
		Stop();//通信结束
		return 1;
	}
	SendData(reg);//发送存储地址
	if(RecvACK())//接收应答
	{
		Stop();
		return 1;
	}
	Start();//发送SR
	SendData((devAddr<<1)+1);//发送设备地址加读命令
	if(RecvACK())//接收应答
	{
		Stop();//通信结束
		return 1;
	}
	for(i=0;i<amount;i++)
	{
		*(pData+i)=RecvData(); //接收数据
		if(i!=amount-1)
		{
			SendACK();//发送应答
		}
	}
	SendNAK();//发送未应答
	Stop();//通信结束
	return 0;
}
