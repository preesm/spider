/*
 * lrt_debug.c
 *
 *  Created on: 23 janv. 2013
 *      Author: yoliva
 */

#include "xuartlite_l.h"
#include "xgpio_l.h"


#define XUartChanged_IsTransmitFull(BaseAddress)			 \
	((Xil_In32((BaseAddress) + 0x2C) & 	\
	 0x10) == 0x10)

void XUartChanged_SendByte(u32 BaseAddress, u8 Data)
{
		/*
		 * Wait until there is space in TX FIFO
		 */
		while (XUartChanged_IsTransmitFull(BaseAddress));

		/*
		 * Write the byte into the TX FIFO
		 */
		XGpio_WriteReg(BaseAddress, 0x30, Data);
}

void outbyte(char c) {
	 XUartChanged_SendByte(0xE0001000, c);
}

void zynq_print(char *ptr)
{
  while (*ptr) {
    outbyte (*ptr++);
  }
}

void zynq_putnum(unsigned int num)
{
  char  buf[9];
  int   cnt;
  char  *ptr;
  int   digit;

  ptr = buf;
  for (cnt = 7 ; cnt >= 0 ; cnt--) {
    digit = (num >> (cnt * 4)) & 0xf;

    if (digit <= 9)
      *ptr++ = (char) ('0' + digit);
    else
      *ptr++ = (char) ('a' - 10 + digit);
  }

  *ptr = (char) 0;
  zynq_print (buf);
}
