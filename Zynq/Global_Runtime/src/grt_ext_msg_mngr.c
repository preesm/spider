/*
 * rt_ext_msg_mngr.c
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

#include <xil_io.h>
#include <xil_types.h>
//#include <xil_printf.h>
#include <xmbox_hw.h>
#include "grt_cfg.h"
#include "xparameters.h"
#include "grt_definitions.h"


/**********************************************************************************************************
*                                              blocking_write
* Description: Writes a 32 bits block into a mailbox. Blocks if it is full.
*
* Arguments  : mb_base_addr is the base address of the mailbox.
* 			   data to be copied.
*
* Returns    : none
**********************************************************************************************************/
void blocking_write(u32 mb_base_addr, u32 data)
{
	// Wait until there is space in the FIFO
	while(XMbox_IsFullHw(mb_base_addr));
	XMbox_WriteMBox(mb_base_addr, data);
}



/**********************************************************************************************************
*                                              blocking_read
* Description: Reads a 32 bits block from a mailbox. Blocks if it is empty.
*
* Arguments  : mb_base_addr is the base address of the mailbox.
*
* Returns    : the read data.
**********************************************************************************************************/
u32 blocking_read(u32 mb_base_addr)
{
	// Wait until there is some data in the FIFO
//	while(FIFO_IsEmpty(XPAR_MBOX_0_BASEADDR));
//	return X_mReadReg(XPAR_MBOX_0_BASEADDR, FIFO_READ_REG_OFF);
	return 0;
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





/*$PAGE*/
/*
*********************************************************************************************************
*                                              send_ext_msg
*
* Description: Sends a message to a local runtime through the corresponding mailbox.
*
* Arguments  :	mb_base_addr is the base address of the targeted mailbox.
* 				msg	is the message to be sent.
*
* Returns    : none
*
*********************************************************************************************************
*/
void  send_ext_msg(u32 addr, void* msg, u16 size)
{
	u16 nb_writes = size/4;		// Number of words to write a complete msg.
	// Writes into the mailbox.
	while(nb_writes > 0)
	{
		nb_writes--;
		blocking_write(addr, *(u32*)msg);
		msg = (u32*)msg + 1;
	}
#if DEBUG_EN
		print("Message sent to local runtime\n\r");
#endif
}
