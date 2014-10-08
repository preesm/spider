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

#include <platform_types.h>
#include <platform_queue.h>
#include <grt_definitions.h>

#include <ti/csl/csl_tsc.h>
#include <string.h>
#include <stdio.h>
#include "qmss.h"

#include <sys/mman.h>

#include <stdint.h>
#include <unistd.h>
#include <ti/drv/qmss/qmss_drv.h>

#define CTRL_OUT(id)	(BASE_CTRL_OUT 	+ id-1)
#define CTRL_IN(id)		(BASE_CTRL_IN	+ id-1)

static MonoPcktDesc* cur_mono_pkt_in[13];
static int cur_mono_pkt_in_size[13];

static MonoPcktDesc* cur_mono_pkt_out[13];
static int cur_mono_pkt_out_size[13];

void delay(UINT32 cycles){
	usleep(cycles);
}

UINT32 platform_QPush_data_internal(UINT8 slaveId, void* data, int size){
	while(cur_mono_pkt_out[slaveId] == 0){
		cur_mono_pkt_out[slaveId] = (MonoPcktDesc*)Qmss_queuePop(EMPTY_CTRL);
		if(cur_mono_pkt_out[slaveId] != 0){
			/* Initialize header */
//			cache_invL1D(cur_mono_pkt[slaveId][queueType][0], CTRL_DESC_SIZE);

			cur_mono_pkt_out[slaveId]->type_id = 0x2;
			cur_mono_pkt_out[slaveId]->packet_type = 0x10;
			cur_mono_pkt_out[slaveId]->data_offset = 12;
			cur_mono_pkt_out[slaveId]->epib = 0;
			cur_mono_pkt_out[slaveId]->pkt_return_qnum = EMPTY_CTRL;
			cur_mono_pkt_out[slaveId]->src_tag_lo = 1; //copied to .flo_idx of streaming i/f
			break;
		}
		delay(100);
	}

	UINT32 toCopy = MIN(CTRL_DESC_SIZE - cur_mono_pkt_out_size[slaveId] - PACKET_HEADER, size);

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((UINT32)cur_mono_pkt_out[slaveId]) + PACKET_HEADER + cur_mono_pkt_out_size[slaveId]);
	memcpy(data_pkt, data, toCopy);

	cur_mono_pkt_out_size[slaveId] += toCopy;
	return toCopy;
}

void platform_QPush_finalize(UINT8 slaveId){
	if(cur_mono_pkt_out[slaveId]){
		/* Send the descriptor */
		cur_mono_pkt_out[slaveId]->packet_length = cur_mono_pkt_out_size[slaveId]+PACKET_HEADER;

		msync(cur_mono_pkt_out[slaveId], CTRL_DESC_SIZE, MS_SYNC);
//		cache_wbInvL1D(cur_mono_pkt[slaveId][queueType][0], CTRL_DESC_SIZE);

		Qmss_queuePushDesc(CTRL_OUT(slaveId), cur_mono_pkt_out[slaveId]);

		cur_mono_pkt_out[slaveId] = 0;
		cur_mono_pkt_out_size[slaveId] = 0;

	}
}

UINT32 platform_QPush_data(UINT8 slaveId, void* data, int size){
	UINT32 sended = 0;
	sended += platform_QPush_data_internal(slaveId, ((char*)data)+sended, size-sended);
	while(sended < size){
		platform_QPush_finalize(slaveId);
		sended += platform_QPush_data_internal(slaveId, ((char*)data)+sended, size-sended);
	}
	return sended;
}



UINT32 platform_QPush(UINT8 slaveId, void* data, int size){
	int res = 0;
	while(res != size){
		res += platform_QPush_data(slaveId, (void*)((UINT32)data+res), MIN(size-res, PACKET_HEADER));
	}
	return res;
}

UINT32 platform_QPushUINT32(UINT8 slaveId, UINT32 data){
	UINT32 res = platform_QPush_data(slaveId, &data, sizeof(unsigned int));
	return res;
}

UINT32 platform_QNBPop_data(UINT8 slaveId, void* data, UINT32 size){
	if(cur_mono_pkt_in[slaveId] == 0){
		cur_mono_pkt_in[slaveId] = (MonoPcktDesc*)Qmss_queuePop(CTRL_IN(slaveId));
		if(cur_mono_pkt_in[slaveId] == 0){
			return 0;
		}

		msync(cur_mono_pkt_in[slaveId], CTRL_DESC_SIZE, MS_SYNC);
//		cache_invL1D(cur_mono_pkt[slaveId][queueType][1], CTRL_DESC_SIZE);
	}

	void* data_pkt = (void*)(((UINT32)cur_mono_pkt_in[slaveId])
			+ cur_mono_pkt_in[slaveId]->data_offset
			+ cur_mono_pkt_in_size[slaveId]);
	UINT32 data_size = cur_mono_pkt_in[slaveId]->packet_length
			- cur_mono_pkt_in[slaveId]->data_offset
			- cur_mono_pkt_in_size[slaveId];

	UINT32 toCopy = MIN(size, data_size);

	memcpy(data, data_pkt, toCopy);
	cur_mono_pkt_in_size[slaveId] += toCopy;

	/* End of descriptor */
	if(toCopy == data_size){
		Qmss_queuePushDesc(EMPTY_CTRL, cur_mono_pkt_in[slaveId]);
		cur_mono_pkt_in[slaveId] = 0;
		cur_mono_pkt_in_size[slaveId] = 0;
	}

	return toCopy;
}


UINT32 platform_QPop(UINT8 slaveId, void* data, int size){
	UINT32 recved = 0;
	while(recved < size){
		recved += platform_QNBPop_data(slaveId, (char*)data+recved, size-recved);
	}
	return recved;
}

UINT32 platform_QPopUINT32(UINT8 slaveId){
	UINT32 data;
	platform_QPop(slaveId, &data, sizeof(UINT32));
	return data;
}

UINT32 platform_QNonBlockingPop(UINT8 slaveId, void* data, int size){
	return platform_QNBPop_data(slaveId, data, size);
}
