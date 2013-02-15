/*
 * rt_ext_msg_mngr.c
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

#include <xil_io.h>
#include "lrt_prototypes.h"
#include "xparameters.h"
#include "xmbox_hw.h"
//#include "xgpio_l.h"


//--- Global variables ----//
static EXT_MSG_STRUCT 	ext_msg;							// To store an external message.
static INT32U 			tasks_stack[OS_DEFAULT_STACK_SIZE];	// Stack pour the tasks. Currently we need only one zone.

LRT_FIFO_HNDLE  		InputFIFOs[OS_NB_IN_FIFO];			// Table of input FIFO handles.
LRT_FIFO_HNDLE  		OutputFIFOs[OS_NB_OUT_FIFO];		// Table of output FIFO handles.



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
void blocking_fifo_write(INT32U mb_base_addr, INT32U data)
{
	// Wait until there is space in the FIFO
	while(XMbox_IsFullHw(mb_base_addr));

	XMbox_WriteMBox(mb_base_addr, data);
}



/**********************************************************************************************************
*                                              blocking_fifo_read
* Description: Reads a 32 bits block from the FIFO. Blocks if FIFO is empty.
*
* Arguments  : none
*
* Returns    : the read data.
**********************************************************************************************************/
INT32U blocking_fifo_read(INT32U mb_base_addr)
{
	// Wait until there is some data in the FIFO
	while(XMbox_IsEmptyHw(mb_base_addr));

	return XMbox_ReadMBox(mb_base_addr);
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

			break;

		case MSG_CREATE_SH_MEM:

			break;

		case MSG_START_TASK:
			OSRunning = true;
			OS_Sched();
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
* Description: This is the function waits for a external message is placed into	the mailbox or memory address.
*
* Arguments  : addr is the base address of the shared memory or mailbox.
*
* Returns    : none
*
*********************************************************************************************************
*/
void  wait_for_ext_msg(INT32U control_addr)
{

	INT32U *ptr = (INT32U*)&ext_msg;
	INT8U nb_reads = sizeof(EXT_MSG_STRUCT)/4;		// Number of 32bits words to read to get a complete msg.

#if CONTROL_COMM == 0	// Reads from a mailbox.
	while(nb_reads > 0)
	{
		nb_reads--;
		*ptr++ = blocking_fifo_read(control_addr);
	}
#else					// Reads from a memory location.
	while(nb_reads > 0)
	{
		nb_reads--;
		*ptr++ = *(INT32U*)control_addr;
		control_addr = (INT32U)((INT32U*)control_addr + 1);
	}
#endif


#if OS_DEBUG_EN > 0
	print("Local Runtime have got a message\n\r");
#endif
	local_rt_call();

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
//	INT32U data;
//
//	getfsl(data, 0);
//
//#if OS_DEBUG_EN > 0
//	print("Local Runtime 1 has read");
//	putnum(data);
//	print("from FSL\n\r");
//#endif

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
//	putfsl(data, 0);
//
//#if OS_DEBUG_EN > 0
//	print("Local Runtime 0 has put data into FSL\n\r");
//#endif
}














/*
*********************************************************************************************************
*                                              create_fifo_hndl
*
* Description: Creates a handle for a FIFO.
*
* Arguments  : addr is the base address of the FIFO.
* 			   size is the size of the FIFO in bytes.
* 			   dir is the direction of the FIFO (0 - input, 1 - output).
*
* Returns    : a pointer to a FIFO handle.
*
*********************************************************************************************************
*/
LRT_FIFO_HNDLE* create_fifo_hndl(INT32U addr, INT32U size, INT8U dir)
{
	LRT_FIFO_HNDLE* fifo_hndl;
	if(dir == 0)
		fifo_hndl = &InputFIFOs[0];
	else
		fifo_hndl = &OutputFIFOs[0];

	fifo_hndl->FifoBaseAddr = (INT32U*)addr;
	fifo_hndl->DataBaseAddr = (INT32U*)(addr + FIFO_DATA_OFFSET);
	fifo_hndl->Size = size;
	fifo_hndl->rd_ix = (INT32U*)(addr + FIFO_RD_IX_OFFSET);
	fifo_hndl->wr_ix = (INT32U*)(addr + FIFO_WR_IX_OFFSET);
	// Set indices' values to 0 if it is an output FIFO.
	*fifo_hndl->rd_ix &= (0xFFFFFFFE | ~dir);
	*fifo_hndl->wr_ix &= (0xFFFFFFFE | ~dir);

	return fifo_hndl;
}







/*
*********************************************************************************************************
*                                              read_input_fifo
*
* Description: Reads data (tokens) from an input FIFO.
*
* Arguments  : in_fifo_hndl is a pointer to the input FIFO's handle.
* 			   size is the amount of data to be read in bytes.
* 			   buffer is a pointer to a data block that will store the read data.
*			   perr will contain the error code : OS_ERR_NONE or OS_ERR_FIFO_NO_ENOUGH_DATA.
* Returns	 :
*
*********************************************************************************************************
*/
void  read_input_fifo(LRT_FIFO_HNDLE* in_fifo_hndl, INT32U size, INT8U* buffer, INT8U *perr)
{
	INT32U	i, wr_ix, rd_ix;

	// Get indices from the handle.
	wr_ix = *in_fifo_hndl->wr_ix;
	rd_ix = *in_fifo_hndl->rd_ix;

	if(wr_ix < rd_ix)					// If true, wr_ix reached the end of the memory and restarted from the beginning.
		wr_ix += in_fifo_hndl->Size;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

	if((rd_ix + size) <= wr_ix)			// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
	{
		// Read the data.
		for(i=0; i<size; i++)
			buffer[i] = ((INT8U*)(in_fifo_hndl->DataBaseAddr))[(rd_ix + i) % in_fifo_hndl->Size];

		// Move the temporary read index.
		rd_ix = (rd_ix + size) % in_fifo_hndl->Size;

		// Update the real read index's value.
		*in_fifo_hndl->rd_ix = rd_ix;

		*perr = OS_ERR_NONE;
	}
	else
		*perr = OS_ERR_FIFO_NO_ENOUGH_DATA;
}











/*
*********************************************************************************************************
*                                              write_output_fifo
*
* Description: Writes data (tokens) into an output FIFO.
*
* Arguments  : out_fifo_hndl is a pointer to output FIFO's handle.
* 			   size is the amount of data to be written in bytes.
* 			   buffer is a pointer to the data block to be copied.
* 			   perr will contain the error code : OS_ERR_NONE or OS_ERR_FIFO_NO_ENOUGH_ESPACE.
*
* Returns    :
*
*********************************************************************************************************
*/
void  write_output_fifo(LRT_FIFO_HNDLE* out_fifo_hndl, INT32U size, INT8U* buffer, INT8U *perr)
{
	INT32U	i, wr_ix, rd_ix;

	// Get indices from the handle.
	wr_ix = *out_fifo_hndl->wr_ix;
	rd_ix = *out_fifo_hndl->rd_ix;


	if(rd_ix <= wr_ix)					// If true, rd_ix reached the end of the memory and restarted from the beginning.
										// Or the FIFO is empty.
		rd_ix += out_fifo_hndl->Size;	// Place rd_ix to the right of wr_ix as in an unbounded memory.

	if((wr_ix + size) < rd_ix)			/* Writer is allowed to write until wr_ix == rd_ix - 1
										 * cause wr_ix == rd_ix means that the FIFO is empty.
										 */
	{
		// Write the data.
		for(i=0; i<size; i++)
			((INT8U*)(out_fifo_hndl->DataBaseAddr))[(wr_ix + i) % out_fifo_hndl->Size] = buffer[i];

		// Move the temporary write index.
		wr_ix = (wr_ix + size) % out_fifo_hndl->Size;

		// Update the real write index's value.
		*out_fifo_hndl->wr_ix = wr_ix;

		*perr = OS_ERR_NONE;
	}
	else
		*perr = OS_ERR_FIFO_NO_ENOUGH_ESPACE;
}









/*
*********************************************************************************************************
*                                              blocking_read_input_fifo
*
* Description: Reads data (tokens) from an input FIFO. Blocks until there is enough data.
*
* Arguments  : in_fifo_hndl is a pointer to the input FIFO's handle.
* 			   size is the amount of data to be read in bytes.
* 			   buffer is a pointer to a data block that will store the read data.
* 			   perr will contain the error code : OS_ERR_NONE.
*
* Returns    :
*
*********************************************************************************************************
*/
void  blocking_read_input_fifo(LRT_FIFO_HNDLE* in_fifo_hndl, INT32U size, INT8U* buffer, INT8U *perr)
{
	INT32U	i, *wr_ix, *rd_ix, temp_wr_ix;

	// Get indices from the handle.
	wr_ix = in_fifo_hndl->wr_ix;
	rd_ix = in_fifo_hndl->rd_ix;

	while(TRUE)
	{
		temp_wr_ix = *wr_ix;
		if(*wr_ix < *rd_ix)								// If true, wr_ix reached the end of the memory and restarted from the beginning.
			temp_wr_ix = *wr_ix + in_fifo_hndl->Size;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

		if(((*rd_ix) + size) <= temp_wr_ix)				// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
		{
			// Read the data.
			for(i=0; i<size; i++)
				buffer[i] = ((INT8U*)(in_fifo_hndl->DataBaseAddr))[(*rd_ix + i) % in_fifo_hndl->Size];

			// Update the read index.
			*rd_ix = (*rd_ix + size) % in_fifo_hndl->Size;

			*perr = OS_ERR_NONE;

			return;
		}
	}
}


