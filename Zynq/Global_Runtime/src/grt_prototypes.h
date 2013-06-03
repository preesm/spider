/********************************************************************************
 * File    : grt_prototypes.h
 * By      : Yaset Oliva Venegas
 * Version : 1.0
 * Date	   : October 2012
 * Descrip : This file contains the prototypes of the global RunTime's functions.
 * 			Prototypes should be MTAPI-compliant.
*********************************************************************************/

#ifndef GRT_PROTOTYPES
#define GRT_PROTOTYPES

#include "grt_definitions.h"
#include <xil_types.h>





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
//extern void create_fifo_hndl(u32 addr, u32 size, u8 dir);






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
extern void flush_mailboxes(u32 mb_base_addr);






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
extern void  send_ext_msg(u32 addr, u32 msg_type, void* msg);


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
extern void  wait_ext_msg(INT32U addr, void* msg);
#endif
