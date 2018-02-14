/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2015 - 2016) :
 *
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2015 - 2016)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#include "K2ArmSpiderCommunicator.h"

#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <algorithm>

extern "C"{
#include <ti/drv/qmss/qmss_drv.h>
#include <ti/drv/cppi/cppi_drv.h>
#include <ti/drv/cppi/cppi_desc.h>
#include "init.h"
}

#define CTRL_SPIDER_TO_LRT(id)	(QUEUE_CTRL_DOWN_0 + id)
#define CTRL_LRT_TO_SPIDER(id)	(QUEUE_CTRL_UP_0   + id)

static Cppi_Desc* 	cur_mono_pkt_in[13];
static int 			cur_mono_pkt_in_size[13];
static Cppi_Desc* 	cur_mono_pkt_out[13];
static int 			cur_mono_pkt_out_size[13];
static Cppi_Desc* 	cur_mono_trace_in;
static int 			cur_mono_trace_in_size;
static Cppi_Desc* 	cur_mono_trace_out;
static int 			cur_mono_trace_out_size;

K2ArmSpiderCommunicator::K2ArmSpiderCommunicator(){
	memset(cur_mono_pkt_in,    		0, sizeof(cur_mono_pkt_in));
	memset(cur_mono_pkt_in_size,   	0, sizeof(cur_mono_pkt_in_size));
	memset(cur_mono_pkt_out,   		0, sizeof(cur_mono_pkt_out));
	memset(cur_mono_pkt_out_size,   0, sizeof(cur_mono_pkt_out_size));
	cur_mono_trace_in 		= 0;
	cur_mono_trace_in_size 	= 0;
	cur_mono_trace_out 		= 0;
	cur_mono_trace_out_size = 0;
	maxCtrlMsgSize = 0;
}

K2ArmSpiderCommunicator::~K2ArmSpiderCommunicator(){
	printf("Spider Ctrl msg Max: %d / %d\n", maxCtrlMsgSize, CTRL_DESC_SIZE);
}

void* K2ArmSpiderCommunicator::ctrl_start_send(int lrtIx, int size){
	int dataOffset = 0;

	maxCtrlMsgSize = std::max(maxCtrlMsgSize,size);

	if(cur_mono_pkt_out[lrtIx] != 0)
		throw "SpiderCommunicator: Ctrl: Try to send a msg when previous one is not sent";

	while(cur_mono_pkt_out[lrtIx] == 0){
		cur_mono_pkt_out[lrtIx] = (Cppi_Desc*)Qmss_queuePop(QUEUE_FREE_CTRL);

		if(cur_mono_pkt_out[lrtIx] != 0){
			/* Get Packet info */
			cur_mono_pkt_out_size[lrtIx]  = CTRL_DESC_SIZE;//QMSS_DESC_SIZE(cur_mono_pkt_out[lrtIx]);
			cur_mono_pkt_out[lrtIx] = (Cppi_Desc*)QMSS_DESC_PTR (cur_mono_pkt_out[lrtIx]);

			/* Clear Cache */
			Osal_qmssBeginMemAccess(cur_mono_pkt_out[lrtIx], cur_mono_pkt_out_size[lrtIx]);

			/* Get info */
			dataOffset = Cppi_getDataOffset(Cppi_DescType_MONOLITHIC, cur_mono_pkt_out[lrtIx]);
			break;
		}
		usleep(100);
	}

	if(size > cur_mono_pkt_out_size[lrtIx] - dataOffset){
		printf("%d > %d\n", size, cur_mono_pkt_out_size[lrtIx] - dataOffset);
		throw "SpiderCommunicator: ctrl_start_send: Try to send a message too big";
	}

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((int)cur_mono_pkt_out[lrtIx]) + dataOffset);
	return data_pkt;
}

void K2ArmSpiderCommunicator::ctrl_end_send(int lrtIx, int size){
	if(cur_mono_pkt_out[lrtIx]){
		/* Send the descriptor */
		Osal_qmssEndMemAccess(cur_mono_pkt_out[lrtIx], cur_mono_pkt_out_size[lrtIx]);
		Qmss_queuePushDesc(CTRL_SPIDER_TO_LRT(lrtIx), (void*)cur_mono_pkt_out[lrtIx]);

		cur_mono_pkt_out[lrtIx] = 0;
		cur_mono_pkt_out_size[lrtIx] = 0;
	}else
		throw "SpiderCommunicator: Try to send a free'd message";
}

int K2ArmSpiderCommunicator::ctrl_start_recv(int lrtIx, void** data){
	int dataOffset;

	if(cur_mono_pkt_in[lrtIx] == 0){
		cur_mono_pkt_in[lrtIx] = (Cppi_Desc*)Qmss_queuePop(CTRL_LRT_TO_SPIDER(lrtIx));
		if(cur_mono_pkt_in[lrtIx] == 0){
			return 0;
		}

		/* Get Packet info */
		cur_mono_pkt_in_size[lrtIx] = CTRL_DESC_SIZE;//QMSS_DESC_SIZE(cur_mono_pkt_in[lrtIx]);
		cur_mono_pkt_in[lrtIx] = (Cppi_Desc*)QMSS_DESC_PTR (cur_mono_pkt_in[lrtIx]);

		/* Clear Cache */
		Osal_qmssBeginMemAccess(cur_mono_pkt_in[lrtIx], cur_mono_pkt_in_size[lrtIx]);

		/* Get info */
		dataOffset = Cppi_getDataOffset(Cppi_DescType_MONOLITHIC, cur_mono_pkt_in[lrtIx]);
	}else
		throw "SpiderCommunicator: Ctrl: Try to receive a message when the previous one is not free'd";

	void* data_pkt = (void*)(((int)cur_mono_pkt_in[lrtIx]) + dataOffset);
	int data_size = cur_mono_pkt_in_size[lrtIx] - dataOffset;

	*data = data_pkt;
	return data_size;
}

void K2ArmSpiderCommunicator::ctrl_end_recv(int lrtIx){
	if(cur_mono_pkt_in[lrtIx]){
		/* Send the descriptor */
		Osal_qmssEndMemAccess(cur_mono_pkt_in[lrtIx], cur_mono_pkt_in_size[lrtIx]);
		Qmss_queuePushDesc(QUEUE_FREE_CTRL, cur_mono_pkt_in[lrtIx]);

		cur_mono_pkt_in[lrtIx] = 0;
		cur_mono_pkt_in_size[lrtIx] = 0;
	}else
		throw "SpiderCommunicator: Try to send a free'd message";
}

void* K2ArmSpiderCommunicator::trace_start_send(int size){
	int dataOffset = 0;

	if(cur_mono_trace_out != 0)
		throw "SpiderCommunicator: Try to send a trace msg when previous one is not sent";

	while(cur_mono_trace_out == 0){
		cur_mono_trace_out = (Cppi_Desc*)Qmss_queuePop(QUEUE_FREE_TRACE);

		if(cur_mono_trace_out != 0){
			/* Get Packet info */
			cur_mono_trace_out_size  = TRACE_DESC_SIZE;//QMSS_DESC_SIZE(cur_mono_trace_out);
			cur_mono_trace_out = (Cppi_Desc*)QMSS_DESC_PTR (cur_mono_trace_out);

			/* Clear Cache */
			Osal_qmssBeginMemAccess(cur_mono_trace_out, cur_mono_trace_out_size);

			/* Get info */
			dataOffset = Cppi_getDataOffset(Cppi_DescType_MONOLITHIC, cur_mono_trace_out);
			break;
		}
		usleep(100);
	}

	if(size > cur_mono_trace_out_size - dataOffset)
		throw "SpiderCommunicator: Try to send a trace message too big";

	/* Add data to current descriptor */
	void* data_pkt = (void*)(((int)cur_mono_trace_out) + dataOffset);
	return data_pkt;
}

void K2ArmSpiderCommunicator::trace_end_send(int size){
	if(cur_mono_trace_out){
		/* Send the descriptor */
		Osal_qmssEndMemAccess(cur_mono_trace_out, cur_mono_trace_out_size);
		Qmss_queuePushDesc(QUEUE_TRACE, cur_mono_trace_out);

		cur_mono_trace_out = 0;
		cur_mono_trace_out_size = 0;
	}else
		throw "SpiderCommunicator: Try to send a free'd message";
}

int K2ArmSpiderCommunicator::trace_start_recv(void** data){
	int dataOffset;

	if(cur_mono_trace_in == 0){
		cur_mono_trace_in = (Cppi_Desc*)Qmss_queuePop(QUEUE_TRACE);
		if(cur_mono_trace_in == 0){
			return 0;
		}

		/* Get Packet info */
		cur_mono_trace_in_size = TRACE_DESC_SIZE;//QMSS_DESC_SIZE(cur_mono_trace_in);
		cur_mono_trace_in = (Cppi_Desc*)QMSS_DESC_PTR (cur_mono_trace_in);

		/* Clear Cache */
		Osal_qmssBeginMemAccess(cur_mono_trace_in, cur_mono_trace_in_size);

		/* Get info */
		dataOffset = Cppi_getDataOffset(Cppi_DescType_MONOLITHIC, cur_mono_trace_in);
	}else
		throw "SpiderCommunicator: Ctrl: Try to receive a message when the previous one is not free'd";

	void* data_pkt = (void*)(((int)cur_mono_trace_in) + dataOffset);
	int data_size = cur_mono_trace_in_size - dataOffset;

	*data = data_pkt;
	return data_size;
}

void K2ArmSpiderCommunicator::trace_end_recv(){
	if(cur_mono_trace_in){
		/* Send the descriptor */
		Osal_qmssEndMemAccess(cur_mono_trace_in, cur_mono_trace_in_size);
		Qmss_queuePushDesc(QUEUE_FREE_TRACE, cur_mono_trace_in);

		cur_mono_trace_in = 0;
		cur_mono_trace_in_size = 0;
	}else
		throw "SpiderCommunicator: Try to send a free'd message";
}
