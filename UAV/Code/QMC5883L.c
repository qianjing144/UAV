/*
 * QMC5883L.c
 *
 *  Created on: Jul 18, 2024
 *      Author: qian-jing
 */

#include "QMC5883L.h"
#ifdef DEBUG
#include "oled.h"
#endif
#include <math.h>
#include <stdlib.h>
//L->H 2Byte
#define DATA_OUTPUT_X 0x00
#define DATA_OUTPUT_Y 0x02
#define DATA_OUTPUT_Z 0x04
#define STATUS 0x06
#define DATA_OUTPUT_T 0x07
#define CONTROL1 0x09 //2BYTE
#define	CONTROL2 0x0A
#define SR_PERIOD 0x0B //SR:SET_OR_RESET
#define ID 0x0D

#define CalThreshold 0
#define MAG_DEC 0//磁偏角

#define M_PI 3.14159265358979323846

float offsetX=-0.14699;
float offsetY=0.057160;

static unsigned char write_reg(QMC5883L_t *obj,unsigned char reg,unsigned char *pData,unsigned char amount)
{
	i2c_mem_write(obj->devAddr,reg,pData,amount);
	return 0;
}

static unsigned char read_reg(QMC5883L_t *obj,unsigned char reg,unsigned char *pData,unsigned char amount)
{
	i2c_mem_read(obj->devAddr,reg,pData,amount);
	return 0;
}

unsigned char qmc5883l_init(QMC5883L_t *obj)
{
	unsigned char pData[1];
	unsigned char status=0;
	status=read_reg(obj, ID, pData, 1);
	if(*pData!=0xFF)
	{
		return 1;
	}
	pData[0]=0x80;
	status=write_reg(obj, CONTROL2, pData, 1);//RESET
	pData[0]=0x1D;//default 0001 1101
	status=write_reg(obj, CONTROL1, pData, 1);
	pData[0]=0x00;//0000 0000
	status=write_reg(obj, CONTROL2, pData, 1);
	pData[0]=0x01;//recommended
	status=write_reg(obj, SR_PERIOD, pData, 1);
	return status;
}

unsigned char qmc5883l_calibrate(QMC5883L_t *obj)
{
	unsigned char status=0;
	unsigned char ready_status=0;
	unsigned char raw[6]={0};
	float x=0,y=0;
	float xMin,xMax,yMin,yMax;
	offsetX=offsetY=0;
	xMin=xMax=yMin=yMax=0;
	while(1)
	{
		status=read_reg(obj, STATUS, &ready_status, 1);
		if((ready_status&0x01)!=0x01)
		{
			continue;
		}
		status=read_reg(obj, DATA_OUTPUT_X, raw, 6);
		x=((int)(raw[1]<<8)|raw[0])/3000.0;
		y=((int)(raw[3]<<8)|raw[2])/3000.0;
		if (x > xMax)
			xMax = x;
		if (x < xMin )
			xMin = x;
		if(y > yMax )
			yMax = y;
		if(y < yMin )
			yMin = y;
//		if(z > zMax )
//			zMax = z;
//		if(z < zMin )
//			zMin = z;
		if(fabs(xMax - xMin) > CalThreshold )
			offsetX = (xMax + xMin)/2;
		if(fabs(yMax - yMin) > CalThreshold )
			offsetY = (yMax + yMin)/2;
		#ifdef DEBUG
		OLED_Showdecimal(0, 0, offsetX, 1, 5, 12, 0);
		OLED_Showdecimal(64, 0, offsetY, 1, 5, 12, 0);
		OLED_Showdecimal(0, 1, xMax, 1, 5, 12, 0);
		OLED_Showdecimal(0, 2, yMax, 1, 5, 12, 0);
		OLED_Showdecimal(0, 3, xMin, 1, 5, 12, 0);
		OLED_Showdecimal(0, 4, yMin, 1, 5, 12, 0);
		#endif
	}

//	if(abs(zMax - zMin) > CalThreshold )
//		offsetZ = (zMax +zMin)/2;
	return status;
}

unsigned char qmc5883l_getHeading(QMC5883L_t *obj,float *heading)
{
	unsigned char raw[6]={0};
//	unsigned char x=0,y=0,z=0;
	float x=0,y=0;
	unsigned char status=0;
	unsigned char ready_status=0;
	status=read_reg(obj, STATUS, &ready_status, 1);
	if((ready_status&0x01)!=0x01)
	{
		return 1;
	}
	status=read_reg(obj, DATA_OUTPUT_X, raw, 6);
	x=((int)(raw[1]<<8)|raw[0])/3000.0;
	y=((int)(raw[3]<<8)|raw[2])/3000.0;
//	z=(int)(raw[5]<<8)|raw[4];

	*heading=atan2(y-offsetY,x-offsetX);
	if(*heading<0)
	{
		*heading+=2*(float)M_PI;
	}
	*heading*=180/M_PI;
	*heading+=MAG_DEC;
	if(*heading>360)
	{
		*heading-=360;
	}
	return status;
}
