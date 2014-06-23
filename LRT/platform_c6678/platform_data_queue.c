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

#include <lrt_cfg.h>
#include <stdio.h>
#include <stdlib.h>

#include <platform.h>
#include <platform_types.h>
#include <qmss_utils.h>
#include "cache.h"
#include "memoryAlloc.h"

#define EMPTY_DATA 	940
#define BASE_DATA	941
#define NB_Q		8191 - BASE_ID

#define FIFO_MUTEX_SIZE			1
#define SH_MEM_HDR_REGION_SIZE  FIFO_MUTEX_SIZE*OS_NB_FIFO

void platform_flushFIFO(UINT32 id){
	UINT32 n;
	empty_queue(id, (UINT32*)NULL, &n);
}


void platform_writeFifo(UINT32 id, UINT32 addr, UINT32 size, UINT8* buffer) {
	MNAV_MonolithicPacketDescriptor *mono_pkt;
	do{
		mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(EMPTY_DATA);
	}while(mono_pkt == 0);

	cache_invL1D(mono_pkt, DATA_DESC_SIZE);

	mono_pkt->type_id = 0x2;
	mono_pkt->packet_type = 0;
	mono_pkt->data_offset = 12;
	mono_pkt->packet_length = 16;
	mono_pkt->epib = 0;
	mono_pkt->pkt_return_qnum = EMPTY_DATA;
	mono_pkt->src_tag_lo = 1; //copied to .flo_idx of streaming i/f

//	memcpy((void*)(SHARED_MEM_BASE + addr), buffer, size);

	cache_wbInvL1D(mono_pkt, DATA_DESC_SIZE);
#ifdef ENABLE_CACHE
	cache_wbInvL1D((void*)(SHARED_MEM_BASE + addr), size);
#endif

	push_queue(BASE_DATA+id, 1, 0, (UINT32)mono_pkt);
}


void platform_readFifo(UINT32 id, UINT32 addr, UINT32 size, UINT8* buffer) {
	MNAV_MonolithicPacketDescriptor *mono_pkt;

	do{
		mono_pkt = (MNAV_MonolithicPacketDescriptor*)pop_queue(BASE_DATA+id);
	}while(mono_pkt == 0);

	cache_invL1D(mono_pkt, DATA_DESC_SIZE);
#ifdef ENABLE_CACHE
	cache_invL1D((void*)(SHARED_MEM_BASE + addr), size);
#endif
//	memcpy(buffer, (void*)(SHARED_MEM_BASE + addr), size);

	push_queue(EMPTY_DATA, 1, 0, (UINT32)mono_pkt);
}
