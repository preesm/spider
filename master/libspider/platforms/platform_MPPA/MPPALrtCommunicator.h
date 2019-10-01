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

#ifndef MPPA_LRT_COMMUNICATOR_H
#define MPPA_LRT_COMMUNICATOR_H

#include <LrtCommunicator.h>
#include <Message.h>
#include <tools/Stack.h>
#include <platformMPPA.h>

#include <mppa_async.h>

#define CLUSTER_PER_BITMASK (2)
#define PE_PER_BITMASK (32)
#define PE_PER_CLUSTER (16)

#define MAX_OUT_PARAM (20)

class MPPALrtCommunicator: public LrtCommunicator{
public:
	MPPALrtCommunicator(
			int msgSizeMax,
			mppa_async_segment_t* fIn,
			mppa_async_segment_t* fOut,
			void* jobTab_,
			void* traceBuffer,
			mppa_async_segment_t* notif_segment,
			mppa_async_segment_t* jobTab_segment,
			mppa_async_segment_t* dataMem_segment,
			mppa_async_segment_t* trace_rdma_segment,
			PlatformMPPA* platform
		);

	~MPPALrtCommunicator();

	void* ctrl_start_send(int size);
	void ctrl_end_send(int size);

	int ctrl_start_recv(void** data);
	void ctrl_end_recv();

	void* trace_start_send(int size);
	void trace_end_send(int size);

	uintptr_t data_start_send(Fifo* f);
	void data_end_send(Fifo* f);

	uintptr_t data_recv(Fifo* f);

	void allocateDataBuffer(int nbInput, Fifo* fIn, int nbOutput, Fifo* fOut);
	void freeDataBuffer(int nbInput, int nbOutput);

	void setLrtJobIx(int lrtIx, int jobIx);
	long getLrtJobIx(int lrtIx);

	void rstLrtJobIx(int lrtIx);

	void waitForLrtUnlock(int nbDependency, int* blkLrtIx, int* blkLrtJobIx, int jobIx);
	void unlockLrt(int jobIx);

	void check_segment_buffer_wraparound(mppa_async_segment_t* segment, void* ptr_dequeue, int size, unsigned long buffer_size, void* out_ptr);

private:
	mppa_async_segment_t* fIn_;
	mppa_async_segment_t* fOut_;

	int msgSizeMax_;

	void* msgBufferSend_;
	int curMsgSizeSend_;

	void* msgBufferRecv_;
	unsigned long curMsgSizeRecv_;

	Notif_t* notif_tab_;
	void* traceBuffer_;

	int pending_dequeued_;
	mppa_async_event_t event_;
	
	unsigned int *size_receive_;

	char* ptr_temp_;

	#ifdef VERBOSE
	int wrap_count_;
	#endif

	mppa_async_segment_t* notif_segment_;
	mppa_async_segment_t* jobTab_segment_;
	mppa_async_segment_t* dataMem_segment_;
	mppa_async_segment_t* trace_rdma_segment_;

	int nb_inFifos_;
	int nb_outFifos_;

	int input_flushed_;

	void** input_local_buffer_;
	void** output_local_buffer_;

	static long long lock;

	static long long traceBufferIx_;

	static long long nb_ongoing_job_;

	bool active_state_self_;

	long long true_value_;

	mppa_async_event_t event_trace_;

	mppa_async_event_t event_data_;

	PlatformMPPA* platform_;

	void* temp_peek_ptr_;
 
	mppa_async_event_t* peek_event_;

	int peek_sent_;
 };


//void check_segment_buffer_wraparound(mppa_async_segment_t* segment, void* ptr_dequeue, int size, unsigned long buffer_size, void* out_ptr);

#endif/*MPPA_LRT_COMMUNICATOR_H*/
