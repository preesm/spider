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

#include <stdio.h>
#include <stdlib.h>

#include <ti/csl/csl_tsc.h>

#include <platform_types.h>
#include <platform_queue.h>
#include <platform.h>

#include "qmss.h"
#include "cache.h"

#include <ti/drv/qmss/qmss_mgmt.h>

static MonoPcktDesc* cur_mono_pkt_in;
static int cur_mono_pkt_in_size;
static MonoPcktDesc* cur_mono_pkt_out;
static int cur_mono_pkt_out_size;

void delay(UINT32 cycles){
	UINT64      value;
    /* Get the current TSC  */
    value = CSL_tscRead ();
    while ((CSL_tscRead ()  - value) < cycles);
}

UINT32 platform_queue_push_data_internal(void* data, int size){
	while(cur_mono_pkt_out == 0){
		cur_mono_pkt_out = (MonoPcktDesc*)Qmss_queuePop(EMPTY_CTRL);
		if(cur_mono_pkt_out !=0){
			/* Initialize header */
			cache_invL1D(cur_mono_pkt_out, CTRL_DESC_SIZE);

			cur_mono_pkt_out->type_id = 0x2;
			cur_mono_pkt_out->packet_type = 0x0;
			cur_mono_pkt_out->data_offset = 12;
			cur_mono_pkt_out->epib = 0;
			cur_mono_pkt_out->pkt_return_qnum = EMPTY_CTRL;
			cur_mono_pkt_out->src_tag_lo = 1; //copied to .flo_idx of streaming i/f
			break;
		}
		delay(100);
	}

	UINT32 toCopy = MIN(CTRL_DESC_SIZE - cur_mono_pkt_out_size - PACKET_HEADER, size);

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((UINT32)cur_mono_pkt_out) + PACKET_HEADER + cur_mono_pkt_out_size);
	memcpy(data_pkt, data, toCopy);

	cur_mono_pkt_out_size += toCopy;
	return toCopy;
}

void platform_queue_push_finalize(){
	if(cur_mono_pkt_out){
		/* Send the descriptor */
		cur_mono_pkt_out->packet_length = cur_mono_pkt_out_size+PACKET_HEADER;

		cache_wbInvL1D(cur_mono_pkt_out, CTRL_DESC_SIZE);

		Qmss_queuePushDesc(platform_getCoreId()+BASE_CTRL_OUT, cur_mono_pkt_out);

		cur_mono_pkt_out = 0;
		cur_mono_pkt_out_size = 0;

	}
}

UINT32 platform_queue_push(void* data, int size){
	UINT32 sended = 0;
	sended += platform_queue_push_data_internal(((char*)data)+sended, size-sended);
	while(sended < size){
		platform_queue_push_finalize();
		sended += platform_queue_push_data_internal(((char*)data)+sended, size-sended);
	}
	return sended;
}

UINT32 platform_queue_push_UINT32(UINT32 data){
	UINT32 res = platform_queue_push(&data, sizeof(unsigned int));
	return res;
}

UINT32 platform_queue_nbpop_data(void* data, int size){
	if(cur_mono_pkt_in == 0){
		cur_mono_pkt_in = (MonoPcktDesc*)Qmss_queuePop(BASE_CTRL_IN + platform_getCoreId());
		if(cur_mono_pkt_in == 0){
			return 0;
		}
		cache_invL1D(cur_mono_pkt_in, CTRL_DESC_SIZE);
	}

	void* data_pkt = (void*)(((UINT32)cur_mono_pkt_in)
			+ cur_mono_pkt_in->data_offset
			+ cur_mono_pkt_in_size);
	UINT32 data_size = cur_mono_pkt_in->packet_length
			- cur_mono_pkt_in->data_offset
			- cur_mono_pkt_in_size;

	UINT32 toCopy = MIN(size, data_size);

	memcpy(data, data_pkt, toCopy);
	cur_mono_pkt_in_size += toCopy;

	/* End of descriptor */
	if(toCopy == data_size){
		Qmss_queuePushDesc(EMPTY_CTRL, cur_mono_pkt_in);
		cur_mono_pkt_in = 0;
		cur_mono_pkt_in_size = 0;
	}

	return toCopy;
}

UINT32 platform_queue_pop_UINT32(){
	UINT32 data;
	platform_queue_pop(&data, sizeof(UINT32));
	return data;
}


UINT32 platform_queue_pop(void* data, int size){
	UINT32 recved = 0;
	while(recved < size){
		recved += platform_queue_nbpop_data((char*)data + recved, size-recved);
	}
	return recved;
}

BOOL platform_queue_NBPop_UINT32(UINT32* data){
	return platform_queue_nbpop_data(data, sizeof(UINT32));
}
