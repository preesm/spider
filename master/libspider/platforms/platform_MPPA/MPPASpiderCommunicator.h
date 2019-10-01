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

#ifndef MPPA_SPIDER_COMMUNICATOR_H
#define MPPA_SPIDER_COMMUNICATOR_H

#include "Message.h"
#include "SpiderCommunicator.h"
#include "tools/Stack.h"
#include <semaphore.h>
//#include <platformMPPA.h>
//#include <queue>
#include "ControlMessageQueue.h"
#include "NotificationQueue.h"

#include <mppa_async.h>

class MPPASpiderCommunicator: public SpiderCommunicator{
public:
	MPPASpiderCommunicator(
		int msgSizeMax, 
		int nLrt,
		void* fTraceWr,
		void* fTraceRd);
	
	~MPPASpiderCommunicator();

	void setLrtCom(int lrtIx, mppa_async_segment_t *fIn, mppa_async_segment_t *fOut);

	void* ctrl_start_send(int lrtIx, int size);
	void ctrl_end_send(int lrtIx, int size);

	int ctrl_start_recv(int lrtIx, void** data);
	void ctrl_end_recv(int lrtIx);

	void* trace_start_send(int size);
	void trace_end_send(int size);

	int trace_start_recv(void** data);
	void trace_end_recv();

	void check_segment_buffer_wraparound(mppa_async_segment_t* segment, void* ptr_dequeue, int size, unsigned long buffer_size, void* out_ptr);

private:
	mppa_async_segment_t** fIn_;
	mppa_async_segment_t** fOut_;
	void* fTraceRd_;
	void* fTraceWr_;

	int msgSizeMax_;

	void* msgBufferRecv_;
	unsigned long curMsgSizeRecv_;
	void* msgBufferSend_;
	unsigned long curMsgSizeSend_;

	char* ptr_temp_;

	unsigned int *size;
	int pending_dequeued_;
	mppa_async_event_t event_;

	int traceBufferIx_;
};

#endif/*MPPA_SPIDER_COMMUNICATOR_H*/
