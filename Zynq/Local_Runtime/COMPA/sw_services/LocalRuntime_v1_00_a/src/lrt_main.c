/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 */


#define DOMAIN			0
#define SIZE			8
#define PERF_MON		0

#define FIFO_SIZE		4096
#define FIFO_IN_ADDR	XPAR_BRAM_0_BASEADDR
#define FIFO_OUT_ADDR	XPAR_BRAM_0_BASEADDR
#define FIFO_IN_DIR		0
#define FIFO_OUT_DIR	1

#include "platform.h"
#include "xparameters.h"
#include "lrt_prototypes.h"
//#include "lrt_cpu_cfg.h"
#include "xuartlite_l.h"
//#include "xmbox_hw.h"
#include "xgpio_l.h"
#include "lrt.h"

#if PERF_MON == 1
#include "xaxipmon_hw.h"
#endif


//#include "xintc_l.h"






void test1()
{
	print("Hello from test1\n\r");

	INT8U buffer_in[] = {1, 2, 3, 4, 5, 6, 7, 8};
	INT8U error;


	LRT_FIFO_HNDLE *out_fifo = create_fifo_hndl(FIFO_OUT_ADDR, FIFO_SIZE, FIFO_OUT_DIR);

#if PERF_MON == 1
	XAxiPmon_WriteReg(XPAR_AXI_PERF_MON_0_BASEADDR, XAPM_CTL_OFFSET, XAPM_CR_GCC_RESET_MASK);
	XAxiPmon_WriteReg(XPAR_AXI_PERF_MON_0_BASEADDR, XAPM_CTL_OFFSET, XAPM_CR_GCC_ENABLE_MASK);
#endif

	write_output_fifo(out_fifo, SIZE, buffer_in, &error);

#if PERF_MON == 1
	INT32U nb_cycles = XAxiPmon_ReadReg(XPAR_AXI_PERF_MON_0_BASEADDR, XAPM_GCC_LOW_OFFSET);
	print("number of cycles for writing: ");
	putnum(nb_cycles);
	print("\n\r");
#endif

//		XGpio_WriteReg(XPAR_LEDS_4BITS_BASEADDR, XGPIO_DATA_OFFSET, 0xFF);
}



void test2()
{
	print("Hello from test2\n\r");

	INT8U buffer_out[SIZE];
	INT8U i, error;


	LRT_FIFO_HNDLE *in_fifo = create_fifo_hndl(FIFO_IN_ADDR, FIFO_SIZE, FIFO_IN_DIR);

#if PERF_MON == 1
	XAxiPmon_WriteReg(XPAR_AXI_PERF_MON_0_BASEADDR, XAPM_CTL_OFFSET, XAPM_CR_GCC_RESET_MASK);
	XAxiPmon_WriteReg(XPAR_AXI_PERF_MON_0_BASEADDR, XAPM_CTL_OFFSET, XAPM_CR_GCC_ENABLE_MASK);
#endif

	blocking_read_input_fifo(in_fifo, SIZE, buffer_out, &error);

#if PERF_MON == 1
	INT32U nb_cycles = XAxiPmon_ReadReg(XPAR_AXI_PERF_MON_0_BASEADDR, XAPM_GCC_LOW_OFFSET);
	print("number of cycles for reading: ");
	putnum(nb_cycles);
	print("\n\r");
#endif

	if(error == OS_ERR_NONE)
	{
		print("Read values :\n\r");
		for(i=0;i<SIZE;i++)
		{
			putnum(buffer_out[i]);
			print("\n\r");
		}
	}
	else
	{
		print("Error :");
		putnum(error);
		print("\n\r");
	}
//		XGpio_WriteReg(XPAR_LEDS_4BITS_BASEADDR, XGPIO_DATA_OFFSET, 0xFF);
}




int main()
{
    init_platform();

	/*
	 * Enable interrupts for all devices that cause interrupts, and enable
	 * the INTC master enable bit.
	 */
//	XIntc_EnableIntr(XPAR_INTC_0_BASEADDR, XIN_INT_MASTER_ENABLE_MASK);

    functions_tbl[0] = test1;
    functions_tbl[1] = test2;

//	init_lrt(DOMAIN,XPAR_CPU_ID,0,0,0);
    init_lrt(XPAR_MBOX_0_BASEADDR);

    cleanup_platform();

    return 0;
}
