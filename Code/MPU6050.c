/*
 * MPU6050.c
 *
 *  Created on: Jul 16, 2024
 *      Author: qian-jing
 */
#include <STC8051U.H>
#include <math.h>
#include "main.h"
#include "MPU6050.h"
#include "timer.h"
#ifdef DEBUG
#include "oled.h"
#endif


#define SELF_TEST_X 0x0D
#define SELF_TEST_Y 0x0E
#define SELF_TEST_Z 0x0F
#define SELF_TEST_A 0x10
#define SMPLRT_DIV	0x19
#define CONFIG			0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define FIFO_EN			 0x23
#define I2C_MST_CTRL 0x24
#define I2C_SLV0_ADDR 0x25
#define I2C_SLV0_REG  0x26
#define I2C_SLV0_CTRL 0x27
#define I2C_SLV1_ADDR 0x28
#define I2C_SLV1_REG  0x29
#define I2C_SLV1_CTRL 0x2A
#define I2C_SLV2_ADDR 0x2B
#define I2C_SLV2_REG  0x2C
#define I2C_SLV2_CTRL 0x2D
#define I2C_SLV3_ADDR 0x2E
#define I2C_SLV3_REG  0x2F
#define I2C_SLV3_CTRL 0x30
#define I2C_SLV4_ADDR 0x31
#define I2C_SLV4_REG  0x32
#define I2C_SLV4_DO   0x33
#define I2C_SLV4_CTRL 0x34
#define I2C_SLV4_DI   0x35
#define I2C_MST_STATUS 0x36
#define INT_PIN_CFG    0x37
#define INT_ENABLE     0x38
#define INT_STATUS     0x3A
#define ACCEL_XOUT 0x3B//H->L 2Byte
#define ACCEL_YOUT 0x3D
#define ACCEL_ZOUT 0x3F
#define TEMP_OUT   0x41
#define GYRO_XOUT  0x43
#define GYRO_YOUT  0x45
#define GYRO_ZOUT  0x47
#define EXT_SENS_DATA 0x49 //... 24Byte
#define I2C_SLV_DO 0x63 //... 4Byte
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET 0x68
#define USER_CTRL 0x6A
#define PWR_MGMT 0x6B //2Byte
#define FIFO_COUNT 0x72 //2Byte H->L
#define FIFO_R_W 0x74
#define WHO_AM_I 0x75

#define I_AM 0x68

#define RAD_TO_DEG 57.295779513082320876798154814105
static unsigned char write_reg(MPU6050_t *obj,unsigned char reg,unsigned char *pData,unsigned char amount)
{
	if(i2c_mem_write(obj->devAddr,reg,pData,amount))
	{
		return 1;
	}
	return 0;
}

static unsigned char read_reg(MPU6050_t *obj,unsigned char reg,unsigned char *pData,unsigned char amount)
{
	if(i2c_mem_read(obj->devAddr,reg,pData,amount))
	{
		return 1;
	}
	return 0;
}

unsigned char mpu6050_init(MPU6050_t *obj)
{
	unsigned char cmd;
	unsigned char status;
	status=read_reg(obj, WHO_AM_I, &cmd, 1);
	if(cmd!=I_AM)
	{
		return 2;
	}
	cmd=0x01;
	status=write_reg(obj,PWR_MGMT,&cmd,1);
	cmd=0x09;
	status=write_reg(obj,SMPLRT_DIV,&cmd,1);
	cmd=0x06;
	status=write_reg(obj, CONFIG, &cmd, 1);
//	cmd=0xF8;
//	status=write_reg(obj, GYRO_CONFIG, &cmd, 1);
//	cmd=0xF8;
//	status=write_reg(obj, ACCEL_CONFIG, &cmd, 1);
	cmd=0x18;
	status=write_reg(obj, GYRO_CONFIG, &cmd, 1);
	cmd=0x18;
	status=write_reg(obj, ACCEL_CONFIG, &cmd, 1);
	cmd=0x01;
	status=write_reg(obj, INT_ENABLE, &cmd, 1);
	return status;
}

unsigned char getAnRaw(MPU6050_t *obj,unsigned char *pRaw) //6
{
	unsigned char cmd;
	if(read_reg(obj, INT_STATUS, &cmd, 1))//清空中断标志位
	{
		return 1;
	}
	if((cmd&0x01)!=0x01)
	{
		return 1;
	}
	if(read_reg(obj, ACCEL_XOUT, pRaw, 6))
	{
		return 1;
	}
	return 0;
}

unsigned char getGnRaw(MPU6050_t *obj,unsigned char *pRaw) //6
{
	unsigned char cmd;
	if(read_reg(obj, INT_STATUS, &cmd, 1))//
	{
		return 1;
	}
	if((cmd&0x01)!=0x01)
	{
		return 1;
	}
	if(read_reg(obj, GYRO_XOUT, pRaw, 6))
	{
		return 1;
	}
	return 0;
}

unsigned char getTRaw(MPU6050_t *obj,unsigned char *pRaw) //6
{
	unsigned char cmd;
	if(read_reg(obj, INT_STATUS, &cmd, 1))//清空中断标志位
	{
		return 1;
	}
	if((cmd&0x01)!=0x01)
	{
		return 1;
	}
	if(read_reg(obj, TEMP_OUT, pRaw, 1))
	{
		return 1;
	}
	return 0;
}

unsigned char getAllRaw(MPU6050_t *obj,unsigned char *pRaw) //14 A6 T2 G6
{
	unsigned char cmd;
	if(read_reg(obj, INT_STATUS, &cmd, 1))//清空中断标志位
	{
		return 1;
	}
	if((cmd&0x01)!=0x01)
	{
		return 1;
	}
	if(read_reg(obj, ACCEL_XOUT, pRaw, 14))
	{
		return 1;
	}
	return 0;
}


unsigned char mpu6050_getAn(MPU6050_t *obj,float *pData)//An[3]
{
	unsigned char pRaw[6]={0};
	if(getAnRaw(obj, pRaw))
	{
		return 1;
	}
	pData[0]=(int)(pRaw[0]<<8|pRaw[1]);
	pData[1]=(int)(pRaw[2]<<8|pRaw[3]);
	pData[2]=(int)(pRaw[4]<<8|pRaw[5]);
	pData[0]=(pData[0] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	pData[1]=(pData[1] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	pData[2]=(pData[2] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	return 0;
}

unsigned char mpu6050_getGn(MPU6050_t *obj,float *pData)//pData[3]
{
	unsigned char pRaw[6]={0};
	if(getAnRaw(obj, pRaw))
	{
		return 1;
	}
	pData[0]=(int)(pRaw[0]<<8|pRaw[1]);
	pData[1]=(int)(pRaw[2]<<8|pRaw[3]);
	pData[2]=(int)(pRaw[4]<<8|pRaw[5]);
	pData[0]=(pData[0] - (-32767.0)) / (32767.0-(-32767.0)) * (2000.0-(-2000.0)) + (-2000.0);
	pData[1]=(pData[1] - (-32767.0)) / (32767.0-(-32767.0)) * (2000.0-(-2000.0)) + (-2000.0);
	pData[2]=(pData[2] - (-32767.0)) / (32767.0-(-32767.0)) * (2000.0-(-2000.0)) + (-2000.0);
	return 0;
}

unsigned char mpu6050_getT(MPU6050_t *obj,float *pData)//pData[1]
{
	unsigned char pRaw[2];
	if(getTRaw(obj,pRaw))
	{
		return 1;
	}
	pData[0]=(int)(pRaw[0]<<8|pRaw[1]);
	pData[0]=pData[0]/340.0 + 36.53;
	return 0;
}

unsigned char getAllData(MPU6050_t *obj,float *an,float* t,float *gn)//An[3]
{
	unsigned char pRaw[14]={0};
	if(getAllRaw(obj, pRaw))
	{
		return 1;
	}
	an[0]=(int)(pRaw[0]<<8|pRaw[1]);
	an[1]=(int)(pRaw[2]<<8|pRaw[3]);
	an[2]=(int)(pRaw[4]<<8|pRaw[5]);
	an[0]=(an[0] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	an[1]=(an[1] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	an[2]=(an[2] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);

	*t=(int)(pRaw[6]<<8|pRaw[7]);
	*t=*t/340.0 + 36.53;

	gn[0]=(int)(pRaw[8]<<8|pRaw[9]);
	gn[1]=(int)(pRaw[10]<<8|pRaw[11]);
	gn[2]=(int)(pRaw[12]<<8|pRaw[13]);
	gn[0]=(gn[0] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	gn[1]=(gn[1] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	gn[2]=(gn[2] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	return 0;
}

unsigned char getAll(MPU6050_t *obj,unsigned char *pRaw,float *an,float* t,float *gn)//An[3]
{
	if(getAllRaw(obj, pRaw))
	{
		return 1;
	}
	an[0]=(int)(pRaw[0]<<8|pRaw[1]);
	an[1]=(int)(pRaw[2]<<8|pRaw[3]);
	an[2]=(int)(pRaw[4]<<8|pRaw[5]);
	an[0]=(an[0] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	an[1]=(an[1] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	an[2]=(an[2] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);

	*t=(int)(pRaw[6]<<8|pRaw[7]);
	*t=*t/340.0 + 36.53;

	gn[0]=(int)(pRaw[8]<<8|pRaw[9]);
	gn[1]=(int)(pRaw[10]<<8|pRaw[11]);
	gn[2]=(int)(pRaw[12]<<8|pRaw[13]);
	gn[0]=(gn[0] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	gn[1]=(gn[1] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	gn[2]=(gn[2] - (-32767.0)) / (32767.0-(-32767.0)) * (16.0-(-16.0)) + (-16.0);
	return 0;
}


unsigned char mpu6050_getAngleGn(MPU6050_t *obj,float* pitch,float* roll,float* gx,float* gy)
{
	static int ax;
	static int ay;
	static int az;
	unsigned char pRaw[14]={0};
	unsigned int sqrt_temp=0;
	if(getAllRaw(obj, pRaw)==0)
	{
		ax=((int)pRaw[0]<<8|pRaw[1]);
		ay=((int)pRaw[2]<<8|pRaw[3]);
		az=((int)pRaw[4]<<8|pRaw[5]);
		*gx=((int)pRaw[8]<<8|pRaw[9]);
		*gy=((int)pRaw[10]<<8)|pRaw[11];
		*pitch = atan2(-ax, az) * RAD_TO_DEG;
		*roll  = atan2( ay, az) * RAD_TO_DEG;
	}
	else
	{
		return 1;
	}
	
	return 0;
}
