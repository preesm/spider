/*
 * rt_ext_msg_mngr.c
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

#include <xil_io.h>
#include <xil_types.h>
#include <string.h>
#include <xmbox_hw.h>
#include "grt_cfg.h"
#include "xparameters.h"
#include "grt_definitions.h"






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


/*****************************************************************************/
/**										flush_mailboxes
*
* Resets the mailboxes.
*
* @param	mb_base_addr is mailbox's base address.
*
* @note		Data from read FIFO is thrown away.
*
******************************************************************************/
void flush_mailboxes(u32 mb_base_addr)
{
	do {
		XMbox_ReadMBox(mb_base_addr);
	} while (!(XMbox_IsEmptyHw(mb_base_addr)));
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
		case MSG_CREATE_FIFO:
			msg_size = sizeof(MSG_CREATE_FIFO_STRUCT);
			break;

		case MSG_CREATE_TASK:
			msg_size = sizeof(MSG_CREATE_TASK_STRUCT);
			break;

		case MSG_STOP_TASK:
			msg_size = sizeof(INT32U);
		default:
			break;
	}

	// Sending the rest of data.
	if(msg_size > 0) blocking_mb_block_write(addr, msg, msg_size);

#if DEBUG_EN
//		print("Message sent to local runtime\n\r");
#endif
}


/*
*********************************************************************************************************
*                                              wait_ext_msg
*
* Description: Waits for a message from a local runtime through the corresponding mailbox.
*
* Arguments  :	addr is the base address of the targeted mailbox.
* 				msg	is the message to be read.
*
* Returns    : none
*
*********************************************************************************************************
*/
void  wait_ext_msg(INT32U addr, void* msg)
{
	INT32U 	msg_type;

	msg_type = blocking_mb_read(addr);	// Reads the first incoming word i.e. the message type.

	switch (msg_type) {
		case MSG_CURR_VERTEX_ID:
			blocking_mb_block_read(addr, msg, sizeof(INT32U));
		default:
			break;
	}
}
