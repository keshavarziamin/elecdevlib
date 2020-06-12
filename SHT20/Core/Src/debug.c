/*
 * debug.c
 *
 *  Created on: Jun 12, 2020
 *      Author: amin
 */

#include "debug.h"


int __io_putchar(int ch){
	 uint8_t c[1];
	 c[0] = ch & 0x00FF;
	 HAL_UART_Transmit(&UART_DEBUG, &*c, 1, 10);
	 return ch;
	}

int _write(int file,char *ptr, int len){
	 int DataIdx;
	 for(DataIdx= 0; DataIdx< len; DataIdx++)
		 __io_putchar(*ptr++);
	 return len;
}
