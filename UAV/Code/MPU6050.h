/*
 * MPU6050.h
 *
 *  Created on: Jul 16, 2024
 *      Author: qian-jing
 */

#ifndef MPU6050_H_
#define MPU6050_H_

#include "i2c.h"

typedef struct
{
	unsigned char devAddr;
}MPU6050_t;

unsigned char mpu6050_init(MPU6050_t *obj);
unsigned char mpu6050_getAn(MPU6050_t *obj,float *pData);
unsigned char mpu6050_getGn(MPU6050_t *obj,float *pData);
unsigned char mpu6050_getT(MPU6050_t *obj,float *pData);
unsigned char mpu6050_getAngle(MPU6050_t *obj,float* kalmanAngleX,float* kalmanAngleY);
unsigned char mpu6050_getAngleGn(MPU6050_t *obj,float* pitch,float* roll,float* gx,float* gy);


#endif /* MPU6050_H_ */
