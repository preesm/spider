/*
 * print.c
 *
 *  Created on: Aug 23, 2013
 *      Author: jheulot
 */

#include <stdio.h>
#include <types.h>
#include <print.h>
#include <platform.h>
#include <string.h>
#include <xparameters.h>
#include <xuartlite_l.h>

void print(char *);

static BOOLEAN newLine=1;

void zynq_putc(char c){
	if(newLine==1){
		print("[UB");
		XUartLite_SendByte(STDOUT_BASEADDRESS, cpuId+'0');
		print("] ");
		newLine = 0;
	}
	if(c == '\n') newLine=1;
	XUartLite_SendByte(STDOUT_BASEADDRESS, c);
}

void zynq_puts(const char* s){
	while(*s != '\0')
		zynq_putc(*(s++));
}

void zynq_putdec(UINT32 num){
	char  buf[11];
	int   cnt=0, digit;
	unsigned int   div;
	BOOLEAN wordStarted=0;

	for (div = 1000000000 ; div > 0 ; div/=10) {
		digit = (num / div)%10 ;
		if(wordStarted || digit != 0){
			buf[cnt++] = digit + '0';
			wordStarted = 1;
		}
	}

	if(cnt == 0) buf[cnt++] = '0';

	buf[cnt]='\0';
	print (buf);
}
void zynq_puthex(UINT32 num){
	char  buf[11];
	int   cnt;
	char  *ptr;
	int   digit;

	ptr = buf;
	*ptr++ = '0';
	*ptr++ = 'x';
	for (cnt = 7 ; cnt >= 0 ; cnt--) {
		digit = (num >> (cnt * 4)) & 0xf;
		if (digit <= 9)
			*ptr++ = (char) ('0' + digit);
		else
			*ptr++ = (char) ('a' - 10 + digit);
	}

	*ptr = (char) 0;
	print (buf);
}

void zynq_test(){
	zynq_puts("Print 100 in dec: ");
	zynq_putdec(100);
	zynq_puts("\nPrint 0x100 in hex: ");
	zynq_puthex(0x100);
}
