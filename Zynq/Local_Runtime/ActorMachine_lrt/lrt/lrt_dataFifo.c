/*
 * lrt_dataFifo.c
 *
 *  Created on: Jul 25, 2013
 *      Author: jheulot yoliva
 */

#include <string.h>
#include "lrt_prototypes.h"

#include <types.h>
#include <hwQueues.h>
#include <platform.h>
#include <sharedMem.h>

#include <hwQueues.h>
#include <print.h>

/* MACROs */
#define DATA_ADD(f)  f->address + FIFO_DATA_OFFSET
#define RD_IX_ADD(f) f->address + FIFO_RD_IX_OFFSET
#define WR_IX_ADD(f) f->address + FIFO_WR_IX_OFFSET

/* LOCAL VARIABLES */
static LRT_FIFO_HNDLE FIFO_tbl[OS_NB_FIFO]; /* Table of FIFO handles */

/* Initialize the FIFO from a CREATE_FIFO_STRUCT */
UINT8 create_fifo(){
	UINT8 id = OS_CtrlQPop_UINT32();
	LRT_FIFO_HNDLE* fifo_hndl;
	if (id >= OS_NB_FIFO) {
		exitWithCode(1000);
	}

	fifo_hndl = &FIFO_tbl[id];
	fifo_hndl->Size 	= OS_CtrlQPop_UINT32();
	fifo_hndl->address 	= OS_CtrlQPop_UINT32();
	fifo_hndl->Status 	= FIFO_STAT_INIT;

//	zynq_puts("Create Fifo ID"); zynq_putdec(id);
//	zynq_puts(" @"); zynq_puthex(fifo_hndl->address);
//	zynq_puts(" L"); zynq_puthex(fifo_hndl->Size);
//	zynq_puts("\n");

	return OS_ERR_NONE;
}

UINT8 create_fifo_args(UINT8 id, UINT32 size, UINT32 address){
	LRT_FIFO_HNDLE* fifo_hndl;
	if (id >= OS_NB_FIFO) {
		exitWithCode(1000);
	}

	fifo_hndl = &FIFO_tbl[id];
	fifo_hndl->Size = size;
	fifo_hndl->address = address;
	fifo_hndl->Status = FIFO_STAT_INIT;

//	zynq_puts("Create Fifo ID"); zynq_putdec(id);
//	zynq_puts(" @"); zynq_puthex(fifo_hndl->address);
//	zynq_puts(" L"); zynq_puthex(fifo_hndl->Size);
//	zynq_puts("\n");

	return OS_ERR_NONE;
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
LRT_FIFO_HNDLE* get_fifo_hndl(UINT8 fifo_id) {
	LRT_FIFO_HNDLE* fifo_hndl;
	if (fifo_id >= OS_NB_FIFO) {
		exitWithCode(1001);
	}

	fifo_hndl = &FIFO_tbl[fifo_id];

	if (fifo_hndl->Status != FIFO_STAT_INIT) {
		exitWithCode(1002);
	}

	return fifo_hndl;
}

/* Flush the contents of a FIFO */
void flush_fifo() {
	UINT8 fifo_id = OS_CtrlQPop_UINT32();
	LRT_FIFO_HNDLE* fifo_hndl = get_fifo_hndl(fifo_id);
	UINT32 tmp = 0;

	OS_ShMemWrite(RD_IX_ADD(fifo_hndl), &tmp, sizeof(UINT32));
	OS_ShMemWrite(WR_IX_ADD(fifo_hndl), &tmp, sizeof(UINT32));

//	zynq_puts("Clear Fifo ID"); zynq_putdec(fifo_id); zynq_puts("\n");
}

void flush_fifo_args(UINT8 fifo_id) {
	LRT_FIFO_HNDLE* fifo_hndl = get_fifo_hndl(fifo_id);
	UINT32 tmp = 0;

	OS_ShMemWrite(RD_IX_ADD(fifo_hndl), &tmp, sizeof(UINT32));
	OS_ShMemWrite(WR_IX_ADD(fifo_hndl), &tmp, sizeof(UINT32));

//	zynq_puts("Clear Fifo ID"); zynq_putdec(fifo_id); zynq_puts("\n");
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
 * Returns	 : TRUE if there is enough data.
 *
 *********************************************************************************************************
 */
BOOLEAN check_input_fifo(UINT8 in_fifo_id, UINT32 size) {
	UINT32 wr_ix, rd_ix;
	LRT_FIFO_HNDLE	*in_fifo_hndl = get_fifo_hndl(in_fifo_id);

	OS_ShMemRead(RD_IX_ADD(in_fifo_hndl), &rd_ix, sizeof(UINT32));
	OS_ShMemRead(WR_IX_ADD(in_fifo_hndl), &wr_ix, sizeof(UINT32));

	if (wr_ix < rd_ix)// If TRUE, wr_ix reached the end of the memory and restarted from the beginning.
		wr_ix = wr_ix + in_fifo_hndl->Size;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

	return (rd_ix + size) <= wr_ix;	// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
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
void read_input_fifo(UINT8 in_fifo_id, UINT32 size, UINT8* buffer) {
	UINT32 wr_ix, rd_ix, temp;
	LRT_FIFO_HNDLE	*in_fifo_hndl = get_fifo_hndl(in_fifo_id);

	while(1){
		// Get indices from the handle.
		OS_ShMemRead(RD_IX_ADD(in_fifo_hndl), &rd_ix, sizeof(UINT32));
		OS_ShMemRead(WR_IX_ADD(in_fifo_hndl), &wr_ix, sizeof(UINT32));

		if (wr_ix < rd_ix)// If TRUE, wr_ix reached the end of the memory and restarted from the beginning.
			wr_ix += in_fifo_hndl->Size;// Place wr_ix to the right of rd_ix as in an unbounded memory.

		if (rd_ix + size <= wr_ix){
			// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
			if (rd_ix + size > in_fifo_hndl->Size) {
				OS_ShMemRead(DATA_ADD(in_fifo_hndl) + rd_ix, buffer, in_fifo_hndl->Size - rd_ix);
				OS_ShMemRead(DATA_ADD(in_fifo_hndl), buffer + in_fifo_hndl->Size - rd_ix, size - in_fifo_hndl->Size + rd_ix);
			} else {
				OS_ShMemRead(DATA_ADD(in_fifo_hndl) + rd_ix, buffer, size);
			}

			// Update the read index.
			rd_ix = (rd_ix + size) % in_fifo_hndl->Size;
			do{
				OS_ShMemWrite(RD_IX_ADD(in_fifo_hndl), &rd_ix, sizeof(UINT32));
				OS_ShMemRead(RD_IX_ADD(in_fifo_hndl), &temp, sizeof(UINT32));
			}while(rd_ix != temp);

			return;
		}
	}
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
 * Returns    : TRUE if there is enough space in the FIFO.
 *
 *********************************************************************************************************
 */
BOOLEAN check_output_fifo(UINT8 out_fifo_id, UINT32 size) {
	UINT32 wr_ix, rd_ix;
	LRT_FIFO_HNDLE	*out_fifo_hndl = get_fifo_hndl(out_fifo_id);

	OS_ShMemRead(RD_IX_ADD(out_fifo_hndl), &rd_ix, sizeof(UINT32));
	OS_ShMemRead(WR_IX_ADD(out_fifo_hndl), &wr_ix, sizeof(UINT32));

	if (rd_ix <= wr_ix)	// If TRUE, rd_ix reached the end of the memory and restarted from the beginning.
						// Or the FIFO is empty.
		rd_ix += out_fifo_hndl->Size;// Place rd_ix to the right of wr_ix as in an unbounded memory.

	return (wr_ix + size) <= rd_ix; /* Writer is allowed to write until wr_ix == rd_ix - 1
	 * cause wr_ix == rd_ix means that the FIFO is empty.
	 */
}

UINT32 get_fifo_cnt(UINT8 fifo_id){
	UINT32 wr_ix, rd_ix;
	LRT_FIFO_HNDLE	*fifo_hndl = get_fifo_hndl(fifo_id);

	OS_ShMemRead(RD_IX_ADD(fifo_hndl), &rd_ix, sizeof(UINT32));
	OS_ShMemRead(WR_IX_ADD(fifo_hndl), &wr_ix, sizeof(UINT32));

	if (wr_ix < rd_ix)// If TRUE, wr_ix reached the end of the memory and restarted from the beginning.
		wr_ix = wr_ix + fifo_hndl->Size;	// Place wr_ix to the right of rd_ix as in an unbounded memory.

	return wr_ix-rd_ix ;	// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
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
void write_output_fifo(UINT8 out_fifo_id, UINT32 size, UINT8* buffer) {
	UINT32 wr_ix, rd_ix, temp;
	LRT_FIFO_HNDLE	*out_fifo_hndl = get_fifo_hndl(out_fifo_id);

	while(1){
		// Get indices from the handle.
		OS_ShMemRead(RD_IX_ADD(out_fifo_hndl), &rd_ix, sizeof(UINT32));
		OS_ShMemRead(WR_IX_ADD(out_fifo_hndl), &wr_ix, sizeof(UINT32));

		if (rd_ix <= wr_ix)	// If TRUE, rd_ix reached the end of the memory and restarted from the beginning.
							// Or the FIFO is empty.
			rd_ix += out_fifo_hndl->Size;// Place rd_ix to the right of wr_ix as in an unbounded memory.

		if (wr_ix + size < rd_ix){ // Writer is allowed to write until wr_ix == rd_ix - 1
								   // cause wr_ix == rd_ix means that the FIFO is empty.

			if (wr_ix + size > out_fifo_hndl->Size) {
				OS_ShMemWrite(DATA_ADD(out_fifo_hndl) + wr_ix, buffer, out_fifo_hndl->Size - wr_ix);
				OS_ShMemWrite(DATA_ADD(out_fifo_hndl), buffer + out_fifo_hndl->Size - wr_ix, size - out_fifo_hndl->Size + wr_ix);
			} else {
				OS_ShMemWrite(DATA_ADD(out_fifo_hndl) + wr_ix, buffer, size);
			}

			// Update write index.
			wr_ix = (wr_ix + size) % out_fifo_hndl->Size;

			do{
				OS_ShMemWrite(WR_IX_ADD(out_fifo_hndl), &wr_ix, sizeof(UINT32));
				OS_ShMemRead(WR_IX_ADD(out_fifo_hndl), &temp, sizeof(UINT32));
			}while(wr_ix != temp);


			return;
		}
	}
}
