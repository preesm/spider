/*
 * rt_ext_msg_mngr.c
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

//#include <xil_io.h>
#include <string.h>
#include "lrt_prototypes.h"
#include "xparameters.h"
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
*                                              mb_write
* Description: Writes a 32 bits block into the mailbox if not full.
*
* Arguments  :
* 			   data - data to be copied.
*
* Returns    : none
**********************************************************************************************************/
void mb_write(INT32U mb_base_addr, INT32U data)
{
	if(!XMbox_IsFullHw(mb_base_addr))
		XMbox_WriteMBox(mb_base_addr, data);
}


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
*                                              blocking_mb_block_write
* Description: Writes a block of data into a mailbox. Blocks if it is full.
*
* Arguments  : mb_base_addr is the base address of the mailbox.
* 			   BufferPtr is the source buffer.
* 			   RequestedBytes is the size of the block to be written.
* Returns    : none
**********************************************************************************************************/
void blocking_mb_block_write(INT32U mb_base_addr, INT32U* SrcBufferPtr, INT32U RequestedBytes)
{
	INT32U	NumBytes = 0;
	do {
		blocking_mb_write(mb_base_addr, *SrcBufferPtr++);
		NumBytes += 4;
	} while (NumBytes < RequestedBytes);
}


/**********************************************************************************************************
*                                              mb_read
* Description: Reads a 32 bits block from the mailbox if not empty.
*
* Arguments  : none
*
* Returns    : the read data or zero if the mb is empty.
**********************************************************************************************************/
INT32U mb_read(INT32U mb_base_addr)
{
	if(!XMbox_IsEmptyHw(mb_base_addr))
		return XMbox_ReadMBox(mb_base_addr);
	else
		return 0;
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


/**********************************************************************************************************
*                                              blocking_mb_block_read
* Description: Reads requested bytes from the mailbox into the buffer pointed to by the provided pointer.
* 			The number of bytes must be a multiple of 4 (bytes).
*
* Arguments  : mb_base_addr is the mailbox's base address.
* 			   BufferPtr is the destination buffer.
* 			   RequestedBytes is the size of the block to be read.
*
* Returns    :
**********************************************************************************************************/
void blocking_mb_block_read(INT32U mb_base_addr, INT32U *BufferPtr, INT32U RequestedBytes)
{
	INT32U	NumBytes = 0;
	do {
		/*
		 * Read the Mailbox until empty or the length
		 * requested is satisfied
		 */
		*BufferPtr++ = blocking_mb_read(mb_base_addr);
		NumBytes += 4;
	} while (NumBytes != RequestedBytes);
}




/*
*********************************************************************************************************
*                                              send_ext_msg
*
* Description: Sends a message to a local runtime through the corresponding mailbox.
*
* Arguments  :	addr is the base address of the targeted mailbox.
* 				msg_type is the message type.
* 				msg	is the message to be sent.
*
* Returns    : none
*
*********************************************************************************************************
*/
void  send_ext_msg(INT32U addr, INT32U msg_type, void* msg)
{
	INT32U	msg_size = 0;

	// Sending the type of message as the first data word.
	blocking_mb_write(addr, msg_type);

	// Determining the size of the rest of data.
	switch (msg_type) {
		case MSG_CURR_VERTEX_ID:
			msg_size = sizeof(INT32U);
		default:
			break;
	}

	// Sending the rest of data.
	if(msg_size > 0) blocking_mb_block_write(addr, msg, msg_size);
}


/*
*********************************************************************************************************
*                                              wait_for_ext_typed_msg
*
* Description: Waits for an external message is placed into the mailbox.
*
* Arguments  : addr is the base address of the mailbox.
*
* Returns    : none
*
*********************************************************************************************************
*/
void  wait_ext_msg()
{
#if CONTROL_COMM == 0	// Reads from a mailbox.

	INT8U	err;
	INT32U 	msg_type;

	MSG_CREATE_TASK_STRUCT msg_create_task;
	MSG_CREATE_FIFO_STRUCT msg_create_fifo;
	INT32U				   msg_stop_task, curr_vertex_id;

	msg_type = mb_read(control_addr);	// Reads the first incoming word i.e. the message type.

	switch(msg_type)
	{
	// TODO: Pass to LrtTaskCreate a pointer to the msg_create_task object instead of all these arguments.
	case MSG_CREATE_TASK:
		blocking_mb_block_read(control_addr, (INT32U*)&msg_create_task, sizeof(MSG_CREATE_TASK_STRUCT));
		LrtTaskCreate(&tasks_stack[OS_DEFAULT_STACK_SIZE - 1],	// the top of the stack (highest address).
					  tasks_stack,								// the bottom of the stack (lower address).
					  OS_DEFAULT_STACK_SIZE,					// the size of the stack.
					  &msg_create_task);
		break;

	case MSG_STOP_TASK:
		blocking_mb_block_read(control_addr, &msg_stop_task, sizeof(INT32U));
//		err = OSTaskDel(msg_stop_task, &curr_vertex_id);
		curr_vertex_id = 1;
		send_ext_msg(control_addr, MSG_CURR_VERTEX_ID, &curr_vertex_id);
		if(!OSTCBCur) lrt_running = false;
		break;

	case MSG_CREATE_FIFO:
		blocking_mb_block_read(control_addr, (INT32U*)&msg_create_fifo, sizeof(MSG_CREATE_FIFO_STRUCT));
		create_fifo_hndl(&msg_create_fifo, &err);
		break;

	case MSG_CLEAR_FIFO:
		break;

	case MSG_START_SCHED:
		lrt_running = true;
		break;

	default:
		break;
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
//	print("Local Runtime have got a message\n\r");
#endif

	if(lrt_running) OSStartCur();
}





//
//
///*
//*********************************************************************************************************
//*                                              get_ext_msg
//*
//* Description: Checks the mailbox for new messages.
//*
//* Arguments  :
//*
//* Returns    : none
//*
//*********************************************************************************************************
//*/
//void  get_ext_msg()
//{
//	// Returns if the mailbox is empty.
//	while(!XMbox_IsEmptyHw(control_addr))
//	{
//		INT32U *ptr = (INT32U*)&ext_msg;
//
//		// nb_reads stores the message's size in number of 32bits words.
//		INT32U nb_reads = sizeof(LRT_MSG)/4;
//
//		while(nb_reads > 0)
//		{
//			nb_reads--;
//			*ptr++ = blocking_mb_read(control_addr);
//		}
//	#if OS_DEBUG_EN > 0
//		print("Local Runtime have got a message\n\r");
//	#endif
//		local_rt_call();
//	}
//}








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
//LRT_FIFO_HNDLE* create_fifo_hndl(INT16U id, INT32U size, INT8U dir, INT8U* perr)
LRT_FIFO_HNDLE* create_fifo_hndl(MSG_CREATE_FIFO_STRUCT *msg_create_fifo, INT8U* perr)
{
	if(msg_create_fifo->id >= OS_NB_FIFO )
	{
		*perr = OS_ERR_FIFO_INVALID_ID;
		return (LRT_FIFO_HNDLE*)0;
	}
	else
	{
		LRT_FIFO_HNDLE* fifo_hndl = &FIFO_tbl[msg_create_fifo->id];

		// Determining the FIFO's base address. TODO: Authorize different size FIFOs.
		fifo_hndl->FifoBaseAddr = msg_create_fifo->mem_block + msg_create_fifo->block_ix * (msg_create_fifo->size + FIFO_DATA_OFFSET);
		fifo_hndl->DataBaseAddr = fifo_hndl->FifoBaseAddr + FIFO_DATA_OFFSET;
		fifo_hndl->Size = msg_create_fifo->size;
		fifo_hndl->Direction = msg_create_fifo->direction;
		fifo_hndl->Status = FIFO_STAT_INIT;
		fifo_hndl->rd_ix = fifo_hndl->FifoBaseAddr + FIFO_RD_IX_OFFSET;
		fifo_hndl->wr_ix = fifo_hndl->FifoBaseAddr + FIFO_WR_IX_OFFSET;
		if(msg_create_fifo->init)
		{
			*((INT32U*)(fifo_hndl->rd_ix)) = 0;
			*((INT32U*)(fifo_hndl->wr_ix)) = 0;
		}

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
//		fifo_hndl->Status = FIFO_STAT_EMPTY;
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
	if(fifo_id >= OS_NB_FIFO)
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
*                                              check_input_fifo
*
* Description: Checks whether a data block can be read from an input FIFO.
*
* Arguments  : in_fifo_hndl is a pointer to the input FIFO's handle.
* 			   size is the amount of data to be read in bytes.
*
* Returns	 : true if there is enough data.
*
*********************************************************************************************************
*/
BOOLEAN  check_input_fifo(LRT_FIFO_HNDLE* in_fifo_hndl, INT32U size)
{
	volatile INT32U *wr_ix, *rd_ix, temp_wr_ix;

	// Get indices from the handle.
	wr_ix = (volatile INT32U*)in_fifo_hndl->wr_ix;
	rd_ix = (volatile INT32U*)in_fifo_hndl->rd_ix;

	temp_wr_ix = *wr_ix;
	if(*wr_ix < *rd_ix)								// If true, wr_ix reached the end of the memory and restarted from the beginning.
		temp_wr_ix = *wr_ix + in_fifo_hndl->Size;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

	return (((*rd_ix) + size) <= temp_wr_ix);		// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
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
	volatile INT32U *wr_ix, *rd_ix, temp_wr_ix;

	// Get indices from the handle.
	wr_ix = (volatile INT32U*)in_fifo_hndl->wr_ix;
	rd_ix = (volatile INT32U*)in_fifo_hndl->rd_ix;

	temp_wr_ix = *wr_ix;
	if(*wr_ix < *rd_ix)								// If true, wr_ix reached the end of the memory and restarted from the beginning.
		temp_wr_ix = *wr_ix + in_fifo_hndl->Size;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

	if(((*rd_ix) + size) <= temp_wr_ix)				// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
	{
		// Read the data.
//			for(i=0; i<size/4; i++)
//				buffer[i] = ((INT8U*)(in_fifo_hndl->DataBaseAddr))[(*rd_ix + i) % in_fifo_hndl->Size];

		if(*rd_ix + size > in_fifo_hndl->Size){
			memcpy(buffer, in_fifo_hndl->DataBaseAddr + *rd_ix, (in_fifo_hndl->Size - *rd_ix) * sizeof(INT8U));
//			OS_MemCopy(buffer, (INT8U*)in_fifo_hndl->DataBaseAddr + *rd_ix, (in_fifo_hndl->Size - *rd_ix) * sizeof(INT8U));
			memcpy(buffer + in_fifo_hndl->Size - *rd_ix, in_fifo_hndl->DataBaseAddr, (size - in_fifo_hndl->Size + *rd_ix) * sizeof(INT8U));
//			OS_MemCopy(buffer + in_fifo_hndl->Size - *rd_ix, (INT8U*)in_fifo_hndl->DataBaseAddr, (size - in_fifo_hndl->Size + *rd_ix) * sizeof(INT8U));
		}else{
			memcpy(buffer, in_fifo_hndl->DataBaseAddr + *rd_ix, size * sizeof(INT8U));
//			OS_MemCopy(buffer, (INT8U*)in_fifo_hndl->DataBaseAddr + *rd_ix, size * sizeof(INT8U));
		}

		// Update the read index.
		*rd_ix = (*rd_ix + size) % in_fifo_hndl->Size;

		// Updating IFFO' state.
//		if (*rd_ix == *wr_ix) in_fifo_hndl->Status = FIFO_STAT_EMPTY;

		*perr = OS_ERR_NONE;
	}
	else
		*perr = OS_ERR_FIFO_NO_ENOUGH_DATA;
}





/*
*********************************************************************************************************
*                                              check_output_fifo
*
* Description: Checks whether a data block can be written into an output FIFO.
*
* Arguments  : out_fifo_hndl is a pointer to output FIFO's handle.
* 			   size is the amount of data to be written in bytes.
*
* Returns    : true if there is enough space in the FIFO.
*
*********************************************************************************************************
*/
BOOLEAN  check_output_fifo(LRT_FIFO_HNDLE* out_fifo_hndl, INT32U size)
{
	volatile INT32U	*wr_ix, *rd_ix, temp_rd_ix;

	// Get indices from the handle.
	wr_ix = (volatile INT32U*)out_fifo_hndl->wr_ix;
	rd_ix = (volatile INT32U*)out_fifo_hndl->rd_ix;

	temp_rd_ix = *rd_ix;
	if(*rd_ix <= *wr_ix)							// If true, rd_ix reached the end of the memory and restarted from the beginning.
													// Or the FIFO is empty.
		temp_rd_ix = *rd_ix + out_fifo_hndl->Size;	// Place rd_ix to the right of wr_ix as in an unbounded memory.

	return((*wr_ix + size) < temp_rd_ix);			/* Writer is allowed to write until wr_ix == rd_ix - 1
										 	 	 	 * cause wr_ix == rd_ix means that the FIFO is empty.
										 	 	 	 */
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
	volatile INT32U	*wr_ix, *rd_ix, temp_rd_ix;

	// Get indices from the handle.
	wr_ix = (volatile INT32U*)out_fifo_hndl->wr_ix;
	rd_ix = (volatile INT32U*)out_fifo_hndl->rd_ix;

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
			memcpy(out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, (out_fifo_hndl->Size - *wr_ix)*sizeof(INT8U));
			memcpy(out_fifo_hndl->DataBaseAddr, buffer + out_fifo_hndl->Size - *wr_ix, (size - out_fifo_hndl->Size + *wr_ix) * sizeof(INT8U));
//			OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, (out_fifo_hndl->Size - *wr_ix)*sizeof(INT8U));
//			OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr, buffer + out_fifo_hndl->Size - *wr_ix, (size - out_fifo_hndl->Size + *wr_ix) * sizeof(INT8U));
		}
		else
		{
			memcpy(out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, size * sizeof(INT8U));
//			OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, size * sizeof(INT8U));
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
	volatile INT32U	*wr_ix, *rd_ix, temp_wr_ix;
//	INT32U i;

	// Get indices from the handle.
	wr_ix = (volatile INT32U*)in_fifo_hndl->wr_ix;
	rd_ix = (volatile INT32U*)in_fifo_hndl->rd_ix;

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
				memcpy(buffer, in_fifo_hndl->DataBaseAddr + *rd_ix, (in_fifo_hndl->Size - *rd_ix) * sizeof(INT8U));
//				OS_MemCopy(buffer, (INT8U*)in_fifo_hndl->DataBaseAddr + *rd_ix, (in_fifo_hndl->Size - *rd_ix) * sizeof(INT8U));
				memcpy(buffer + in_fifo_hndl->Size - *rd_ix, in_fifo_hndl->DataBaseAddr, (size - in_fifo_hndl->Size + *rd_ix) * sizeof(INT8U));
//				OS_MemCopy(buffer + in_fifo_hndl->Size - *rd_ix, (INT8U*)in_fifo_hndl->DataBaseAddr, (size - in_fifo_hndl->Size + *rd_ix) * sizeof(INT8U));
			}else{
				memcpy(buffer, in_fifo_hndl->DataBaseAddr + *rd_ix, size * sizeof(INT8U));
//				OS_MemCopy(buffer, (INT8U*)in_fifo_hndl->DataBaseAddr + *rd_ix, size * sizeof(INT8U));
			}

			// Update the read index.
			*rd_ix = (*rd_ix + size) % in_fifo_hndl->Size;

			// Updating IFFO' state.
//			if (*rd_ix == *wr_ix) in_fifo_hndl->Status = FIFO_STAT_EMPTY;

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
	volatile INT32U	*wr_ix, *rd_ix, temp_rd_ix;
//	INT32U i;

	// Get indices from the handle.
	wr_ix = (volatile INT32U*)out_fifo_hndl->wr_ix;
	rd_ix = (volatile INT32U*)out_fifo_hndl->rd_ix;

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
				memcpy(out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, (out_fifo_hndl->Size - *wr_ix)*sizeof(INT8U));
//				OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, (out_fifo_hndl->Size - *wr_ix)*sizeof(INT8U));
				memcpy(out_fifo_hndl->DataBaseAddr, buffer + out_fifo_hndl->Size - *wr_ix, (size - out_fifo_hndl->Size + *wr_ix) * sizeof(INT8U));
//				OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr, buffer + out_fifo_hndl->Size - *wr_ix, (size - out_fifo_hndl->Size + *wr_ix) * sizeof(INT8U));
			}
			else
			{
				memcpy((out_fifo_hndl->DataBaseAddr + *wr_ix), buffer, size * sizeof(INT8U));
//				OS_MemCopy((INT8U*)out_fifo_hndl->DataBaseAddr + *wr_ix, buffer, size * sizeof(INT8U));
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






