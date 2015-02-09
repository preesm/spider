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

#include "K2ArmSpiderCommunicator.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

#include <qmss.h>

extern "C"{
#include <ti/drv/qmss/qmss_drv.h>
}

#define CTRL_SPIDER_TO_LRT(id)	(BASE_SPIDER_TO_LRT + id)
#define CTRL_LRT_TO_SPIDER(id)	(BASE_LRT_TO_SPIDER	+ id)

static MonoPcktDesc* cur_mono_pkt_in[13];
static MonoPcktDesc* cur_mono_pkt_out[13];
static MonoPcktDesc* cur_mono_trace_in;
static MonoPcktDesc* cur_mono_trace_out;

K2ArmSpiderCommunicator::K2ArmSpiderCommunicator(){
	memset(cur_mono_pkt_in,    0, sizeof(cur_mono_pkt_in));
	memset(cur_mono_pkt_out,   0, sizeof(cur_mono_pkt_out));
	cur_mono_trace_in = 0;
	cur_mono_trace_out = 0;
}

K2ArmSpiderCommunicator::~K2ArmSpiderCommunicator(){
}

void* K2ArmSpiderCommunicator::ctrl_start_send(int lrtIx, int size){
	if(cur_mono_pkt_out[lrtIx] != 0)
		throw "SpiderCommunicator: Ctrl: Try to send a msg when previous one is not sent";

	while(cur_mono_pkt_out[lrtIx] == 0){
		cur_mono_pkt_out[lrtIx] = (MonoPcktDesc*)Qmss_queuePop(EMPTY_CTRL);
		if(cur_mono_pkt_out[lrtIx] != 0){
			/* Initialize header */
			cur_mono_pkt_out[lrtIx]->type_id = 0x2;
			cur_mono_pkt_out[lrtIx]->packet_type = 0x10;
			cur_mono_pkt_out[lrtIx]->packet_length = CTRL_DESC_SIZE;
			cur_mono_pkt_out[lrtIx]->data_offset = 12;
			cur_mono_pkt_out[lrtIx]->epib = 0;
			cur_mono_pkt_out[lrtIx]->pkt_return_qnum = EMPTY_CTRL;
			cur_mono_pkt_out[lrtIx]->src_tag_lo = 1; //copied to .flo_idx of streaming i/f
			break;
		}
		usleep(100);
	}

	if(size > CTRL_DESC_SIZE - PACKET_HEADER){
		printf("%d > %d\n", size, CTRL_DESC_SIZE - PACKET_HEADER);
		throw "SpiderCommunicator: Ctrl: Try to send a message too big";
	}

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((int)cur_mono_pkt_out[lrtIx]) + PACKET_HEADER);
	return data_pkt;
}

void K2ArmSpiderCommunicator::ctrl_end_send(int lrtIx, int size){
	if(cur_mono_pkt_out[lrtIx]){
		/* Send the descriptor */
		msync(cur_mono_pkt_out[lrtIx], CTRL_DESC_SIZE, MS_SYNC);
		Qmss_queuePushDesc(CTRL_SPIDER_TO_LRT(lrtIx), cur_mono_pkt_out[lrtIx]);

		cur_mono_pkt_out[lrtIx] = 0;
	}else
		throw "SpiderCommunicator: Ctrl: Try to send a free'd message";
}

int K2ArmSpiderCommunicator::ctrl_start_recv(int lrtIx, void** data){
	if(cur_mono_pkt_in[lrtIx] == 0){
		cur_mono_pkt_in[lrtIx] = (MonoPcktDesc*)Qmss_queuePop(CTRL_LRT_TO_SPIDER(lrtIx));
		if(cur_mono_pkt_in[lrtIx] == 0){
			return 0;
		}

		msync(cur_mono_pkt_in[lrtIx], CTRL_DESC_SIZE, MS_SYNC);
	}else
		throw "SpiderCommunicator: Ctrl: Try to receive a message when the previous one is not free'd";


	void* data_pkt = (void*)(((int)cur_mono_pkt_in[lrtIx])
			+ cur_mono_pkt_in[lrtIx]->data_offset);
	int data_size = cur_mono_pkt_in[lrtIx]->packet_length
			- cur_mono_pkt_in[lrtIx]->data_offset;

	*data = data_pkt;
	return data_size;
}

void K2ArmSpiderCommunicator::ctrl_end_recv(int lrtIx){
	Qmss_queuePushDesc(EMPTY_CTRL, cur_mono_pkt_in[lrtIx]);
	cur_mono_pkt_in[lrtIx] = 0;
}

void* K2ArmSpiderCommunicator::trace_start_send(int size){
	if(cur_mono_trace_out != 0)
		throw "SpiderCommunicator: Try to send a trace msg when previous one is not sent";

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
		usleep(100);
	}

	if(size > TRACE_DESC_SIZE - PACKET_HEADER)
		throw "SpiderCommunicator: Try to send a trace message too big";

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((int)cur_mono_trace_out) + PACKET_HEADER);
	return data_pkt;
}

void K2ArmSpiderCommunicator::trace_end_send(int size){
	if(cur_mono_trace_out){
		/* Send the descriptor */
		msync(cur_mono_trace_out, TRACE_DESC_SIZE, MS_SYNC);
		Qmss_queuePushDesc(BASE_TRACE, cur_mono_trace_out);

		cur_mono_trace_out = 0;
	}else
		throw "SpiderCommunicator: Try to send a free'd message";
}

int K2ArmSpiderCommunicator::trace_start_recv(void** data){
	if(cur_mono_trace_in == 0){
		cur_mono_trace_in = (MonoPcktDesc*)Qmss_queuePop(BASE_TRACE);
		if(cur_mono_trace_in == 0){
			return 0;
		}
		msync(cur_mono_trace_in, TRACE_DESC_SIZE, MS_SYNC);
	}else
		throw "SpiderCommunicator: Try to receive a trace message when the previous one is not free'd";


	void* data_pkt = (void*)(((int)cur_mono_trace_in)
			+ cur_mono_trace_in->data_offset);
	int data_size = cur_mono_trace_in->packet_length
			- cur_mono_trace_in->data_offset;

	*data = data_pkt;
	return data_size;
}

void K2ArmSpiderCommunicator::trace_end_recv(){
	Qmss_queuePushDesc(EMPTY_TRACE, cur_mono_trace_in);
	cur_mono_trace_in = 0;
}
