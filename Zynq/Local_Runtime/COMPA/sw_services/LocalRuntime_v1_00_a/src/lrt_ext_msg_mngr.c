/*
 * rt_ext_msg_mngr.c
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

//#include <xil_io.h>
//#include <string.h>
#include "lrt_prototypes.h"
#include "xparameters.h"
#include "xmbox_hw.h"
//#include "xgpio_l.h"


//--- Global variables ----//
static LRT_MSG		 	ext_msg;							// To store an external message.
static INT32U 			tasks_stack[OS_DEFAULT_STACK_SIZE];	// Stack pour the tasks. Currently we need only one zone.
#if CONTROL_COMM == 1
LRT_FIFO_HNDLE 			*cntrl_fifo;						// Pointer to an input FIFO for control messages.
#endif

INT32U					control_addr;						// Address for receiving external messages.

//LRT_FIFO_HNDLE  		InputFIFOs[OS_NB_IN_FIFO];			// Table of input FIFO handles.
//LRT_FIFO_HNDLE  		OutputFIFOs[OS_NB_OUT_FIFO];		// Table of output FIFO handles.
LRT_FIFO_HNDLE  		FIFO_tbl[OS_NB_FIFO];				// Table of FIFO handles.
//INT32U					sh_mem_pointer;						// Current position in the FIFO shared memory.


//extern mtapi_task_id_t action_cntr;					// Action's counter.
FUNCTION_TYPE functions_tbl[NB_LOCAL_FUNCTIONS];

/**********************************************************************************************************
*                                              blocking_mb_write
* Description: Writes a 32 bits block into the mailbox. Blocks if the mailbox is full.
*
* Arguments  :
* 			   data - data to be copied.
*
* Returns    : none
**********************************************************************************************************/
void blocking_mb_write(INT32U mb_base_addr, INT32U data)
{
	// Wait until there is space in the mailbox.
	while(XMbox_IsFullHw(mb_base_addr));

	XMbox_WriteMBox(mb_base_addr, data);
}



/**********************************************************************************************************
*                                              blocking_mb_read
* Description: Reads a 32 bits block from the mailbox. Blocks if the mailbox is empty.
*
* Arguments  : none
*
* Returns    : the read data.
**********************************************************************************************************/
INT32U blocking_mb_read(INT32U mb_base_addr)
{
	// Wait until there is some data in the mailbox.
	while(XMbox_IsEmptyHw(mb_base_addr));

	return XMbox_ReadMBox(mb_base_addr);
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                              local_rt_call
*
* Description: Decodes an external message.
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
	INT8U error;

    switch (ext_msg.msg_type)
    {
		case MSG_CREATE_TASK:
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
					ext_msg.nb_fifo_in,							// Number of input FIFOs.
					ext_msg.nb_fifo_out,						// Number of output FIFOs.
					(INT16U*)ext_msg.fifo_in,					// Array of input FIFO ids.
					(INT16U*)ext_msg.fifo_out,					// Array of output FIFO ids.
					(void*)0,									// a pointer to a structure with more data.
					0);											// other options.

			break;

		case MSG_CREATE_FIFO:
			create_fifo_hndl(ext_msg.fifo_id, FIFO_SIZE, FIFO_IN_DIR, &error);
			break;

		case MSG_START_SCHED:
			OSRunning = true;
			OS_Sched();
			break;

		case MSG_STOP_TASK:
			OSTaskDel(ext_msg.task_id);
			break;

		case MSG_CLEAR_FIFO:
			clear_fifo(ext_msg.fifo_id, &error);
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
* Description: Waits for an external message is placed into the mailbox.
*
* Arguments  : addr is the base address of the mailbox.
*
* Returns    : none
*
*********************************************************************************************************
*/
void  wait_for_ext_msg()
{
#if CONTROL_COMM == 0	// Reads from a mailbox.
	INT32U *ptr = (INT32U*)&ext_msg;

	// nb_reads stores the message's size in number of 32bits words.
	INT32U nb_reads = sizeof(LRT_MSG)/4;

	while(nb_reads > 0)
	{
		nb_reads--;
		*ptr++ = blocking_mb_read(control_addr);
	}
#else					// Reads from a FIFO.
	blocking_read_input_fifo(cntrl_fifo, sizeof(EXT_MSG_STRUCT), (INT8U*)&ext_msg, (INT8U*)0);
//	while(nb_reads > 0)
//	{
//		nb_reads--;
//		*ptr++ = *(INT32U*)control_addr;
//		control_addr = (INT32U)((INT32U*)control_addr + 1);
//	}
#endif


#if OS_DEBUG_EN > 0
	print("Local Runtime have got a message\n\r");
#endif
	local_rt_call();

}







/*
*********************************************************************************************************
*                                              get_ext_msg
*
* Description: Checks the mailbox for new messages.
*
* Arguments  :
*
* Returns    : none
*
*********************************************************************************************************
*/
void  get_ext_msg()
{
	// Returns if the mailbox is empty.
	while(!XMbox_IsEmptyHw(control_addr))
	{
		INT32U *ptr = (INT32U*)&ext_msg;

		// nb_reads stores the message's size in number of 32bits words.
		INT32U nb_reads = sizeof(LRT_MSG)/4;

		while(nb_reads > 0)
		{
			nb_reads--;
			*ptr++ = blocking_mb_read(control_addr);
		}
	#if OS_DEBUG_EN > 0
		print("Local Runtime have got a message\n\r");
	#endif
		local_rt_call();
	}
}








/*
*********************************************************************************************************
*                                              create_fifo_hndl
*
* Description: Creates a handle for a FIFO.
*
* Arguments  : id is the index on the table of FIFOs.
* 			   size is the size of the FIFO in bytes.
* 			   dir is the direction of the FIFO (0 - input, 1 - output).
* 			   perr will contain one of these error codes : OS_ERROR_NONE, OS_ERR_FIFO_INVALID_ID.
*
* Returns    : a pointer to a FIFO handle.
*
*********************************************************************************************************
*/
LRT_FIFO_HNDLE* create_fifo_hndl(INT16U id, INT32U size, INT8U dir, INT8U* perr)
{
	static INT32U sh_mem_pointer = XPAR_BRAM_0_BASEADDR; // Current position in the FIFO shared memory.
	if(id >= OS_NB_FIFO - 1)
	{
		*perr = OS_ERR_FIFO_INVALID_ID;
		return (LRT_FIFO_HNDLE*)0;
	}
	else
	{
		LRT_FIFO_HNDLE* fifo_hndl = &FIFO_tbl[id];

		fifo_hndl->FifoBaseAddr = sh_mem_pointer;
		fifo_hndl->DataBaseAddr = sh_mem_pointer + FIFO_DATA_OFFSET;
		fifo_hndl->Size = size;
		fifo_hndl->Direction = dir;
		fifo_hndl->Status = FIFO_STAT_INIT;
		fifo_hndl->rd_ix = sh_mem_pointer + FIFO_RD_IX_OFFSET;
		fifo_hndl->wr_ix = sh_mem_pointer + FIFO_WR_IX_OFFSET;
		*((INT32U*)(fifo_hndl->rd_ix)) = 0;
		*((INT32U*)(fifo_hndl->wr_ix)) = 0;

		sh_mem_pointer = fifo_hndl->DataBaseAddr + size;
		*perr = OS_ERR_NONE;
		return fifo_hndl;
	}
}









/*
*********************************************************************************************************
*                                              clear_fifo
*
* Description: Clears the contents of a FIFO by reinitializing its indices .
*
* Arguments  : id is the index of the FIFO.
* 			   perr will contain one of these error codes : OS_ERROR_NONE, OS_ERR_FIFO_INVALID_ID.
*
* Returns    :
*
*********************************************************************************************************
*/
void clear_fifo(INT16U id, INT8U* perr)
{
	INT8U error;
	LRT_FIFO_HNDLE* fifo_hndl = get_fifo_hndl(id, &error);

	if(error == OS_ERR_NONE)
	{
		*((INT32U*)(fifo_hndl->rd_ix)) = 0;
		*((INT32U*)(fifo_hndl->wr_ix)) = 0;
	}
	*perr = error;
}






/*
*********************************************************************************************************
*                                              get_fifo_hndl
*
* Description: Gets the pointer to a FIFO handle.
*
* Arguments  : fifo_id
* 			   perr will contain the error code : OS_ERR_NONE or OS_ERR_FIFO_NOT_FOUND
*
*
* Returns    : A pointer to the FIFO or NULL if it doesn't found it.
*
*********************************************************************************************************
*/
LRT_FIFO_HNDLE* get_fifo_hndl(INT16U fifo_id, INT8U* perr)
{
	if(fifo_id >= OS_NB_FIFO - 1)
	{
		*perr = OS_ERR_FIFO_INVALID_ID;
		return (LRT_FIFO_HNDLE*)0;
	}
	else
	{
		LRT_FIFO_HNDLE* fifo_hndl = &FIFO_tbl[fifo_id];
		if(fifo_hndl->Status != FIFO_STAT_INIT)
		{
			*perr = OS_ERR_FIFO_NOT_INITIALIZED;
			return (LRT_FIFO_HNDLE*)0;
		}
		else
		{
			*perr = OS_ERR_NONE;
			return fifo_hndl;
		}

	}
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
	INT32U *wr_ix, *rd_ix, temp_wr_ix;

	// Get indices from the handle.
	wr_ix = (INT32U*)in_fifo_hndl->wr_ix;
	rd_ix = (INT32U*)in_fifo_hndl->rd_ix;

	temp_wr_ix = *wr_ix;
	if(*wr_ix < *rd_ix)								// If true, wr_ix reached the end of the memory and restarted from the beginning.
		temp_wr_ix = *wr_ix + in_fifo_hndl->Size;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

	if(((*rd_ix) + size) <= temp_wr_ix)				// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
	{
		// Read the data.
//			for(i=0; i<size/4; i++)
//				buffer[i] = ((INT8U*)(in_fifo_hndl->DataBaseAddr))[(*rd_ix + i) % in_fifo_hndl->Size];

		if(*rd_ix + size > in_fifo_hndl->Size){
//			memcpy(buffer, (INT32U*)in_fifo_hndl->DataBaseAddr + *rd_ix, (in_fifo_hndl->Size - *rd_ix) * sizeof(INT8U));
			OS_MemCopy(buffer, (INT8U*)in_fifo_hndl->DataBaseAddr + *rd_ix, (in_fifo_hndl->Size - *rd_ix) * sizeof(INT8U));
//			memcpy(buffer + in_fifo_hndl->Size - *rd_ix, (INT32U*)in_fifo_hndl->DataBaseAddr, (size - in_fifo_hndl->Size + *rd_ix) * sizeof(INT8U));
			OS_MemCopy(buffer + in_fifo_hndl->Size - *rd_ix, (INT8U*)in_fifo_hndl->DataBaseAddr, (size - in_fifo_hndl->Size + *rd_ix) * sizeof(INT8U));
		}else{
//			memcpy(buffer, (INT32U*)in_fifo_hndl->DataBaseAddr + *rd_ix, size * sizeof(INT8U));
			OS_MemCopy(buffer, (INT8U*)in_fifo_hndl->DataBaseAddr + *rd_ix, size * sizeof(INT8U));
		}

		// Update the read index.
		*rd_ix = (*rd_ix + size) % in_fifo_hndl->Size;

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
	INT32U	*wr_ix, *rd_ix, temp_rd_ix;

	// Get indices from the handle.
	wr_ix = (INT32U*)out_fifo_hndl->wr_ix;
	rd_ix = (INT32U*)out_fifo_hndl->rd_ix;

	temp_rd_ix = *rd_ix;
	if(*rd_ix <= *wr_ix)							// If true, rd_ix reached the end of the memory and restarted from the beginning.
													// Or the FIFO is empty.
		temp_rd_ix = *rd_ix + out_fifo_hndl->Size;	// Place rd_ix to the right of wr_ix as in an unbounded memory.

	if((*wr_ix + size) < temp_rd_ix)				/* Writer is allowed to write until wr_ix == rd_ix - 1
										 	 	 	 * cause wr_ix == rd_ix means that the FIFO is empty.
										 	 	 	 */
	{
		// Write the data.
//			for(i=0; i<size/4; i++)
//				((INT32U*)(out_fifo_hndl->DataBaseAddr))[(*wr_ix + i) % out_fifo_hndl->Size] = ((INT32U*)buffer)[i];

//			memcpy(out_fifo_hndl->DataBaseAddr+*wr_ix, buffer, size);
		if(*wr_ix + size > out_fifo_hndl->Size)
		{
//			memcpy((INT32U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, (out_fifo_hndl->Size - *wr_ix)*sizeof(INT8U));
//			memcpy((INT32U*)out_fifo_hndl->DataBaseAddr, buffer + out_fifo_hndl->Size - *wr_ix, (size - out_fifo_hndl->Size + *wr_ix) * sizeof(INT8U));
			OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, (out_fifo_hndl->Size - *wr_ix)*sizeof(INT8U));
			OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr, buffer + out_fifo_hndl->Size - *wr_ix, (size - out_fifo_hndl->Size + *wr_ix) * sizeof(INT8U));
		}
		else
		{
//			memcpy((INT32U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, size * sizeof(INT8U));
			OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, size * sizeof(INT8U));
		}

		// Update write index.
		*wr_ix = (*wr_ix + size) % out_fifo_hndl->Size;

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
	INT32U	*wr_ix, *rd_ix, temp_wr_ix;
//	INT32U i;

	// Get indices from the handle.
	wr_ix = (INT32U*)in_fifo_hndl->wr_ix;
	rd_ix = (INT32U*)in_fifo_hndl->rd_ix;

	while(TRUE)
	{
		temp_wr_ix = *wr_ix;
		if(*wr_ix < *rd_ix)								// If true, wr_ix reached the end of the memory and restarted from the beginning.
			temp_wr_ix = *wr_ix + in_fifo_hndl->Size;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

		if(((*rd_ix) + size) <= temp_wr_ix)				// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
		{
			// Read the data.
//			for(i=0; i<size/4; i++)
//				buffer[i] = ((INT8U*)(in_fifo_hndl->DataBaseAddr))[(*rd_ix + i) % in_fifo_hndl->Size];

			if(*rd_ix + size > in_fifo_hndl->Size){
//				memcpy(buffer, (INT32U*)in_fifo_hndl->DataBaseAddr + *rd_ix, (in_fifo_hndl->Size - *rd_ix) * sizeof(INT8U));
				OS_MemCopy(buffer, (INT8U*)in_fifo_hndl->DataBaseAddr + *rd_ix, (in_fifo_hndl->Size - *rd_ix) * sizeof(INT8U));
//				memcpy(buffer + in_fifo_hndl->Size - *rd_ix, (INT32U*)in_fifo_hndl->DataBaseAddr, (size - in_fifo_hndl->Size + *rd_ix) * sizeof(INT8U));
				OS_MemCopy(buffer + in_fifo_hndl->Size - *rd_ix, (INT8U*)in_fifo_hndl->DataBaseAddr, (size - in_fifo_hndl->Size + *rd_ix) * sizeof(INT8U));
			}else{
//				memcpy(buffer, (INT32U*)in_fifo_hndl->DataBaseAddr + *rd_ix, size * sizeof(INT8U));
				OS_MemCopy(buffer, (INT8U*)in_fifo_hndl->DataBaseAddr + *rd_ix, size * sizeof(INT8U));
			}

			// Update the read index.
			*rd_ix = (*rd_ix + size) % in_fifo_hndl->Size;

			*perr = OS_ERR_NONE;

			return;
		}
	}
}









/*
*********************************************************************************************************
*                                              blocking_write_output_fifo
*
* Description: Writes data (tokens) to an output FIFO. Blocks until there is enough space.
*
* Arguments  : out_fifo_hndl is a pointer to the output FIFO's handle.
* 			   size is the amount of data to be written in bytes.
* 			   buffer is a pointer to a data block containing the data.
* 			   perr will contain the error code : OS_ERR_NONE.
*
* Returns    :
*
*********************************************************************************************************
*/
void  blocking_write_output_fifo(LRT_FIFO_HNDLE* out_fifo_hndl, INT32U size, INT8U* buffer, INT8U *perr)
{
	INT32U	*wr_ix, *rd_ix, temp_rd_ix;
//	INT32U i;

	// Get indices from the handle.
	wr_ix = (INT32U*)out_fifo_hndl->wr_ix;
	rd_ix = (INT32U*)out_fifo_hndl->rd_ix;

	while(TRUE)
	{
		temp_rd_ix = *rd_ix;
		if(*rd_ix <= *wr_ix)							// If true, rd_ix reached the end of the memory and restarted from the beginning.
														// Or the FIFO is empty.
			temp_rd_ix = *rd_ix + out_fifo_hndl->Size;	// Place rd_ix to the right of wr_ix as in an unbounded memory.

		if((*wr_ix + size) < temp_rd_ix)				/* Writer is allowed to write until wr_ix == rd_ix - 1
											 	 	 	 * cause wr_ix == rd_ix means that the FIFO is empty.
											 	 	 	 */
		{
			// Write the data.
//			for(i=0; i<size/4; i++)
//				((INT32U*)(out_fifo_hndl->DataBaseAddr))[(*wr_ix + i) % out_fifo_hndl->Size] = ((INT32U*)buffer)[i];

			if(*wr_ix + size > out_fifo_hndl->Size)
			{
//				memcpy((INT32U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, (out_fifo_hndl->Size - *wr_ix)*sizeof(INT8U));
				OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, (out_fifo_hndl->Size - *wr_ix)*sizeof(INT8U));
//				memcpy((INT32U*)out_fifo_hndl->DataBaseAddr, buffer + out_fifo_hndl->Size - *wr_ix, (size - out_fifo_hndl->Size + *wr_ix) * sizeof(INT8U));
				OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr, buffer + out_fifo_hndl->Size - *wr_ix, (size - out_fifo_hndl->Size + *wr_ix) * sizeof(INT8U));
			}
			else
			{
//				memcpy((INT8U*)(out_fifo_hndl->DataBaseAddr + *wr_ix), buffer, size * sizeof(INT8U));
				OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, size * sizeof(INT8U));
			}

			// Update write index.
			*wr_ix = (*wr_ix + size) % out_fifo_hndl->Size;

			*perr = OS_ERR_NONE;

			return;
		}
	}
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






