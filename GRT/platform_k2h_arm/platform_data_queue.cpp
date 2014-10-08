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
#include <stdio.h>
#include <stdlib.h>

#include <platform_types.h>
#include <platform.h>
#include <platform_data_queue.h>
#include <sys/mman.h>
#include "qmss.h"

#include <stdint.h>

extern "C"{
#include <ti/drv/qmss/qmss_drv.h>
}

void platform_flushFIFO(UINT32 id){
    Qmss_queueEmpty (id);
}


void platform_writeFifo(UINT32 id, UINT32 addr, UINT32 size, UINT8* buffer) {
	MonoPcktDesc *mono_pkt;
	int queueId = BASE_DATA+id;

	if(queueId < BASE_DATA || queueId > MAX_QUEUES ){
		printf("Error: request queue %d, out of bound\n", id);
		abort();
	}

	do{
		mono_pkt = (MonoPcktDesc*)Qmss_queuePop(EMPTY_DATA);
	}while(mono_pkt == 0);

//	cache_invL1D(mono_pkt, DATA_DESC_SIZE);

	mono_pkt->type_id = 0x2;
	mono_pkt->packet_type = 0;
	mono_pkt->data_offset = 12;
	mono_pkt->packet_length = 16;
	mono_pkt->epib = 0;
	mono_pkt->pkt_return_qnum = EMPTY_DATA;
	mono_pkt->src_tag_lo = 1; //copied to .flo_idx of streaming i/f

//	memcpy((void*)(SHARED_MEM_BASE + addr), buffer, size);

//	cache_wbInvL1D(mono_pkt, DATA_DESC_SIZE);
	msync(mono_pkt, DATA_DESC_SIZE, MS_SYNC);
#ifdef ENABLE_CACHE
//	cache_wbInvL1D((void*)(SHARED_MEM_BASE + addr), size);
	msync(platform_getDataMemAdd()+addr, size, MS_SYNC);
#endif

	Qmss_queuePushDesc(queueId, mono_pkt);
//	push_queue(BASE_DATA+id, 1, 0, (UINT32)mono_pkt);
}


void platform_readFifo(UINT32 id, UINT32 addr, UINT32 size, UINT8* buffer) {
	MonoPcktDesc *mono_pkt;
	int queueId = BASE_DATA+id;

	if(queueId < BASE_DATA || queueId > MAX_QUEUES ){
		printf("Error: request queue %d, out of bound\n", id);
		abort();
	}

	do{
		mono_pkt = (MonoPcktDesc*)Qmss_queuePop(BASE_DATA+id);
	}while(mono_pkt == 0);

//	cache_invL1D(mono_pkt, DATA_DESC_SIZE);
	msync(mono_pkt, DATA_DESC_SIZE, MS_SYNC);
#ifdef ENABLE_CACHE
//	cache_invL1D((void*)(SHARED_MEM_BASE + addr), size);
	msync(platform_getDataMemAdd()+addr, size, MS_SYNC);
#endif
//	memcpy(buffer, (void*)(SHARED_MEM_BASE + addr), size);

	Qmss_queuePushDesc(EMPTY_DATA, mono_pkt);
}
