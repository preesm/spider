/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet,            *
 * Hugo Miomandre                                                           *
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

#include "MPPASpiderCommunicator.h"
#include <monitor/StackMonitor.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <platform.h>

#include <utask.h>

MPPASpiderCommunicator::MPPASpiderCommunicator(
		int msgSizeMax,
		int nLrt,
		void* fTraceWr,
		void* fTraceRd
	){

	fIn_ = (mppa_async_segment_t**) utask_smem_malloc(nLrt * sizeof(mppa_async_segment_t*));
	fOut_ = (mppa_async_segment_t**) utask_smem_malloc(nLrt * sizeof(mppa_async_segment_t*));
	fTraceRd_ = fTraceRd;
	fTraceWr_ = fTraceWr;

	msgSizeMax_ = msgSizeMax;

	msgBufferRecv_ = (void*) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
	curMsgSizeRecv_ = 0;
	msgBufferSend_ = (void*) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
	curMsgSizeSend_ = 0;

	ptr_temp_ = NULL;

	size = NULL;
	pending_dequeued_ = 0;
	traceBufferIx_ = 0;
}

MPPASpiderCommunicator::~MPPASpiderCommunicator(){

	utask_smem_free(fIn_);
	utask_smem_free(fOut_);

	StackMonitor::free(ARCHI_STACK, fIn_);
	StackMonitor::free(ARCHI_STACK, fOut_);
	StackMonitor::free(ARCHI_STACK, msgBufferRecv_);
	StackMonitor::free(ARCHI_STACK, msgBufferSend_);
}

void MPPASpiderCommunicator::setLrtCom(int lrtIx, mppa_async_segment_t *fIn, mppa_async_segment_t *fOut){
	#ifdef VERBOSE_SEGMENT_ID
	printf("MPPASpiderCommunicator::setLrtCom %d %p %p\n", lrtIx, fIn, fOut);
	#endif

	fIn_[lrtIx] = fIn;
	fOut_[lrtIx] = fOut;
}

void* MPPASpiderCommunicator::ctrl_start_send(int lrtIx, int size){

	if(curMsgSizeSend_){
		printf("SpiderCommunicator: Try to send a msg when previous one is not sent\n");
		throw "SpiderCommunicator: Try to send a msg when previous one is not sent\n";
	}
	//printf("MPPASpiderCommunicator::ctrl_start_send size %d to lrt %d\n",size,lrtIx);
	curMsgSizeSend_ = size;

	memcpy(msgBufferSend_, &curMsgSizeSend_ , sizeof(unsigned long));
	return &((char*)msgBufferSend_)[sizeof(unsigned long)];
}

void MPPASpiderCommunicator::ctrl_end_send(int lrtIx, int size){
	//unsigned long s = curMsgSizeSend_;


	if(curMsgSizeSend_ > (unsigned long)10240){
		printf("IO%d : MPPASpiderCommunicator try send Msg too big with %lu max is %d\n",__k1_get_cpu_id(), curMsgSizeSend_, 10240);
		while(1);
	}


	if(mppa_async_enqueue(fOut_[lrtIx], msgBufferSend_, sizeof(unsigned long) + curMsgSizeSend_, 0, NULL) != 0)
	{
		printf("MPPASpiderCommunicator::ctrl_end_send mppa_async_enqueue failed\n");
		while(1);
	}

	// printf("Spider to %d : Writing out %p\n",lrtIx,fOut_[lrtIx]);

	curMsgSizeSend_ = 0;
}

int MPPASpiderCommunicator::ctrl_start_recv(int lrtIx, void** data){

	//static unsigned int *size = 0;
	static int pending_dequeued_ = 0;
	static mppa_async_event_t event_;


	if(pending_dequeued_ == 0)
	{
		if(mppa_async_dequeue(fIn_[lrtIx], 0, (void**) &ptr_temp_, &event_) != 0)
		{
			printf("MPPASpiderCommunicator::ctrl_start_recv mppa_async_dequeue 1 failed\n");
			while(1);
		}
		pending_dequeued_ = 1;
	}


	if(mppa_async_event_test(&event_) == 0)
	{
		pending_dequeued_ = 0;	/* has something */
	}else{
		return 0; /* empty */
	}


	//checking size wraparound
	check_segment_buffer_wraparound(fIn_[lrtIx], ptr_temp_, sizeof(unsigned long), QUEUE_LRT_TO_SPIDER_SIZE, &curMsgSizeRecv_);

	//check if receive size is realistic
	if(curMsgSizeRecv_ > (unsigned long)msgSizeMax_){
			printf("IO%d : MPPASpiderCommunicator Msg too big with %lu max is %d\n",__k1_get_cpu_id(), curMsgSizeRecv_, msgSizeMax_);
			while(1);
	}

	//checking message wraparound
	check_segment_buffer_wraparound(fIn_[lrtIx], &ptr_temp_[sizeof(unsigned long)], curMsgSizeRecv_, QUEUE_LRT_TO_SPIDER_SIZE, msgBufferRecv_);

	mppa_async_discard(fIn_[lrtIx], sizeof(unsigned long) + curMsgSizeRecv_);

	*data = msgBufferRecv_;
	return curMsgSizeRecv_;
}

void MPPASpiderCommunicator::ctrl_end_recv(int lrtIx){
	curMsgSizeRecv_ = 0;
}

void* MPPASpiderCommunicator::trace_start_send(int size){
	//printf("IO%d : MPPASpiderCommunicator::trace_start_send called\n",__k1_get_cpu_id());

	if(curMsgSizeSend_){
		printf("SpiderCommunicator: Try to send a msg when previous one is not sent\n");
		throw "SpiderCommunicator: Try to send a msg when previous one is not sent\n";
	}
	curMsgSizeSend_ = size;
	return msgBufferSend_;
}

void MPPASpiderCommunicator::trace_end_send(int size){
long long index = __builtin_k1_afdau(&traceBufferIx_,curMsgSizeSend_);

	memcpy((char*) fTraceWr_ + index,msgBufferSend_,curMsgSizeSend_);
	__builtin_k1_wpurge(); // write memory barrier

	curMsgSizeSend_ = 0;
}

int MPPASpiderCommunicator::trace_start_recv(void** data){
	
	//lire des morceaux de n bytes jusqu'a different de 0xFF
	static int indice = 0;
	while((*((char*)fTraceRd_ + indice*sizeof(TraceMsg))) == -1) indice++;

	if(indice*sizeof(TraceMsg) >= (1000*1000* 32) * (16+1+1)  ){
		printf("Trace buffer overflow\n");
		while(1);
	}else{
		if(indice%100 == 0){
			printf("cid %d indice indice %d\n", __k1_get_cluster_id(), indice);
		}
	}

	curMsgSizeRecv_ = sizeof(TraceMsg);

	//making a local copy
	memcpy(msgBufferRecv_,(char*) fTraceRd_ + indice*sizeof(TraceMsg),curMsgSizeRecv_);

	//erasing content
	memset((char*) fTraceRd_ + indice*sizeof(TraceMsg),-1,curMsgSizeRecv_);

	//memory fence
	__builtin_k1_wpurge();
	__builtin_k1_fence();

	*data = msgBufferRecv_;
	return curMsgSizeRecv_;
}

void MPPASpiderCommunicator::trace_end_recv(){
	//printf("IO%d : MPPASpiderCommunicator::trace_end_recv called\n",__k1_get_cpu_id());
	curMsgSizeRecv_ = 0;
}


void MPPASpiderCommunicator::check_segment_buffer_wraparound(mppa_async_segment_t* segment, void* ptr_dequeue, int size, unsigned long buffer_size, void* out_ptr)
{
	int size_diff = ((uintptr_t)ptr_dequeue + size) - (buffer_size + (uintptr_t)segment->phys_base);
	if(size_diff > 0) // to be check
	{
		printf("IO%d : Wraparound in Spider\n",__k1_get_cpu_id());
		if(size_diff == 0)
		{
			printf("size_diff is 0 check...\n");
		}
		/* wrap */
		char *ptr = (char*)out_ptr;
		memcpy(ptr, ptr_dequeue, size - size_diff);
		memcpy(&ptr[size - size_diff], (void*)segment->phys_base, size_diff);
	}else{
		memcpy(out_ptr, ptr_dequeue, size);
	}
}
