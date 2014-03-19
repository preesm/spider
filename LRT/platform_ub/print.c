/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

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
