/*
 * print.h
 *
 *  Created on: Aug 23, 2013
 *      Author: jheulot
 */

#ifndef PRINT_H_
#define PRINT_H_

#include <types.h>

void zynq_puts(const char* s);
void zynq_putdec(UINT32);
void zynq_puthex(UINT32);

#endif /* PRINT_H_ */
