/*
 * QMC5883L.h
 *
 *  Created on: Jul 18, 2024
 *      Author: qian-jing
 */

#ifndef QMC5883L_H_
#define QMC5883L_H_
#include "i2c.h"
typedef struct
{
	unsigned char devAddr;//0x0D
}QMC5883L_t;
unsigned char qmc5883l_init(QMC5883L_t *obj);
unsigned char qmc5883l_calibrate(QMC5883L_t *obj);
unsigned char qmc5883l_getHeading(QMC5883L_t *obj,float *direction);
#endif /* QMC5883L_H_ */
