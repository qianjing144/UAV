#include <intrins.h>
#include <math.h>
#include "LPS22HH.h"
#include "i2c.h"
#include "UART.h"
#include "main.h"
#include "oled.h"

#define INTERRUPT_CFG 0x0B
#define THS_P_L 0x0C
#define THS_P_H 0x0D
#define IF_CTRL 0x0E
#define WHO_AM_I 0x0F
#define CTRL_REGn 0x10//3Byte
#define RESET 0x11//CTRL_REG2
#define FIFO_WTM 0x14
#define REF_P_L 0x15
#define REF_P_H 0x16
#define RPDS_L 0x18
#define RPDS_H 0x19
#define INT_SOURCE 0x24
#define FIFO_STATUSn 0x25//2Byte
#define STATUS 0x27
#define PRESS_OUTn 0X28//3Byte XL L H
#define TEMP_OUTn 0x2B//2Byte L H
#define FIFO_DATA_OUT_PRESSn 0x78//3Byte Xl L H
#define FIFO_DATA_OUT_TEMPn 0x7B//2Byte L H

#define SET_INTERRUPT_CFG 0x00//0000 0000
#define SET_IF_CTRL 0x02//0000 0010
#define SET_CTRL_REG1 0x7E//0111 1110
#define SET_CTRL_REG2 0x90//1001 0000
#define RESET_OP 0x04//0000 0100 复位
#define SET_CRTL_REG3 0x04//0000 0000



//float prs=0.0f;//hPa
//float tmp=0.0f;//℃
//float alt=0.0f;//m

static void delayxms(unsigned long xms)	//@11.0592MHz
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

static void write_reg(unsigned char devAddr,unsigned char reg,unsigned char *pData, unsigned char amount)
{
//	if(i2c_mem_write(devAddr,reg,pData,amount)!=I2C_OK) {
//		return 1;
//	}
//	return 0;
	
	
	i2c_mem_write(devAddr,reg,pData,amount);
}
static void read_reg(unsigned char devAddr,unsigned char reg,unsigned char *pData, unsigned char amount)
{
//	if(i2c_mem_read(devAddr, reg, pData, amount)!=I2C_OK) {
//		return 1;
//	}
//	return 0;
	i2c_mem_read(devAddr,reg,pData,amount);
}

void lps22hh_reset(LPS22HH_t* obj)
{
	unsigned char cmd=0x00; 
	cmd=RESET_OP;
	write_reg(obj->devAddr,RESET,&cmd,1);
}


unsigned char lps22hh_init(LPS22HH_t* obj)
{
	unsigned char cmd[3]={0};
	read_reg(obj->devAddr, WHO_AM_I, cmd, 1);
	
	if(cmd[0]!=0xB3)
	{
		return 1;
	}
	
	lps22hh_reset(obj);
	cmd[0]=SET_IF_CTRL;
	write_reg(obj->devAddr,IF_CTRL, cmd, 1);//设置IF_CTRL
	
	cmd[0]=SET_CTRL_REG1;
	cmd[1]=SET_CTRL_REG2;
	cmd[2]=SET_CRTL_REG3;
	write_reg(obj->devAddr,CTRL_REGn,cmd,3);//设置CTRL_REGn
	for(;;) {
		read_reg(obj->devAddr,INT_SOURCE, cmd, 1);
		if((cmd[0]&0x80)==0x00) {//检查是否启动完成 0x00启动完成 0x80正在启动
			break;
		}
	}
	return 0;
}

unsigned char lps22hh_get_tmp(LPS22HH_t* obj,float *tmp)
{
	unsigned char cmd=0x00; 
	unsigned char buf[2]={0};
	unsigned int raw=0;
	read_reg(obj->devAddr,STATUS, &cmd, 1);
	if((cmd&0x22)!=0x22)  //0010 0010
	{
		return 1;
	}
	read_reg(obj->devAddr,TEMP_OUTn, buf, 2);
	*tmp=((int)buf[1]<<8 | buf[0])/100.0;
	return 0;
}

unsigned char lps22hh_get_prs(LPS22HH_t* obj,float *prs)
{
	unsigned char buf[3]={0};
	unsigned char cmd=0x00; 
	read_reg(obj->devAddr,STATUS, &cmd, 1);
	if((cmd&0x11)!=0x11) { //0001 0001
		return 1;
	}
	read_reg(obj->devAddr,PRESS_OUTn, buf, 3);
#ifdef DEBUG
//	delayxms(50);
//	OLED_ShowNum(0,0,buf[0],3,12,0);//3E
//	OLED_ShowNum(0,1,buf[1],3,12,0);//17
//	OLED_ShowNum(0,2,buf[2],3,12,0);//7D
#endif
	*prs=((long)buf[2]<<16|(int)buf[1]<<8|buf[0])/4096.0;
	return 0;
}

unsigned char lps22hh_get_alt(LPS22HH_t* obj,float *alt)
{
	float prs=0;
	float tmp=0;
	unsigned char cmd=0x00; 
	unsigned char buf[5]={0};
	
	read_reg(obj->devAddr,STATUS,&cmd,1);
	
	if((cmd&0x33)!=0x33)
	{
		return 1;
	}
	
//	delayxms(10);
	read_reg(obj->devAddr,PRESS_OUTn, buf, 5);
	
	prs=((long)buf[2]<<16|(int)buf[1]<<8|buf[0])/4096.0;
	tmp=((int)buf[1]<<8|buf[0])/100.0;
	*alt=(((pow(1013.25/prs,1/5.257))-1)*(tmp+273.15))/0.0065;
	return 0;
}
