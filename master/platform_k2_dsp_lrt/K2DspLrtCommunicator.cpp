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

#include "K2DspLrtCommunicator.h"

#include <stdlib.h>
#include <stdio.h>
#include <platform.h>
#include <lrt.h>
#include <qmss.h>

extern "C"{
#include <ti/drv/qmss/qmss_drv.h>
#include <cache.h>
}

#define CTRL_SPIDER_TO_LRT	(BASE_SPIDER_TO_LRT + Platform::get()->getLrt()->getIx())
#define CTRL_LRT_TO_SPIDER	(BASE_LRT_TO_SPIDER	+ Platform::get()->getLrt()->getIx())

static MonoPcktDesc* cur_mono_pkt_in;
static MonoPcktDesc* cur_mono_pkt_out;
static MonoPcktDesc* cur_mono_trace_out;

K2DspLrtCommunicator::K2DspLrtCommunicator(){
	cur_mono_pkt_in = 0;
	cur_mono_pkt_out = 0;
	cur_mono_trace_out = 0;
}

K2DspLrtCommunicator::~K2DspLrtCommunicator(){
}

void* K2DspLrtCommunicator::ctrl_start_send(int size){
	if(cur_mono_pkt_out != 0)
		throw "LrtCommunicator: Try to send a msg when previous one is not sent";

	while(cur_mono_pkt_out == 0){
		cur_mono_pkt_out = (MonoPcktDesc*)Qmss_queuePop(EMPTY_CTRL);
		if(cur_mono_pkt_out != 0){
			/* Initialize header */
			cur_mono_pkt_out->type_id = 0x2;
			cur_mono_pkt_out->packet_type = 0x10;
			cur_mono_pkt_out->packet_length = CTRL_DESC_SIZE;
			cur_mono_pkt_out->data_offset = 12;
			cur_mono_pkt_out->epib = 0;
			cur_mono_pkt_out->pkt_return_qnum = EMPTY_CTRL;
			cur_mono_pkt_out->src_tag_lo = 1; //copied to .flo_idx of streaming i/f
			break;
		}
//		usleep(100);
	}

	if(size > CTRL_DESC_SIZE - PACKET_HEADER)
		throw "LrtCommunicator: Try to send a message too big";

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((int)cur_mono_pkt_out) + PACKET_HEADER);
	return data_pkt;
}

void K2DspLrtCommunicator::ctrl_end_send(int size){
	if(cur_mono_pkt_out){
		/* Send the descriptor */
		cache_wbInvL1D(cur_mono_pkt_out, CTRL_DESC_SIZE);
		Qmss_queuePushDesc(CTRL_LRT_TO_SPIDER, (void*)cur_mono_pkt_out);

		cur_mono_pkt_out = 0;
	}else
		throw "LrtCommunicator: Try to send a free'd message";
}

int K2DspLrtCommunicator::ctrl_start_recv(void** data){
	if(cur_mono_pkt_in == 0){
		cur_mono_pkt_in = (MonoPcktDesc*)Qmss_queuePop(CTRL_SPIDER_TO_LRT);
		if(cur_mono_pkt_in == 0){
			return 0;
		}

		cache_invL1D(cur_mono_pkt_in, CTRL_DESC_SIZE);
	}else
		throw "LrtCommunicator: Try to receive a message when the previous one is not free'd";

	void* data_pkt = (void*)(((int)cur_mono_pkt_in)
			+ cur_mono_pkt_in->data_offset);
	int data_size = cur_mono_pkt_in->packet_length
			- cur_mono_pkt_in->data_offset;

	*data = data_pkt;
	return data_size;
}

void K2DspLrtCommunicator::ctrl_end_recv(){
	Qmss_queuePushDesc(EMPTY_CTRL, cur_mono_pkt_in);
	cur_mono_pkt_in = 0;
}

void* K2DspLrtCommunicator::trace_start_send(int size){
	if(cur_mono_trace_out != 0)
		throw "LrtCommunicator: Try to send a trace msg when previous one is not sent";

	while(cur_mono_trace_out == 0){
		cur_mono_trace_out = (MonoPcktDesc*)Qmss_queuePop(EMPTY_TRACE);
		if(cur_mono_trace_out != 0){
			/* Initialize header */
			cur_mono_trace_out->type_id = 0x2;
			cur_mono_trace_out->packet_type = 0x10;
			cur_mono_trace_out->packet_length = TRACE_DESC_SIZE;
			cur_mono_trace_out->data_offset = 12;
			cur_mono_trace_out->epib = 0;
			cur_mono_trace_out->pkt_return_qnum = EMPTY_TRACE;
			cur_mono_trace_out->src_tag_lo = 1; //copied to .flo_idx of streaming i/f
			break;
		}
//		usleep(100);
	}

	if(size > TRACE_DESC_SIZE - PACKET_HEADER)
		throw "LrtCommunicator: Try to send a trace message too big";

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((int)cur_mono_trace_out) + PACKET_HEADER);
	return data_pkt;
}

void K2DspLrtCommunicator::trace_end_send(int size){
	if(cur_mono_trace_out){
		/* Send the descriptor */
		cache_wbInvL1D(cur_mono_trace_out, TRACE_DESC_SIZE);
		Qmss_queuePushDesc(BASE_TRACE, cur_mono_trace_out);

		cur_mono_trace_out = 0;
	}else
		throw "SpiderCommunicator: Try to send a free'd message";
}

void K2DspLrtCommunicator::data_end_send(Fifo* f){
	if(f->ntoken){
		MonoPcktDesc *mono_pkt;
		int queueId = BASE_DATA+f->id;

		if(queueId < BASE_DATA || queueId > MAX_QUEUES )
			throw "Error: request queue out of bound\n";

		for(int i=0; i<f->ntoken; i++){
			do{
				mono_pkt = (MonoPcktDesc*)Qmss_queuePop(EMPTY_DATA);
			}while(mono_pkt == 0);

			mono_pkt->type_id = 0x2;
			mono_pkt->packet_type = 0;
			mono_pkt->data_offset = 12;
			mono_pkt->packet_length = 16;
			mono_pkt->epib = 0;
			mono_pkt->pkt_return_qnum = EMPTY_DATA;
			mono_pkt->src_tag_lo = 1; //copied to .flo_idx of streaming i/f

			cache_wbL1D(mono_pkt, DATA_DESC_SIZE);
			Qmss_queuePushDesc(queueId, mono_pkt);
			cache_wbL1D(Platform::get()->virt_to_phy((void*)(f->alloc)), f->size);
		}
	}
}
long K2DspLrtCommunicator::data_recv(Fifo* f){
	if(f->ntoken){
		MonoPcktDesc *mono_pkt;
		int queueId = BASE_DATA+f->id;

		for(int i=0; i<f->ntoken; i++){
			if(queueId < BASE_DATA || queueId > MAX_QUEUES )
				throw "Error: request queue out of bound\n";
			do{
				mono_pkt = (MonoPcktDesc*)Qmss_queuePop(queueId);
			}while(mono_pkt == 0);

			cache_invL1D(mono_pkt, DATA_DESC_SIZE);
			Qmss_queuePushDesc(EMPTY_DATA, mono_pkt);
			cache_invL1D(Platform::get()->virt_to_phy((void*)(f->alloc)), f->size);
		}
	}
	return (long)Platform::get()->virt_to_phy((void*)(f->alloc));
}

long K2DspLrtCommunicator::data_start_send(Fifo* f){
	return (long)Platform::get()->virt_to_phy((void*)(f->alloc));
}
