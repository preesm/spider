/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#ifndef SW_FIFOMNGR_H_
#define SW_FIFOMNGR_H_

#include "types.h"

#define FIFO_RD_IX_OFFSET		0		// Offsets in number of 32-bits words.
#define FIFO_WR_IX_OFFSET		4
#define FIFO_DATA_OFFSET		8

//************************ Status codes
#define FIFO_STAT_INIT		1
#define FIFO_STAT_USED		2

// FIFO handle block
typedef struct lrt_fifo_hndle{
    UINT32	address;				// FIFO's base address.
    UINT32  Size;                 		// FIFO's depth.
    UINT8	Status;						// One of the defined FIFO states.
} LRT_FIFO_HNDLE;


UINT8 create_swfifo(LRT_FIFO_HNDLE* fifo_hndl, UINT32 size, UINT32 address);
//void flush_fifo();
//void flush_fifo_args(UINT8 fifo_id);
BOOLEAN check_input_swfifo(LRT_FIFO_HNDLE	*in_fifo_hndl, UINT32 size);
BOOLEAN check_output_swfifo(LRT_FIFO_HNDLE *out_fifo_hndl, UINT32 size);
//UINT32 get_fifo_cnt(UINT8 fifo_id);
void write_output_swfifo(LRT_FIFO_HNDLE *out_fifo_hndl, UINT32 size, UINT8* buffer);
void read_input_swfifo(LRT_FIFO_HNDLE	*in_fifo_hndl, UINT32 size, UINT8* buffer);
#endif /* SW_FIFOMNGR_H_ */
