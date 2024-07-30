/*
 * SBUS.h
 *
 *  Created on: Jul 15, 2024
 *      Author: qian-jing
 */

#ifndef SBUS_H_
#define SBUS_H_

void sbus_init();
void sbus_inirq();
int sbus_getData(int ch_index);
unsigned char sbus_getRxCnt();
void sbus_setRxCnt(unsigned char val);
unsigned char sbus_getRx2TimeOut();
void sbus_setRx2TimeOut(unsigned char val);

#endif /* SBUS_H_ */
