#ifndef __I2C_H
#define __I2C_H

typedef enum
{
	I2C_OK=0,
	I2C_BUSY=1,
	I2C_ERROR=2,
}I2C_Status;

void i2c_init();
unsigned char i2c_mem_write(unsigned char devAddr,unsigned char reg,unsigned char *pData,unsigned char amount);
unsigned char i2c_mem_read(unsigned char devAddr,unsigned char reg,unsigned char *pData,unsigned char amount);
#endif