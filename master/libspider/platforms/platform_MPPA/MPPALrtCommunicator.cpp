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

#include "MPPALrtCommunicator.h"

#include <unistd.h>

#include <fcntl.h>
#include <cstdlib>
#include <stdio.h>
#include <stdint.h>

#include <platform.h>

#include <monitor/StackMonitor.h>

#include <mppa_async.h>
#include <utask.h>

#define LONG_MAX 0x7FFFFFFF

long long MPPALrtCommunicator::lock = 1;

long long MPPALrtCommunicator::nb_ongoing_job_ = 0;

long long MPPALrtCommunicator::traceBufferIx_ = 0;

MPPALrtCommunicator::MPPALrtCommunicator(
		int msgSizeMax,
		mppa_async_segment_t* fIn,
		mppa_async_segment_t* fOut,
		void* notif_tab,
		void* traceBuffer,
		mppa_async_segment_t* notif_segment,
		mppa_async_segment_t* jobTab_segment,
		mppa_async_segment_t* dataMem_segment,
		mppa_async_segment_t* trace_rdma_segment,
		PlatformMPPA* platform
	){
	fIn_ = fIn;
	fOut_ = fOut;

	msgSizeMax_ = msgSizeMax;

	msgBufferRecv_ = (void*) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
	curMsgSizeRecv_ = 0;

	msgBufferSend_ = (void*) CREATE_MUL(ARCHI_STACK, sizeof(ParamValueMsg)+MAX_OUT_PARAM*sizeof(Param), char);
	curMsgSizeSend_ = 0;

	notif_tab_ = (Notif_t*)notif_tab;
	traceBuffer_ = traceBuffer;

	pending_dequeued_ = 0;
	size_receive_ = NULL;
	ptr_temp_ = NULL;

	notif_segment_ = notif_segment;
	jobTab_segment_ = jobTab_segment;
	dataMem_segment_ = dataMem_segment;
	trace_rdma_segment_ = trace_rdma_segment;

	nb_inFifos_ = 0;
	nb_outFifos_ = 0;

	input_flushed_ = 0;

	#ifdef VERBOSE
	wrap_count_ = 0;
	#endif

	traceBufferIx_ = 0;

	active_state_self_ = false;

	true_value_ = 1;

	mppa_async_evalcond(&true_value_, 0, MPPA_ASYNC_COND_GE, &event_trace_);

	mppa_async_evalcond(&true_value_, 0, MPPA_ASYNC_COND_GE, &event_data_);

	platform_ = platform;

	temp_peek_ptr_ = NULL;

	peek_event_ = NULL;

	peek_sent_ = 0;
}

MPPALrtCommunicator::~MPPALrtCommunicator(){
	#ifdef __k1io__
	utask_smem_free(temp_peek_ptr_);
	#else
	free(temp_peek_ptr_);
	#endif

	StackMonitor::free(ARCHI_STACK, msgBufferRecv_);
	StackMonitor::free(ARCHI_STACK, msgBufferSend_);
}

void* MPPALrtCommunicator::ctrl_start_send(int size){
	if(curMsgSizeSend_){
		printf("LrtCommunicator: Try to send a msg when previous one is not sent\n");
		throw "LrtCommunicator: Try to send a msg when previous one is not sent\n";
	}
	curMsgSizeSend_ = size;

	memcpy(msgBufferSend_, &curMsgSizeSend_ , sizeof(unsigned long));

	return &((char*)msgBufferSend_)[sizeof(unsigned long)];
}

void MPPALrtCommunicator::ctrl_end_send(int size){
	unsigned long s = curMsgSizeSend_;
	

	if(mppa_async_enqueue(fOut_, msgBufferSend_, sizeof(unsigned long) + curMsgSizeSend_, 0, NULL) != 0)
	{
		printf("MPPALrtCommunicator::ctrl_end_send mppa_async_enqueue failed\n");
		while(1);
	}

	curMsgSizeSend_ = 0;
}


int MPPALrtCommunicator::ctrl_start_recv(void** data){

	if(pending_dequeued_ == 0)
	{
		//printf("cid %d : Reading from %p\n",__k1_get_cluster_id(),fIn_);
		if(mppa_async_dequeue(fIn_, 0, (void**) &ptr_temp_, &event_) != 0)
		{
			printf("MPPALrtCommunicator::ctrl_start_recv mppa_async_dequeue 1 failed\n");
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
	check_segment_buffer_wraparound(fIn_, ptr_temp_, sizeof(unsigned long), QUEUE_SPIDER_TO_LRT_SIZE, &curMsgSizeRecv_);


	//check if receive size is realistic
	if(curMsgSizeRecv_ > (unsigned long)msgSizeMax_){
			printf("Cluster %d  PE %d: MPPALrtCommunicator Msg too big with %d max is %d\n", __k1_get_cluster_id(), __k1_get_cpu_id(), curMsgSizeRecv_, msgSizeMax_);
			//throw "Msg too big\n";
			while(1);
	}


	//checking message wraparound
	check_segment_buffer_wraparound(fIn_, &ptr_temp_[sizeof(unsigned long)], curMsgSizeRecv_, QUEUE_SPIDER_TO_LRT_SIZE, msgBufferRecv_);

	mppa_async_discard(fIn_, sizeof(unsigned long) + curMsgSizeRecv_);


	*data = msgBufferRecv_;
	return curMsgSizeRecv_;
}

void MPPALrtCommunicator::ctrl_end_recv(){
	curMsgSizeRecv_ = 0;
}

void* MPPALrtCommunicator::trace_start_send(int size){
	mppa_async_event_wait(&event_trace_);
	if(curMsgSizeSend_){
		printf("LrtCommunicator: Try to send a msg when previous one is not sent\n");
		throw "LrtCommunicator: Try to send a msg when previous one is not sent\n";
	}
	curMsgSizeSend_ = size;
	return msgBufferSend_;
}

void MPPALrtCommunicator::trace_end_send(int size){
	long long index = __builtin_k1_afdau(&traceBufferIx_,curMsgSizeSend_);
#ifdef __k1io__

	memcpy((char*) traceBuffer_ + index,msgBufferSend_,curMsgSizeSend_);
	__builtin_k1_wpurge(); // write memory barrier

#else

	off64_t offset = index;

	if(mppa_async_put(msgBufferSend_, trace_rdma_segment_, offset, curMsgSizeSend_, &event_trace_) != 0)
	{
		printf("[Cluster %d] failed to sned to buffer address %p size %d\n", __k1_get_cluster_id(), index, curMsgSizeSend_);
		while(1);
	}

#endif
	curMsgSizeSend_ = 0;
}

uintptr_t MPPALrtCommunicator::data_recv(Fifo* f){
#ifdef __k1io__
	// void *address =  Platform::get()->virt_to_phy( (void*)(uintptr_t)(f->alloc) ) ;
	// printf("cid %d pid %d recv offset 0x%llx size %d\n", __k1_get_cluster_id(), __k1_get_cpu_id(), (uint64_t)(uintptr_t)address, f->size);
	return (uintptr_t)Platform::get()->virt_to_phy((void*)(intptr_t)(f->alloc));
#else

	off64_t offset = f->alloc;

	// void *address = NULL;
	// mppa_async_address(dataMem_segment_, offset, &address);
	// printf("cid %d pid %d recv offset 0x%llx size %d\n", __k1_get_cluster_id(), __k1_get_cpu_id(), (uint64_t) (uintptr_t) address, f->size);


	if(mppa_async_get(&input_local_buffer_[nb_inFifos_], dataMem_segment_, offset, f->size, &event_data_) != 0)
	{
		printf("[Cluster %d] failed to retrieve buffer address %p size %d\n", __k1_get_cluster_id(), f->alloc, f->size);
		while(1);
	}
	return (uintptr_t) &input_local_buffer_[nb_inFifos_++];
#endif
}

uintptr_t MPPALrtCommunicator::data_start_send(Fifo* f){
#ifdef __k1io__
	// void *address =  Platform::get()->virt_to_phy( (void*)(uintptr_t)(f->alloc) ) ;
	// printf("cid %d pid %d send offset 0x%llx size %d\n", __k1_get_cluster_id(), __k1_get_cpu_id(), (uint64_t)(uintptr_t)address, f->size);
	return (uintptr_t) Platform::get()->virt_to_phy((void*)(uintptr_t)(f->alloc));
#else

	mppa_async_event_wait(&event_data_);

	return (uintptr_t) &output_local_buffer_[nb_outFifos_++];
#endif
}

void MPPALrtCommunicator::data_end_send(Fifo* f){
#ifdef __k1io__
	// Nothing to do
#else
	// Freeing input buffers if not already done, only done the first time
	if(input_flushed_ == 0){
	 	input_flushed_ = 1;

		nb_outFifos_ = 0;
	}


	int size = f->size;
	off64_t offset = f->alloc;

	// void *address;
	// mppa_async_address(dataMem_segment_, offset, &address);
	// printf("cid %d pid %d send offset 0x%llx size %d\n", __k1_get_cluster_id(), __k1_get_cpu_id(), (uint64_t) (uintptr_t) address, f->size);

	if(mppa_async_put(&output_local_buffer_[nb_outFifos_++], dataMem_segment_, offset, size, &event_data_) != 0)
	{
		printf("[Cluster %d] failed to send to buffer address %p size %d\n", __k1_get_cluster_id(), f->alloc, size);
		while(1);
	}
#endif
}

void MPPALrtCommunicator::allocateDataBuffer(int nbInput, Fifo* fIn, int nbOutput, Fifo* fOut){
#ifdef __k1io__

#else

	/* alloc sequence */
	size_t in_size = 0;
	for (int i = 0; i < nbInput; i++) in_size += fIn[i].size;

	size_t out_size = 0;
	for (int i = 0; i < nbOutput; i++) out_size += fOut[i].size;


	do{
		while(__builtin_k1_aldcu(&lock) != 1); // lock

		long long count = __builtin_k1_ldu(&nb_ongoing_job_);

		input_local_buffer_ = (void**) malloc(nbInput * sizeof(void*) + in_size + nbOutput * sizeof(void*) + out_size);

		/* alloc failed */
		if(input_local_buffer_ == NULL ){

			if(count == 0){
				printf("[Cluster %d] PE%d : Deadlock detected when trying to allocate %d bytes\n",__k1_get_cluster_id(),__k1_get_cpu_id(),in_size+out_size);
				throw "Deadlock";
			}else{
				active_state_self_ = false;
			}
		}else{
			active_state_self_ = true;
			__builtin_k1_afdau(&nb_ongoing_job_, 1);
		}
		__builtin_k1_wpurge();
		__builtin_k1_fence();
		__builtin_k1_sdu(&lock, 1); // unlock

	}while(active_state_self_ == false);

	//all allocs were successfull


	in_size = 0;
	for (int i = 0; i < nbInput; i++){

		input_local_buffer_[i] = (char*) input_local_buffer_ + (nbInput * sizeof(void*)) + in_size;

		in_size += fIn[i].size;
	}

	output_local_buffer_ = (void**) ((char*) input_local_buffer_ + (nbInput * sizeof(void*)) + in_size);

	out_size = 0;
	for (int i = 0; i < nbOutput; i++){

		output_local_buffer_[i] = (char*) output_local_buffer_ + (nbOutput * sizeof(void*)) + out_size;

		out_size += fOut[i].size;
	}


	input_flushed_ = 0;
	nb_inFifos_ = 0;
	nb_outFifos_ = 0;
#endif
}


void MPPALrtCommunicator::freeDataBuffer(int nbInput, int nbOutput){
#ifdef __k1io__

#else
	mppa_async_event_wait(&event_data_);

	while(__builtin_k1_aldcu(&lock) != 1); // lock

	free(input_local_buffer_);

	__builtin_k1_afdau(&nb_ongoing_job_, -1);
	__builtin_k1_wpurge();
	__builtin_k1_fence();
	__builtin_k1_sdu(&lock, 1); // unlock

#endif
}


void MPPALrtCommunicator::setLrtJobIx(int lrtIx, int jobIx){

	__builtin_k1_sdu(&notif_tab_[__k1_get_cpu_id()].jobIx,(long long) jobIx);

	//printf("cid %d pe %d writing local_buffer %d lrtIx %d\n", __k1_get_cluster_id(), __k1_get_cpu_id(), jobIx, lrtIx);
#ifdef __k1io__

	platform_->setLrtJobIx(lrtIx, jobIx);

#else
	off64_t offset = lrtIx*sizeof(long long);

	if (mppa_async_poke(jobTab_segment_, offset, (long long)jobIx))
	{
		printf("[Cluster %d] failed to set value at offset %lld size %d\n", __k1_get_cluster_id(), offset, sizeof(long long));
		while(1);
	}
#endif
}


long MPPALrtCommunicator::getLrtJobIx(int lrtIx){


#ifdef __k1io__
	return platform_->getLrtJobIx(lrtIx);

#else
	long long local_buffer = 0;

	off64_t offset = lrtIx*sizeof(long long);
	
	if (mppa_async_peek(jobTab_segment_, offset, &local_buffer, NULL))
	{
		printf("[Cluster %d] failed to retrieve value at address %p size %d\n", __k1_get_cluster_id(), lrtIx, sizeof(int));
		while(1);
	}

	//printf("cid %d pe %d reading... local_buffer %d lrtIx %d\n", __k1_get_cluster_id(), __k1_get_cpu_id(), local_buffer, lrtIx);

	return local_buffer;
#endif
}

void MPPALrtCommunicator::rstLrtJobIx(int lrtIx){

	__builtin_k1_sdu(&notif_tab_[__k1_get_cpu_id()].jobIx,0);
}



void MPPALrtCommunicator::waitForLrtUnlock(int nbDependency, int* blkLrtIx, int* blkLrtJobIx, int jobIx){

	if (nbDependency == 0) return;

	#if __k1io__
	register int currentClusterID = platform_->getNbCluster();
	#else
	register int currentClusterID = __k1_get_cluster_id();
	#endif


	long long* peek_response = NULL;
	int* targetCID = NULL;
	int* targetPID = NULL;


	if (temp_peek_ptr_ != NULL){

		mppa_async_event_waitall(peek_sent_, peek_event_);

		#ifdef __k1io__
		utask_smem_free(temp_peek_ptr_);
		#else
		free(temp_peek_ptr_);
		#endif
		
		temp_peek_ptr_ = NULL;	
	}

	peek_sent_ = 0;


	#ifdef __k1io__
	while ((temp_peek_ptr_ = (void*) utask_smem_calloc(nbDependency, sizeof(*peek_response) + sizeof(*targetCID) + sizeof(*targetPID) + sizeof(*peek_event_))) == NULL);
	#else
	while ((temp_peek_ptr_ = (void*) calloc(nbDependency, sizeof(*peek_response) + sizeof(*targetCID) + sizeof(*targetPID) + sizeof(*peek_event_))) == NULL);
	#endif
	__builtin_k1_wpurge();
	__builtin_k1_fence();
	peek_response = (long long*) ((char*) temp_peek_ptr_);
	targetCID = (int*) ((char*) peek_response + sizeof(*peek_response) * nbDependency);
	targetPID = (int*) ((char*) targetCID + sizeof(*targetCID) * nbDependency);
	peek_event_ = (mppa_async_event_t*) ((char*) targetPID + sizeof(*targetPID) * nbDependency);


	off64_t offset;

	for(int i = 0; i < nbDependency; i++){

		// printf("[Cluster %d] PE%d : sending notif %d to lrt %d at job %d\n",__k1_get_cluster_id(),__k1_get_cpu_id(),i,blkLrtIx[i],blkLrtJobIx[i]);

		int targetID = platform_->getThreadNumber(blkLrtIx[i]);

		targetCID[i] = targetID/16;
		targetPID[i] = targetID%16;

		if(targetCID[i] >= 128) targetCID[i] = platform_->getNbCluster();

		// on regarde si on a déjà envoyé une notif à ce targetID
		while((__builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[targetCID[i]/CLUSTER_PER_BITMASK]) & (1ULL << ((targetCID[i]%CLUSTER_PER_BITMASK)*16 + targetPID[i]))) != 0) unlockLrt(jobIx);

		//on enregistre l'envoie
		__builtin_k1_afdau(&notif_tab_[__k1_get_cpu_id()].semaphore[targetCID[i]/CLUSTER_PER_BITMASK],1ULL << ((targetCID[i]%CLUSTER_PER_BITMASK)*16 + targetPID[i]));
		

		if (targetCID[i] == currentClusterID)
		{
			// if locally
			__builtin_k1_swu(&notif_tab_[targetPID[i]].unlockJobIx[currentClusterID*16 + __k1_get_cpu_id()],blkLrtJobIx[i]);
			__builtin_k1_afdau(&notif_tab_[targetPID[i]].bitmask[currentClusterID/CLUSTER_PER_BITMASK],1ULL << ((currentClusterID%CLUSTER_PER_BITMASK)*16 + __k1_get_cpu_id()));
			continue;
		}
		

		offset = targetPID[i]*sizeof(Notif_t) + sizeof(notif_tab_[0].bitmask) + (currentClusterID*16 + __k1_get_cpu_id())*sizeof(notif_tab_[0].unlockJobIx[0]);
		// printf("[Cluster %d] PE%d : Sending blkLrtJobIx[%d] %d at CC%d PE%d with offset %lld\n",__k1_get_cluster_id(),__k1_get_cpu_id(),i,blkLrtJobIx[i],targetCID[i],targetPID[i],offset);

		if (mppa_async_poke(&notif_segment_[targetCID[i]], offset, (long long)blkLrtJobIx[i])){
			printf("[Cluster %d] PE%d : Fail in waitForLrtUnlock, mppa_async_poke\n",__k1_get_cluster_id(),__k1_get_cpu_id(), offset);
			while(1);
		}

		offset = targetPID[i]*sizeof(Notif_t) + (currentClusterID/CLUSTER_PER_BITMASK) * sizeof(notif_tab_[0].bitmask[0]);
		// printf("[Cluster %d] PE%d : Sending notif[%d] 0x%llX at CC%d PE%d with offset %lld\n",__k1_get_cluster_id(),__k1_get_cpu_id(),i,1ULL << ((currentClusterID%CLUSTER_PER_BITMASK)*16 + __k1_get_cpu_id()),targetCID[i],targetPID[i],offset);

		if (mppa_async_postadd(&notif_segment_[targetCID[i]], offset, 1ULL << ((currentClusterID%CLUSTER_PER_BITMASK)*16 + __k1_get_cpu_id()))){
			printf("[Cluster %d] PE%d : Fail in waitForLrtUnlock, mppa_async_postadd\n",__k1_get_cluster_id(),__k1_get_cpu_id(), offset);
			while(1);
		}
	}

	for(int i = 0; i < nbDependency; i++){
		// Lancement d'un peek asynchrone

		if (targetCID[i] == currentClusterID)
		{
			// if locally
			__builtin_k1_sdu(&peek_response[i],__builtin_k1_ldu(&notif_tab_[targetPID[i]].jobIx));
			continue;
		}

		offset = targetPID[i]*sizeof(Notif_t) + sizeof(notif_tab_[0].bitmask) + sizeof(notif_tab_[0].unlockJobIx) + sizeof(notif_tab_[0].semaphore);
		
		if (mppa_async_peek(&notif_segment_[targetCID[i]], offset, &peek_response[i], &peek_event_[peek_sent_++])){
			printf("[Cluster %d] PE%d : Fail in waitForLrtUnlock, mppa_async_peek\n",__k1_get_cluster_id(),__k1_get_cpu_id(), offset);
			while(1);
		}
	}

	//WAITING FOR ANSWERS FROM OTHER LRT TO CONTINUE

	// printf("[Cluster %d] PE%d is locked\n",__k1_get_cluster_id(),__k1_get_cpu_id());

	memset(&notif_tab_[__k1_get_cpu_id()].peek_semaphore[0],0,sizeof(notif_tab_[__k1_get_cpu_id()].peek_semaphore));
	__builtin_k1_wpurge();
	__builtin_k1_fence();

	int pending_notif;

	do{
		unlockLrt(jobIx);

		register long long a0 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[0]);
		register long long a1 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[1]);
		register long long a2 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[2]);
		register long long a3 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[3]);
		register long long a4 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[4]);
		register long long a5 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[5]);
		register long long a6 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[6]);
		register long long a7 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[7]);
		register long long a8 = __builtin_k1_ldu(&notif_tab_[__k1_get_cpu_id()].semaphore[8]);
		

		pending_notif = 0;

		//check peek response
		for (int i = 0; i < nbDependency; i++){ 

			if ((__builtin_k1_lwu(&peek_response[i]) > blkLrtJobIx[i]))
			{
				if ((notif_tab_[__k1_get_cpu_id()].peek_semaphore[targetCID[i]/CLUSTER_PER_BITMASK] & (1ULL << ((targetCID[i]%CLUSTER_PER_BITMASK)*16 + targetPID[i]))) == 0)
				{
					notif_tab_[__k1_get_cpu_id()].peek_semaphore[targetCID[i]/CLUSTER_PER_BITMASK] += 1ULL << ((targetCID[i]%CLUSTER_PER_BITMASK)*16 + targetPID[i]);
					__builtin_k1_wpurge();
					__builtin_k1_fence();
					__builtin_k1_sdu(&peek_response[i],0);
				}
			} 
		}
		

		pending_notif =  a0 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[0]);
		pending_notif += a1 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[1]);
		pending_notif += a2 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[2]);
		pending_notif += a3 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[3]);
		pending_notif += a4 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[4]);
		pending_notif += a5 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[5]);
		pending_notif += a6 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[6]);
		pending_notif += a7 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[7]);
		pending_notif += a8 & ~(notif_tab_[__k1_get_cpu_id()].peek_semaphore[8]);

	}while(pending_notif != 0);
}


void MPPALrtCommunicator::unlockLrt(int jobIx){
	#if __k1io__
	register int currentClusterID = platform_->getNbCluster();
	#else
	register int currentClusterID = __k1_get_cluster_id();
	#endif


	//for each long long in notif_tab_->bitmask
	for (int i = 0; i < sizeof(notif_tab_[0].bitmask)/sizeof(notif_tab_[0].bitmask[0]); i++){

		register int notif_vector = __builtin_k1_lwu(&notif_tab_[__k1_get_cpu_id()].bitmask[i]);

		if(!notif_vector) continue;

		//for each bit in long long
		for (int j = 0; j < PE_PER_BITMASK; j++){

			if(!notif_vector) break;

			int targetCID = i*CLUSTER_PER_BITMASK + j/PE_PER_CLUSTER;
			int targetPID = j%PE_PER_CLUSTER;

			// if someone sent a notif request
			if ((notif_vector & (1ULL << j))){


				int val_request = __builtin_k1_lwu(&notif_tab_[__k1_get_cpu_id()].unlockJobIx[j + i*PE_PER_BITMASK]);
				// printf("[Cluster %d] PE%d : CC%d PE%d is waiting for my job %lld\n",__k1_get_cluster_id(),__k1_get_cpu_id(),i*4 + j/16,j%16,val_request);
				// if current LRT is far enough
				if(jobIx > val_request){
					__builtin_k1_afdau(&notif_tab_[__k1_get_cpu_id()].bitmask[i], -(1ULL << j));

					// maybe unnecessary, I don't remember though
					__builtin_k1_sdu(&notif_tab_[__k1_get_cpu_id()].unlockJobIx[j + i*PE_PER_BITMASK], LONG_MAX);

					notif_vector -= 1ULL << j;

					// printf("[Cluster %d] PE%d : Unlocking CC%d PE%d\n",__k1_get_cluster_id(),__k1_get_cpu_id(),i*4 + j/16,j%16,val_request);				

					if (targetCID == currentClusterID)
					{
						// if locally

						__builtin_k1_afdau(&notif_tab_[targetPID].semaphore[currentClusterID/CLUSTER_PER_BITMASK],-(1ULL << ((currentClusterID%CLUSTER_PER_BITMASK)*PE_PER_CLUSTER + __k1_get_cpu_id())));
						continue;
					}


					off64_t offset;
					offset = (targetPID)*sizeof(Notif_t) + sizeof(notif_tab_[0].bitmask) + sizeof(notif_tab_[0].unlockJobIx) + (currentClusterID/CLUSTER_PER_BITMASK)*sizeof(notif_tab_[0].semaphore[0]);

					// printf("[Cluster %d] PE%d : Sending unlock signal at CC%d PE%d with offset %lld (%llx)\n",
					// 	__k1_get_cluster_id(),__k1_get_cpu_id(),
					// 	targetCID,targetPID,offset,1ULL << ((currentClusterID%CLUSTER_PER_BITMASK)*16 + __k1_get_cpu_id()));

					if (mppa_async_postadd(&notif_segment_[targetCID], offset, -(1ULL << ((currentClusterID%CLUSTER_PER_BITMASK)*PE_PER_CLUSTER + __k1_get_cpu_id()))) != 0){
						printf("[Cluster %d] PE%d : Fail in unlockLrt, mppa_async_postadd\n",__k1_get_cluster_id(),__k1_get_cpu_id());
						while(1);
					}
				}
			}
		}
	}
}

void MPPALrtCommunicator::check_segment_buffer_wraparound(mppa_async_segment_t* segment, void* ptr_dequeue, int size, unsigned long buffer_size, void* out_ptr){

	int size_diff = ((uintptr_t)ptr_dequeue + size) - (buffer_size + (uintptr_t)segment->phys_base);

	#warning "Support for segment buffer overflow checking can be dropped anytime"
	static uint64_t nb_byte_dma[16];
	static uint64_t nb_byte_read[16];

	#ifdef __k1io__
	int ifce = segment->server%4;
	#else
	int ifce = 0;
	#endif

	int tag = segment->channel;
	//uint64_t dma_current_rx_offset = mppa_dnoc[ifce]->rx_queues[tag].current_offset;
	//uint64_t dma_rx_base = mppa_dnoc[ifce]->rx_queues[tag].buffer_base;
	//void *ptr_current_dma_rx = (void*) (uintptr_t) (dma_rx_base + dma_current_rx_offset);

	int bytes_since_dot = mppa_dnoc[ifce]->rx_queues [tag].item_lac.reg;

	nb_byte_dma[__k1_get_cpu_id()] += bytes_since_dot;

	nb_byte_read[__k1_get_cpu_id()] += size;

	if ((nb_byte_dma[__k1_get_cpu_id()] - nb_byte_read[__k1_get_cpu_id()]) > buffer_size){
		printf("[Cluster %d] PE%d : Segment buffer (%ld) is too small for the amount of job sent by SPIDER. Consider increasing its size in PlatformMPPA.h or reducing the amount of ongoing jobs.\n",__k1_get_cluster_id(),__k1_get_cpu_id(),buffer_size);
		while(1);
	}


	if(size_diff > 0){

		#ifdef VERBOSE
		printf("IO%d : %dth wraparound in LRT %d\n",__k1_get_cpu_id(),++wrap_count_,__k1_get_cpu_id());
		#endif

		/* wrap */
		char *ptr = (char*)out_ptr;
		memcpy(ptr, ptr_dequeue, size - size_diff);
		memcpy(&ptr[size - size_diff], (void*)segment->phys_base, size_diff);
	}else{
		memcpy(out_ptr, ptr_dequeue, size);
	}
}
