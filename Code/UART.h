#ifndef __UART_H
#define __UART_H
#include <stdio.h>
void uart1_init(); //115200
void uart1_sendByte(unsigned char dat);
void uart1_sendString(unsigned char *str);
void uart2_init(); //100,000 24M
#endif