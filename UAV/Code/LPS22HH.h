/*
 * LPS22HH.h
 *
 *  Created on: Jul 13, 2024
 *      Author: qian-jing
 */

#ifndef LPS22HH_H_
#define LPS22HH_H_

#include "i2c.h"

typedef struct
{
	unsigned char devAddr;
}LPS22HH_t;

unsigned char lps22hh_init(LPS22HH_t* obj);
unsigned char lps22hh_get_tmp(LPS22HH_t* obj,float *tmp);
unsigned char lps22hh_get_prs(LPS22HH_t* obj,float *prs);
unsigned char lps22hh_get_alt(LPS22HH_t* obj,float *alt);


#endif /* LPS22HH_H_ */
