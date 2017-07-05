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

#ifndef PTHREAD_LRT_COMMUNICATOR_H
#define PTHREAD_LRT_COMMUNICATOR_H

#include <LrtCommunicator.h>
#include <sys/types.h>

#include <semaphore.h>
#ifdef _MSC_VER
	#ifdef int64_t
	#undef int64_t
	#endif

	#ifdef uint64_t
	#undef uint64_t
	#endif
#endif

#include <Message.h>
#include <tools/Stack.h>
#include <queue>

class PThreadLrtCommunicator: public LrtCommunicator{
public:
	PThreadLrtCommunicator(
			int msgSizeMax,
			std::queue<unsigned char>* fIn,
			std::queue<unsigned char>* fOut,
			std::queue<unsigned char>* fTrace,
			sem_t *semTrace,
			sem_t *semFifoSpidertoLRT,
			sem_t *semFifoLRTtoSpider,
			void* fifos,
			void* dataMem
		);

	~PThreadLrtCommunicator();

	void* ctrl_start_send(int size);
	void ctrl_end_send(int size);

	int ctrl_start_recv(void** data);
	void ctrl_end_recv();

	void* trace_start_send(int size);
	void trace_end_send(int size);

	void* data_start_send(Fifo* f);
	void data_end_send(Fifo* f);

	void* data_recv(Fifo* f);

	void setLrtJobIx(int lrtIx, int jobIx);
	long getLrtJobIx(int lrtIx);

	void waitForLrtUnlock(int nbDependency, int* blkLrtIx, int* blkLrtJobIx, int jobIx);

private:
	std::queue<unsigned char>* fIn_;
	std::queue<unsigned char>* fOut_;
	std::queue<unsigned char>* fTrace_;

	sem_t* semTrace_;
	sem_t* semFifoSpidertoLRT_;
	sem_t* semFifoLRTtoSpider_;

	int msgSizeMax_;

	void* msgBufferSend_;
	int curMsgSizeSend_;

	void* msgBufferRecv_;
	int curMsgSizeRecv_;

	unsigned long* jobTab_;
	unsigned char* shMem_;
};

#endif/*PTHREAD_LRT_COMMUNICATOR_H*/
