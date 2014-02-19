
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#include <string.h>
#include <types.h>
#include <platform.h>
#include "sharedMem.h"
#include "swfifoMngr.h"
//#include <rtErrorCodes.h>

/* MACROs */
#define FIFO_RD_IX_OFFSET		0		// Offsets in number of 32-bits words.
#define FIFO_WR_IX_OFFSET		4
#define FIFO_DATA_OFFSET		8

#define DATA_ADD(f)  f->address + FIFO_DATA_OFFSET
#define RD_IX_ADD(f) f->address + FIFO_RD_IX_OFFSET
#define WR_IX_ADD(f) f->address + FIFO_WR_IX_OFFSET


void create_swfifo(RT_SW_FIFO_HNDLE* fifo_hndl, UINT32 size, UINT32 address){
	fifo_hndl->Size = size;
	fifo_hndl->address = address;
}


void flush_swfifo(UINT32 cpuId, RT_SW_FIFO_HNDLE* fifo_hndl) {
	UINT32 tmp = 0;

	ShMemWrite(cpuId, RD_IX_ADD(fifo_hndl), &tmp, sizeof(UINT32));
	ShMemWrite(cpuId, WR_IX_ADD(fifo_hndl), &tmp, sizeof(UINT32));
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
bool check_input_swfifo(UINT32 cpuId, RT_SW_FIFO_HNDLE	*in_fifo_hndl, UINT32 size) {
	UINT32 wr_ix, rd_ix;

	ShMemRead(cpuId, RD_IX_ADD(in_fifo_hndl), &rd_ix, sizeof(UINT32));
	ShMemRead(cpuId, WR_IX_ADD(in_fifo_hndl), &wr_ix, sizeof(UINT32));

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
void read_input_swfifo(UINT32 cpuId, RT_SW_FIFO_HNDLE	*in_fifo_hndl, UINT32 size, UINT8* buffer) {
	UINT32 wr_ix, rd_ix, temp;

	while(1){
		// Get indices from the handle.
		ShMemRead(cpuId, RD_IX_ADD(in_fifo_hndl), &rd_ix, sizeof(UINT32));
		ShMemRead(cpuId, WR_IX_ADD(in_fifo_hndl), &wr_ix, sizeof(UINT32));

		if (wr_ix < rd_ix)// If TRUE, wr_ix reached the end of the memory and restarted from the beginning.
			wr_ix += in_fifo_hndl->Size;// Place wr_ix to the right of rd_ix as in an unbounded memory.

		if (rd_ix + size <= wr_ix){
			// Reader is allowed to read until rd_ix == wr_ix, i.e. until FIFO is empty.
			if (rd_ix + size > in_fifo_hndl->Size) {
				ShMemRead(cpuId, DATA_ADD(in_fifo_hndl) + rd_ix, buffer, in_fifo_hndl->Size - rd_ix);
				ShMemRead(cpuId, DATA_ADD(in_fifo_hndl), buffer + in_fifo_hndl->Size - rd_ix, size - in_fifo_hndl->Size + rd_ix);
			} else {
				ShMemRead(cpuId, DATA_ADD(in_fifo_hndl) + rd_ix, buffer, size);
			}

			// Update the read index.
			rd_ix = (rd_ix + size) % in_fifo_hndl->Size;
			do{
				ShMemWrite(cpuId, RD_IX_ADD(in_fifo_hndl), &rd_ix, sizeof(UINT32));
				ShMemRead(cpuId, RD_IX_ADD(in_fifo_hndl), &temp, sizeof(UINT32));
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
bool check_output_swfifo(UINT32 cpuId, RT_SW_FIFO_HNDLE *out_fifo_hndl, UINT32 size) {
	UINT32 wr_ix, rd_ix;

	ShMemRead(cpuId, RD_IX_ADD(out_fifo_hndl), &rd_ix, sizeof(UINT32));
	ShMemRead(cpuId, WR_IX_ADD(out_fifo_hndl), &wr_ix, sizeof(UINT32));

	if (rd_ix <= wr_ix)	// If TRUE, rd_ix reached the end of the memory and restarted from the beginning.
						// Or the FIFO is empty.
		rd_ix += out_fifo_hndl->Size;// Place rd_ix to the right of wr_ix as in an unbounded memory.

	return (wr_ix + size) <= rd_ix; /* Writer is allowed to write until wr_ix == rd_ix - 1
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
void write_output_swfifo(UINT32 cpuId, RT_SW_FIFO_HNDLE *out_fifo_hndl, UINT32 size, UINT8* buffer) {
	UINT32 wr_ix, rd_ix, temp;

	while(1){
		// Get indices from the handle.
		ShMemRead(cpuId, RD_IX_ADD(out_fifo_hndl), &rd_ix, sizeof(UINT32));
		ShMemRead(cpuId, WR_IX_ADD(out_fifo_hndl), &wr_ix, sizeof(UINT32));

		if (rd_ix <= wr_ix)	// If TRUE, rd_ix reached the end of the memory and restarted from the beginning.
							// Or the FIFO is empty.
			rd_ix += out_fifo_hndl->Size;// Place rd_ix to the right of wr_ix as in an unbounded memory.

		if (wr_ix + size < rd_ix){ // Writer is allowed to write until wr_ix == rd_ix - 1
								   // cause wr_ix == rd_ix means that the FIFO is empty.

			if (wr_ix + size > out_fifo_hndl->Size) {
				ShMemWrite(cpuId, DATA_ADD(out_fifo_hndl) + wr_ix, buffer, out_fifo_hndl->Size - wr_ix);
				ShMemWrite(cpuId, DATA_ADD(out_fifo_hndl), buffer + out_fifo_hndl->Size - wr_ix, size - out_fifo_hndl->Size + wr_ix);
			} else {
				ShMemWrite(cpuId, DATA_ADD(out_fifo_hndl) + wr_ix, buffer, size);
			}

			// Update write index.
			wr_ix = (wr_ix + size) % out_fifo_hndl->Size;

			do{
				ShMemWrite(cpuId, WR_IX_ADD(out_fifo_hndl), &wr_ix, sizeof(UINT32));
				ShMemRead(cpuId, WR_IX_ADD(out_fifo_hndl), &temp, sizeof(UINT32));
			}while(wr_ix != temp);


			return;
		}
	}
}
