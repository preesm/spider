/*
 * main.c
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */

#include <stdlib.h>
#include <stdio.h>

#include "sobel.h"

#include <lrt.h>
#include <lrt_prototypes.h>

#include <xparameters.h>

#include <platform.h>
#include <print.h>


int main(int argc, char** argv){
	init_platform();

	cpuId = XPAR_CPU_ID+1;

	zynq_puts("Start Sobel LRT on UB");zynq_putdec(cpuId);zynq_puts("\n");

	/* Sobel */
	functions_tbl[3] = sobel;

	init_lrt();

	cleanup_platform();
	return 0;
}
