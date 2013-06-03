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
#include <string.h>
#include "platform.h"
#include "grt_cfg.h"
#include "grt_prototypes.h"
#include "xparameters.h"


#define XPAR_MAILBOX_0_BASEADDR		0x43840000
#define XPAR_MAILBOX_1_BASEADDR 	0x43820000
#define XPAR_MAILBOX_2_BASEADDR 	0x43800000

#define NB_LRT						1

#define LRT_1						XPAR_MAILBOX_0_BASEADDR
#define LRT_2						XPAR_MAILBOX_1_BASEADDR


#define FIFO_SIZE					64



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

    INT32U msg_send_test = 5;
    INT32U msg_recv_test = 0;
	send_ext_msg(LRT_1, MSG_STOP_TASK, &msg_send_test);
	send_ext_msg(LRT_1, MSG_STOP_TASK, &msg_send_test);
	wait_ext_msg(LRT_1, &msg_recv_test);


    // Create FIFO 1 on lrt 1
    MSG_CREATE_FIFO_STRUCT msg_create_fifo;

    msg_create_fifo.id 			= 1;
    msg_create_fifo.size 		= FIFO_SIZE;
    msg_create_fifo.mem_block 	= XPAR_PS_SHARED_BRAM_CTRL_0_S_AXI_BASEADDR;
    msg_create_fifo.block_ix 	= 0;
    msg_create_fifo.direction 	= FIFO_OUT_DIR;
    msg_create_fifo.init		= true;

    send_ext_msg(LRT_1, MSG_CREATE_FIFO, &msg_create_fifo);


    // Create FIFO 2 on lrt 1
    msg_create_fifo.id 			= 2;
    msg_create_fifo.size 		= FIFO_SIZE;
    msg_create_fifo.mem_block 	= XPAR_PS_SHARED_BRAM_CTRL_0_S_AXI_BASEADDR;
    msg_create_fifo.block_ix 	= 1;
    msg_create_fifo.direction 	= FIFO_OUT_DIR;
    msg_create_fifo.init		= true;

    send_ext_msg(LRT_1, MSG_CREATE_FIFO, &msg_create_fifo);


    // Creating task/actor 1 on lrt 1
    MSG_CREATE_TASK_STRUCT	msg_create_task_1;
    memset(&msg_create_task_1,'-',sizeof(MSG_CREATE_TASK_STRUCT));

    msg_create_task_1.task_id = 1;
    msg_create_task_1.nb_fifo_in = 0;
    msg_create_task_1.nb_fifo_out = 1;
    msg_create_task_1.fifo_out_id[0] = 1;
    msg_create_task_1.start_vextex_ix = 3;

    // Creating conditions.
    msg_create_task_1.nb_am_conditions = 1;

    msg_create_task_1.am_conditions[0].id = 1;
    msg_create_task_1.am_conditions[0].type = cond_check_out_fifo;
    msg_create_task_1.am_conditions[0].fifo_id = 1;
    msg_create_task_1.am_conditions[0].data_size = FIFO_SIZE - 1;

    // Creating vertices.
    msg_create_task_1.nb_am_vertices = 6;

    msg_create_task_1.am_vertices[0].id = 1;
    msg_create_task_1.am_vertices[0].type = vertex_state;
    msg_create_task_1.am_vertices[0].nb_conditions = 1;
    msg_create_task_1.am_vertices[0].conditions[0].ix = 0;
    msg_create_task_1.am_vertices[0].successor_ix[0] = 1;

    msg_create_task_1.am_vertices[1].id = 2;
    msg_create_task_1.am_vertices[1].type = vertex_test;
    msg_create_task_1.am_vertices[1].cond_ix = 0;
    msg_create_task_1.am_vertices[1].successor_ix[0] = 2;
    msg_create_task_1.am_vertices[1].successor_ix[1] = 4;

    msg_create_task_1.am_vertices[2].id = 3;
    msg_create_task_1.am_vertices[2].type = vertex_state;
    msg_create_task_1.am_vertices[2].nb_conditions = 1;
    msg_create_task_1.am_vertices[2].conditions[0].ix = 0;
    msg_create_task_1.am_vertices[2].conditions[0].value = true;
    msg_create_task_1.am_vertices[2].successor_ix[0] = 3;

    msg_create_task_1.am_vertices[3].id = 4;
    msg_create_task_1.am_vertices[3].type = vertex_exec;
    msg_create_task_1.am_vertices[3].action_funct_ix = 0;
    msg_create_task_1.am_vertices[3].successor_ix[0] = 0;

    msg_create_task_1.am_vertices[4].id = 5;
    msg_create_task_1.am_vertices[4].type = vertex_state;
    msg_create_task_1.am_vertices[4].nb_conditions = 1;
    msg_create_task_1.am_vertices[4].conditions[0].ix = 0;
    msg_create_task_1.am_vertices[4].conditions[0].value = false;
    msg_create_task_1.am_vertices[4].successor_ix[0] = 5;

    msg_create_task_1.am_vertices[5].id = 6;
    msg_create_task_1.am_vertices[5].type = vertex_wait;
    msg_create_task_1.am_vertices[5].successor_ix[0] = 0;

    send_ext_msg(LRT_1, MSG_CREATE_TASK, &msg_create_task_1);


    // Creating task/actor 3 on lrt 1
    MSG_CREATE_TASK_STRUCT	msg_create_task_3;
    memset(&msg_create_task_3,'-',sizeof(MSG_CREATE_TASK_STRUCT));

    msg_create_task_3.task_id = 3;
    msg_create_task_3.nb_fifo_in = 1;
    msg_create_task_3.nb_fifo_out = 0;
    msg_create_task_3.fifo_in_id[0] = 2;
    msg_create_task_3.start_vextex_ix = 0;

    // Creating conditions.
    msg_create_task_3.nb_am_conditions = 1;

    msg_create_task_3.am_conditions[0].id = 1;
    msg_create_task_3.am_conditions[0].type = cond_check_in_fifo;
    msg_create_task_3.am_conditions[0].fifo_id = 2;
    msg_create_task_3.am_conditions[0].data_size = FIFO_SIZE - 1;

    // Creating vertices.
    msg_create_task_3.nb_am_vertices = 6;

    msg_create_task_3.am_vertices[0].id = 1;
    msg_create_task_3.am_vertices[0].type = vertex_state;
    msg_create_task_3.am_vertices[0].nb_conditions = 1;
    msg_create_task_3.am_vertices[0].conditions[0].ix = 0;
    msg_create_task_3.am_vertices[0].successor_ix[0] = 1;

    msg_create_task_3.am_vertices[1].id = 2;
    msg_create_task_3.am_vertices[1].type = vertex_test;
    msg_create_task_3.am_vertices[1].cond_ix = 0;
    msg_create_task_3.am_vertices[1].successor_ix[0] = 2;
    msg_create_task_3.am_vertices[1].successor_ix[1] = 4;

    msg_create_task_3.am_vertices[2].id = 3;
    msg_create_task_3.am_vertices[2].type = vertex_state;
    msg_create_task_3.am_vertices[2].nb_conditions = 1;
    msg_create_task_3.am_vertices[2].conditions[0].ix = 0;
    msg_create_task_3.am_vertices[2].conditions[0].value = true;
    msg_create_task_3.am_vertices[2].successor_ix[0] = 3;

    msg_create_task_3.am_vertices[3].id = 4;
    msg_create_task_3.am_vertices[3].type = vertex_exec;
    msg_create_task_3.am_vertices[3].action_funct_ix = 1;
    msg_create_task_3.am_vertices[3].successor_ix[0] = 0;

    msg_create_task_3.am_vertices[4].id = 5;
    msg_create_task_3.am_vertices[4].type = vertex_state;
    msg_create_task_3.am_vertices[4].nb_conditions = 1;
    msg_create_task_3.am_vertices[4].conditions[0].ix = 0;
    msg_create_task_3.am_vertices[4].conditions[0].value = false;
    msg_create_task_3.am_vertices[4].successor_ix[0] = 5;

    msg_create_task_3.am_vertices[5].id = 6;
    msg_create_task_3.am_vertices[5].type = vertex_wait;
    msg_create_task_3.am_vertices[5].successor_ix[0] = 0;

//    send_ext_msg(LRT_1, MSG_CREATE_TASK, &msg_create_task_3);



   	//*** Starting lrt 1.
   	send_ext_msg(LRT_1, MSG_START_SCHED, (void*)0);

	INT32U counter = 1000000; while(counter--);

   	//*** Stopping task 1 on lrt 1.
   	INT32U	msg_stop_task, msg_curr_vertex_id;
   	msg_stop_task = 1; msg_curr_vertex_id = 0;

	send_ext_msg(LRT_1, MSG_STOP_TASK, &msg_stop_task);
	wait_ext_msg(LRT_1, &msg_curr_vertex_id);

   	//*** Stopping task 3 on lrt 1.
//   	msg_stop_task = 3;
//	send_ext_msg(LRT_1, MSG_STOP_TASK, &msg_stop_task);





    // Create FIFO 1 on lrt 2
    msg_create_fifo.id 			= 1;
    msg_create_fifo.size 		= FIFO_SIZE;
    msg_create_fifo.mem_block 	= XPAR_PS_SHARED_BRAM_CTRL_0_S_AXI_BASEADDR;
    msg_create_fifo.block_ix 	= 0;
    msg_create_fifo.direction 	= FIFO_IN_DIR;
    msg_create_fifo.init		= false;

    send_ext_msg(LRT_2, MSG_CREATE_FIFO, &msg_create_fifo);

    // Create FIFO 2 on lrt 2
    msg_create_fifo.id 			= 2;
    msg_create_fifo.size 		= FIFO_SIZE;
    msg_create_fifo.mem_block 	= XPAR_PS_SHARED_BRAM_CTRL_0_S_AXI_BASEADDR;
    msg_create_fifo.block_ix 	= 1;
    msg_create_fifo.direction 	= FIFO_IN_DIR;
    msg_create_fifo.init		= false;

    send_ext_msg(LRT_2, MSG_CREATE_FIFO, &msg_create_fifo);

//    // Create FIFO 3 on lrt 2
//    msg_create_fifo.id 			= 3;
//    msg_create_fifo.size 		= FIFO_SIZE;
//    msg_create_fifo.mem_block 	= XPAR_PS_SHARED_BRAM_CTRL_0_S_AXI_BASEADDR;
//    msg_create_fifo.block_ix 	= 2;
//    msg_create_fifo.direction 	= FIFO_IN_DIR;
//    msg_create_fifo.init		= true;
//
//    send_ext_msg(LRT_2, MSG_CREATE_FIFO, &msg_create_fifo);

    // Creating task/actor 1 on lrt 1
    memset(&msg_create_task_1,'-',sizeof(MSG_CREATE_TASK_STRUCT));

    msg_create_task_1.task_id = 1;
    msg_create_task_1.nb_fifo_in = 0;
    msg_create_task_1.nb_fifo_out = 1;
    msg_create_task_1.fifo_out_id[0] = 1;
    msg_create_task_1.start_vextex_ix = msg_curr_vertex_id - 1;

    // Creating conditions.
    msg_create_task_1.nb_am_conditions = 1;

    msg_create_task_1.am_conditions[0].id = 1;
    msg_create_task_1.am_conditions[0].type = cond_check_out_fifo;
    msg_create_task_1.am_conditions[0].fifo_id = 1;
    msg_create_task_1.am_conditions[0].data_size = FIFO_SIZE - 1;

    // Creating vertices.
    msg_create_task_1.nb_am_vertices = 6;

    msg_create_task_1.am_vertices[0].id = 1;
    msg_create_task_1.am_vertices[0].type = vertex_state;
    msg_create_task_1.am_vertices[0].nb_conditions = 1;
    msg_create_task_1.am_vertices[0].conditions[0].ix = 0;
    msg_create_task_1.am_vertices[0].successor_ix[0] = 1;

    msg_create_task_1.am_vertices[1].id = 2;
    msg_create_task_1.am_vertices[1].type = vertex_test;
    msg_create_task_1.am_vertices[1].cond_ix = 0;
    msg_create_task_1.am_vertices[1].successor_ix[0] = 2;
    msg_create_task_1.am_vertices[1].successor_ix[1] = 4;

    msg_create_task_1.am_vertices[2].id = 3;
    msg_create_task_1.am_vertices[2].type = vertex_state;
    msg_create_task_1.am_vertices[2].nb_conditions = 1;
    msg_create_task_1.am_vertices[2].conditions[0].ix = 0;
    msg_create_task_1.am_vertices[2].conditions[0].value = true;
    msg_create_task_1.am_vertices[2].successor_ix[0] = 3;

    msg_create_task_1.am_vertices[3].id = 4;
    msg_create_task_1.am_vertices[3].type = vertex_exec;
    msg_create_task_1.am_vertices[3].action_funct_ix = 0;
    msg_create_task_1.am_vertices[3].successor_ix[0] = 0;

    msg_create_task_1.am_vertices[4].id = 5;
    msg_create_task_1.am_vertices[4].type = vertex_state;
    msg_create_task_1.am_vertices[4].nb_conditions = 1;
    msg_create_task_1.am_vertices[4].conditions[0].ix = 0;
    msg_create_task_1.am_vertices[4].conditions[0].value = false;
    msg_create_task_1.am_vertices[4].successor_ix[0] = 5;

    msg_create_task_1.am_vertices[5].id = 6;
    msg_create_task_1.am_vertices[5].type = vertex_wait;
    msg_create_task_1.am_vertices[5].successor_ix[0] = 0;

    send_ext_msg(LRT_2, MSG_CREATE_TASK, &msg_create_task_1);


    // Creating task/actor 2 on lrt 2
    MSG_CREATE_TASK_STRUCT	msg_create_task_2;
    memset(&msg_create_task_2,'-',sizeof(MSG_CREATE_TASK_STRUCT));

    msg_create_task_2.task_id = 2;
    msg_create_task_2.nb_fifo_in = 1;
    msg_create_task_2.nb_fifo_out = 1;
    msg_create_task_2.fifo_in_id[0] = 1;
    msg_create_task_2.fifo_out_id[0] = 2;
    msg_create_task_2.start_vextex_ix = 5;

    // Creating conditions.
    msg_create_task_2.nb_am_conditions = 2;

    msg_create_task_2.am_conditions[0].id = 1;
    msg_create_task_2.am_conditions[0].type = cond_check_in_fifo;
    msg_create_task_2.am_conditions[0].fifo_id = 1;
    msg_create_task_2.am_conditions[0].data_size = FIFO_SIZE - 1;

    msg_create_task_2.am_conditions[1].id = 2;
    msg_create_task_2.am_conditions[1].type = cond_check_out_fifo;
    msg_create_task_2.am_conditions[1].fifo_id = 2;
    msg_create_task_2.am_conditions[1].data_size = FIFO_SIZE - 1;

//    msg_create_task_2.am_conditions[2].id = 3;
//    msg_create_task_2.am_conditions[2].type = cond_check_in_fifo;
//    msg_create_task_2.am_conditions[2].fifo_id = 3;
//    msg_create_task_2.am_conditions[2].data_size = FIFO_SIZE - 1;

    // Creating vertices.
    msg_create_task_2.nb_am_vertices = 10;

    msg_create_task_2.am_vertices[0].id = 1;
    msg_create_task_2.am_vertices[0].type = vertex_state;
    msg_create_task_2.am_vertices[0].nb_conditions = 2;
    msg_create_task_2.am_vertices[0].conditions[0].ix = 0;
    msg_create_task_2.am_vertices[0].conditions[1].ix = 1;
    msg_create_task_2.am_vertices[0].successor_ix[0] = 1;

    msg_create_task_2.am_vertices[1].id = 2;
    msg_create_task_2.am_vertices[1].type = vertex_test;
    msg_create_task_2.am_vertices[1].cond_ix = 0;
    msg_create_task_2.am_vertices[1].successor_ix[0] = 2;
    msg_create_task_2.am_vertices[1].successor_ix[1] = 8;

    msg_create_task_2.am_vertices[2].id = 3;
    msg_create_task_2.am_vertices[2].type = vertex_state;
    msg_create_task_2.am_vertices[2].nb_conditions = 2;
    msg_create_task_2.am_vertices[2].conditions[0].ix = 0;
    msg_create_task_2.am_vertices[2].conditions[0].value = true;
    msg_create_task_2.am_vertices[2].conditions[1].ix = 1;
    msg_create_task_2.am_vertices[2].successor_ix[0] = 3;

    msg_create_task_2.am_vertices[3].id = 4;
    msg_create_task_2.am_vertices[3].type = vertex_test;
    msg_create_task_2.am_vertices[3].cond_ix = 1;
    msg_create_task_2.am_vertices[3].successor_ix[0] = 4;
    msg_create_task_2.am_vertices[3].successor_ix[1] = 6;

    msg_create_task_2.am_vertices[4].id = 5;
    msg_create_task_2.am_vertices[4].type = vertex_state;
    msg_create_task_2.am_vertices[4].nb_conditions = 2;
    msg_create_task_2.am_vertices[4].conditions[0].ix = 0;
    msg_create_task_2.am_vertices[4].conditions[0].value = true;
    msg_create_task_2.am_vertices[4].conditions[1].ix = 1;
    msg_create_task_2.am_vertices[4].conditions[1].value = true;
    msg_create_task_2.am_vertices[4].successor_ix[0] = 5;

    msg_create_task_2.am_vertices[5].id = 6;
    msg_create_task_2.am_vertices[5].type = vertex_exec;
    msg_create_task_2.am_vertices[5].action_funct_ix = 1;
    msg_create_task_2.am_vertices[5].successor_ix[0] = 0;

    msg_create_task_2.am_vertices[6].id = 7;
    msg_create_task_2.am_vertices[6].type = vertex_state;
    msg_create_task_2.am_vertices[6].nb_conditions = 2;
    msg_create_task_2.am_vertices[6].conditions[0].ix = 0;
    msg_create_task_2.am_vertices[6].conditions[0].value = true;
    msg_create_task_2.am_vertices[6].conditions[1].ix = 1;
    msg_create_task_2.am_vertices[6].conditions[1].value = false;
    msg_create_task_2.am_vertices[6].successor_ix[0] = 7;

    msg_create_task_2.am_vertices[7].id = 8;
    msg_create_task_2.am_vertices[7].type = vertex_wait;
    msg_create_task_2.am_vertices[7].successor_ix[0] = 2;

    msg_create_task_2.am_vertices[8].id = 9;
    msg_create_task_2.am_vertices[8].type = vertex_state;
    msg_create_task_2.am_vertices[8].nb_conditions = 2;
    msg_create_task_2.am_vertices[8].conditions[0].ix = 0;
    msg_create_task_2.am_vertices[8].conditions[0].value = false;
    msg_create_task_2.am_vertices[8].conditions[1].ix = 1;
    msg_create_task_2.am_vertices[8].successor_ix[0] = 9;

    msg_create_task_2.am_vertices[9].id = 10;
    msg_create_task_2.am_vertices[9].type = vertex_wait;
    msg_create_task_2.am_vertices[9].successor_ix[0] = 0;

//    send_ext_msg(LRT_2, MSG_CREATE_TASK, &msg_create_task_2);


   	// Starting lrt 2.
	send_ext_msg(LRT_2, MSG_START_SCHED, (void*)0);

	// Stopping task 2 on lrt 2.
//	msg_stop_task = 2;
//	send_ext_msg(LRT_2, MSG_STOP_TASK, &msg_stop_task);

	counter = 1000000; while(counter--);

	//*** Stopping task 1 on lrt 2.
	msg_stop_task = 1; msg_curr_vertex_id = 0;

	send_ext_msg(LRT_2, MSG_STOP_TASK, &msg_stop_task);
	wait_ext_msg(LRT_2, &msg_curr_vertex_id);

    cleanup_platform();

    return 0;
}
