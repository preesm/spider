/*
 * print.c
 *
 *  Created on: Aug 23, 2013
 *      Author: jheulot
 */

#include <stdio.h>
#include "print.h"

void zynq_puts(const char* s){
	printf("%s",s);
}
void zynq_putdec(UINT32 value){
	printf("%d",value);
}
void zynq_puthex(UINT32 value){
	printf("0x%x",value);
}
