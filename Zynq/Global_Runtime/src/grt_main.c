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

#include <stdio.h>
#include "platform.h"
#include "grt_cfg.h"
#include "grt_prototypes.h"
#include "xparameters.h"


#define XPAR_MAILBOX_0_BASEADDR		0x43840000
#define XPAR_MAILBOX_1_BASEADDR 	0x43820000
#define XPAR_MAILBOX_2_BASEADDR 	0x43800000

#define NB_LRT						1

#define INITIATOR					XPAR_MAILBOX_0_BASEADDR
#define CONSUMER_1					XPAR_MAILBOX_0_BASEADDR
#define CONSUMER_2					XPAR_MAILBOX_1_BASEADDR

//#define CORTEX_CNTRL_FIFO_SIZE		1024
//unsigned char* cortex_cntrl_fifo_addr = (unsigned char*)(XPAR_PS_SHARED_BRAM_CTRL_0_S_AXI_BASEADDR + 0x3c00);
//extern LRT_FIFO_HNDLE fifo_hndl;

void print(char *str);

int main()
{
    init_platform();

    // Creates the control FIFO to communicate with the second Cortex.
//    create_fifo_hndl((unsigned int)cortex_cntrl_fifo_addr, CORTEX_CNTRL_FIFO_SIZE, 0);

    print("Global Runtime started\n\r");

    // TODO:Clear mailboxes.
//    flush_mailboxes(INITIATOR);

    LRT_MSG lrt_msg;

    // Create FIFO 1 on initiator lrt
    lrt_msg.msg_type = MSG_CREATE_FIFO;
    lrt_msg.fifo_id = 0;
    lrt_msg.direction = FIFO_OUT_DIR;

    send_ext_msg(INITIATOR, &lrt_msg, sizeof(LRT_MSG));



    // Create task 1 on initiator lrt
    lrt_msg.msg_type = MSG_CREATE_TASK;
    lrt_msg.task_id = 1;
    lrt_msg.function_id = 0;
    lrt_msg.fifo_in = 0;
    lrt_msg.fifo_out = 0;

   	send_ext_msg(INITIATOR, &lrt_msg, sizeof(LRT_MSG));

#if NB_LRT > 1
    // Create task 1 on lrt 0
   	msg_block.msg_type = MSG_CREATE_ACTION;
   	msg_block.task_id = 1;
   	msg_block.function_id = 1;
   	send_ext_msg(CONSUMER_1, &msg_block, 0);


   	// Start scheduling on lrt 0
   	msg_block.msg_type = MSG_START_TASK;
   	send_ext_msg(CONSUMER_1, &msg_block, 0);

# if NB_LRT > 2
   	// Create task 1 on lrt 1
   	msg_block.msg_type = MSG_CREATE_ACTION;
   	msg_block.task_id = 1;
   	msg_block.function_id = 1;
   	send_ext_msg(CONSUMER_2, &msg_block, 0);



   	// Start scheduling on lrt 1
   	msg_block.msg_type = MSG_START_TASK;
   	send_ext_msg(CONSUMER_2, &msg_block, 0);

#endif
#else
   	// Create task 2 on initiator lrt
    lrt_msg.msg_type = MSG_CREATE_TASK;
    lrt_msg.task_id = 2;
    lrt_msg.function_id = 1;
    lrt_msg.fifo_in = 0;
    lrt_msg.fifo_out = 0;

    send_ext_msg(INITIATOR, &lrt_msg, sizeof(LRT_MSG));
#endif


   	// Start scheduling on initiator lrt
   	lrt_msg.msg_type = MSG_START_SCHED;
   	send_ext_msg(INITIATOR, &lrt_msg, sizeof(LRT_MSG));




   	int counter = 1000000000;

   	while(counter--);
   	// Stop task 2 on initiator lrt
    lrt_msg.msg_type = MSG_STOP_TASK;
    lrt_msg.task_id = 2;

   	send_ext_msg(INITIATOR, &lrt_msg, sizeof(LRT_MSG));




    cleanup_platform();

    return 0;
}
