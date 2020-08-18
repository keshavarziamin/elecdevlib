/*
 * debug.h
 *
 *  Created on: Jun 12, 2020
 *      Author: amin
 */

#ifndef DEBUG_H_
#define DEBUG_H_


#include "usart.h"
#include "stdio.h"

#define UART_DEBUG   huart6



int printf_hex(char *txt,uint8_t *array,uint16_t len);

#endif /* DEBUG_H_ */
