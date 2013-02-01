/*
 * rt_ext_msg_mngr.c
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

#include <xil_io.h>
#include "lrt_prototypes.h"
#include "lrt_cpu_cfg.h"
#include "xparameters.h"
#include "xmbox_hw.h"
#include "xgpio_l.h"


//--- Global variables ----//
static EXT_MSG_STRUCT ext_msg;						// To store an external message.
static INT32U tasks_stack[OS_DEFAULT_STACK_SIZE];	// Stack pour the tasks. Currently we need only one zone.
OS_MEM OSMemTbl[OS_MAX_MEM_PART];					/* Storage for memory partition manager            */



//extern mtapi_task_id_t action_cntr;					// Action's counter.
FUNCTION_TYPE functions_tbl[NB_LOCAL_FUNCTIONS];

/**********************************************************************************************************
*                                              blocking_fifo_write
* Description: Writes a 32 bits block into the FIFO. Blocks if FIFO is full.
*
* Arguments  :
* 			   data - data to be copied.
*
* Returns    : none
**********************************************************************************************************/
void blocking_fifo_write(INT32U data)
{
	// Wait until there is space in the FIFO
	while(XMbox_IsFullHw(XPAR_MBOX_0_BASEADDR));

	XMbox_WriteMBox(XPAR_MBOX_0_BASEADDR, data);
}



/**********************************************************************************************************
*                                              blocking_fifo_read
* Description: Reads a 32 bits block from the FIFO. Blocks if FIFO is empty.
*
* Arguments  : none
*
* Returns    : the read data.
**********************************************************************************************************/
INT32U blocking_fifo_read()
{
	// Wait until there is some data in the FIFO
	while(XMbox_IsEmptyHw(XPAR_MBOX_0_BASEADDR));

	return XMbox_ReadMBox(XPAR_MBOX_0_BASEADDR);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                              local_rt_call
*
* Description: This is the function that decodes an external message. The idea is to separated the message
* 			decoding from the way the message is received (polling or interrupt).
*
* Arguments  : none
*
* Returns    : none
*
*********************************************************************************************************
*/

static void local_rt_call()
{
//	ext_msg.task_id = 1;
//	ext_msg.funct_addr = test;
//	ext_msg.msg_type = create_action;

    switch (ext_msg.msg_type)
    {
		case MSG_CREATE_ACTION:
//			mtapi_task_create(ext_msg.funct_addr);
//			mtapi_task_start(ext_msg.funct_addr);// just for test.
//			mtapi_action_create(ext_msg.action_cntr,
//					ext_msg.funct_addr,
//					MCA_NULL,
//					0,
//					&action_attributes,
//					&status);
//			action_cntr++;
			OSTaskCreateExt(functions_tbl[ext_msg.function_id],	// the task's function.
					(void*)0,									// parameters for the task's function.
					&tasks_stack[OS_DEFAULT_STACK_SIZE],		// the top of the stack (highest address).
					ext_msg.task_id,							// task's priority.
					ext_msg.task_id,							// task's id.
					tasks_stack,								// the bottom of the stack (lower address).
					OS_DEFAULT_STACK_SIZE,						// the size of the stack.
					(void*)0,									// a pointer to a structure with more data.
					0);											// other options.
			OSRunning = true;
			OS_Sched();
			break;

		case MSG_CREATE_SH_MEM:

			break;

		case MSG_START_TASK:

			break;

		default:
			break;
	}
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                              wait_for_ext_msg
*
* Description: This is the function of the idle task. It waits for a external message is placed into
* 			the mailbox or memory address.
*
* Arguments  : //addr (optional) is a memory address in case a shared memory is used instead of a mailbox.
*
* Returns    : none
*
*********************************************************************************************************
*/
//void  wait_for_ext_msg(INT32U addr)
void  wait_for_ext_msg()
{
#if PROCESSOR_COMM
	EXT_MSG_BLOCK* ptr_ext_msg = (EXT_MSG_STRUCT*)(addr);
#else
    INT8U nb_reads = sizeof(EXT_MSG_STRUCT)/4;		// Number of 32bits words to read to get a complete msg.
	INT32U *ptr = (INT32U*)&ext_msg;
#endif

#if PROCESSOR_COMM
	while(ptr_ext_msg->msg_type == none);
//    OS_ENTER_CRITICAL();
//    OSIdleCtr++;
//    OS_EXIT_CRITICAL();

	ext_msg.funct_addr = ptr_ext_msg->funct_addr; 	// Saves a copy of the message.
	ext_msg.msg_type   = ptr_ext_msg->msg_type;

	ptr_ext_msg->msg_type = none;					// Clears the shared location.
#else
	while(nb_reads > 0)
	{
		nb_reads--;
		*ptr++ = blocking_fifo_read();
	}
#endif
#if OS_DEBUG_EN > 0
	print("Local Runtime have got a message\n\r");
#endif
	local_rt_call();
#if OS_DEBUG_EN > 0
	print("Return from task's function\n\r");
#endif

}




/*$PAGE*/
/*
*********************************************************************************************************
*                                              read_fsl_fifo (work in progress!!)
*
* Description: It reads data from the FSL fifo.
*
* Arguments  :
*
* Returns    : none
*
*********************************************************************************************************
*/
void  read_fsl_fifo()
{
	INT32U data;

	getfsl(data, 0);

#if OS_DEBUG_EN > 0
	print("Local Runtime 1 has read");
	putnum(data);
	print("from FSL\n\r");
#endif

}




/*$PAGE*/
/*
*********************************************************************************************************
*                                              write_fsl_fifo (work in progress!!)
*
* Description: It writes data into the FSL fifo.
*
* Arguments  :
*
* Returns    : none
*
*********************************************************************************************************
*/
void  write_fsl_fifo(INT32U data)
{
	putfsl(data, 0);

#if OS_DEBUG_EN > 0
	print("Local Runtime 0 has put data into FSL\n\r");
#endif
}












OS_MEM* get_sh_mem_hndl(INT32U index)
{
	return &OSMemTbl[index];
}










void  read_sh_mem(OS_MEM* sh_mem, INT32U size, INT8U* buffer, INT8U *perr)
{
	INT32U	i, wr_ix, rd_ix;

	// Get indices from the shared memory.
	wr_ix = *((INT32U*)sh_mem->MemBaseAddr + SH_MEM_WR_IX_OFFSET);
	rd_ix = *((INT32U*)(sh_mem->MemBaseAddr) + SH_MEM_RD_IX_OFFSET);

	if(wr_ix < rd_ix)					// If true, wr_ix reached the end of the memory and restarted from the beginning.
		wr_ix += sh_mem->OSMemBlkSize;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

	if((rd_ix + size) <= wr_ix)			// Reader is allowed to read until ReadIndex == WriteIndex, i.e. until FIFO is empty.
	{
		// Read the data.
		for(i=0; i<size; i++)
			buffer[i] = ((INT8U*)(sh_mem->DataBaseAddr))[(rd_ix + i) % sh_mem->OSMemBlkSize];

		// Move the ReadIndex.
		rd_ix = (rd_ix + size) % sh_mem->OSMemBlkSize;

		// Update ReadIndex on shared memory
		*((INT32U*)(sh_mem->MemBaseAddr) + SH_MEM_RD_IX_OFFSET) = rd_ix;

		*perr = OS_ERR_NONE;
	}
	else
		*perr = OS_ERR_SH_MEM_NO_ENOUGH_DATA;
}















void  write_sh_mem(OS_MEM* sh_mem, INT32U size, INT8U* buffer, INT8U *perr)
{
	INT32U	i, wr_ix, rd_ix;

	// Get indices from the shared memory.
	wr_ix = *((INT32U*)sh_mem->MemBaseAddr + SH_MEM_WR_IX_OFFSET);
	rd_ix = *((INT32U*)(sh_mem->MemBaseAddr) + SH_MEM_RD_IX_OFFSET);


	if(rd_ix <= sh_mem->wr_ix)			// If true, rd_ix reached the end of the memory and restarted from the beginning.
										// Or the FIFO is empty.
		rd_ix += sh_mem->OSMemBlkSize;	// Place rd_ix to the right of wr_ix as in an unbounded memory.

	if((wr_ix + size) < rd_ix)			/* Writer is allowed to write until WriteIndex == ReadIndex - 1
										 * cause WriteIndex == ReadIndex means that the FIFO is empty.
										 */
	{
		// Write the data.
		for(i=0; i<size; i++)
			((INT8U*)(sh_mem->DataBaseAddr))[(wr_ix + i) % sh_mem->OSMemBlkSize] = buffer[i];

		// Move the WriteIndex.
		sh_mem->wr_ix = (sh_mem->wr_ix + size) % sh_mem->OSMemBlkSize;

		// Update WriteIndex on shared memory
		*((INT32U*)(sh_mem->MemBaseAddr) + SH_MEM_WR_IX_OFFSET) = wr_ix;

		*perr = OS_ERR_NONE;
	}
	else
		*perr = OS_ERR_SH_MEM_NO_ENOUGH_ESPACE;
}



