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

#include "platform.h"
#include "xparameters.h"
#include "lrt_prototypes.h"
//#include "lrt_cpu_cfg.h"
#include "xuartlite_l.h"
//#include "xmbox_hw.h"
#include "xgpio_l.h"
#include "lrt.h"


#define DOMAIN			0
#define SIZE			8


void test()
{
	print("Hello from microblaze ");
	putnum(XPAR_CPU_ID);
	print("\n\r");

	INT8U buffer_in[] = {1, 2, 3, 4, 5, 6, 7, 8};
	INT8U buffer_out[SIZE];
	INT8U error;


	OS_MEM* sh_mem = get_sh_mem_hndl(0);

	write_sh_mem(sh_mem, SIZE, buffer_in, &error);

	read_sh_mem(sh_mem, SIZE, buffer_out, &error);

	print("Read values :\n\r");

	INT8U i;
	for(i=0;i<SIZE;i++)
	{
		putnum(buffer_out[i]);
		print("\n\r");
	}
//		XGpio_WriteReg(XPAR_LEDS_4BITS_BASEADDR, XGPIO_DATA_OFFSET, 0xFF);
}


//extern void create_local_actions();

int main()
{
    init_platform();

    functions_tbl[0] = test;

//	init_lrt(DOMAIN,XPAR_CPU_ID,0,0,0);
    init_lrt();

    cleanup_platform();

    return 0;
}
