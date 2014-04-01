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

#include <grt_definitions.h>
#include "queue_buffer.h"

#define BUFFER_SIZE 160

static UINT8  buffer[NB_MAX_CTRLQ][platformNbQTypes][BUFFER_SIZE];
static UINT32 read_idx[NB_MAX_CTRLQ][platformNbQTypes], write_idx[NB_MAX_CTRLQ][platformNbQTypes];

UINT32 QBuffer_getNbData(UINT8 slaveId, platformQType type){
	INT32 count = write_idx[slaveId][type]-read_idx[slaveId][type];
	if (count < 0) count += BUFFER_SIZE;
	return count;
}

void QBuffer_push(UINT8 slaveId, platformQType type, void* data, UINT32 size){
	UINT32 rd_ix = read_idx[slaveId][type];
	UINT32 wr_ix = write_idx[slaveId][type];
	if (rd_ix <= wr_ix)
		rd_ix += BUFFER_SIZE;

	if (wr_ix + size < rd_ix){
		if (wr_ix + size >BUFFER_SIZE) {
			memcpy(buffer[slaveId][type] + wr_ix, data, BUFFER_SIZE - wr_ix);
			memcpy(buffer[slaveId][type],
					(void*)(((UINT32)data) + BUFFER_SIZE - wr_ix),
					size - BUFFER_SIZE + wr_ix);
		} else {
			memcpy(buffer[slaveId][type] + wr_ix, data, size);
		}

		// Update write index.
		write_idx[slaveId][type] = (write_idx[slaveId][type] + size) % BUFFER_SIZE;
	}
}

void QBuffer_pop(UINT8 slaveId, platformQType type, void* data, UINT32 size){
	UINT32 rd_ix = read_idx[slaveId][type];
	UINT32 wr_ix = write_idx[slaveId][type];

	if (wr_ix < rd_ix)
		wr_ix += BUFFER_SIZE;

	if (rd_ix + size <= wr_ix){
		if (rd_ix + size > BUFFER_SIZE) {
			memcpy(data, buffer[slaveId][type]+rd_ix, BUFFER_SIZE-rd_ix);
			memcpy((void*)(((UINT32)data)+BUFFER_SIZE-rd_ix),
					buffer[slaveId][type], size - BUFFER_SIZE + rd_ix);
		} else {
			memcpy(data, buffer[slaveId][type]+rd_ix, size);
		}

		// Update the read index.
		read_idx[slaveId][type] = (read_idx[slaveId][type] + size) % BUFFER_SIZE;
	}
}
