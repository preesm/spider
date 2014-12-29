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

#include "LinuxLrtCommunicator.h"

#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdlib>
#include <cstdio>

LinuxLrtCommunicator::LinuxLrtCommunicator(
		int msgSizeMax,
		int fIn,
		int fOut,
		int fTrace,
		sem_t *semTrace,
		void* shMem,
		int nFifos,
		Stack* s
	){
	stack_ = s;
	fIn_ = fIn;
	fOut_ = fOut;
	fTrace_ = fTrace;
	semTrace_ = semTrace;
	msgSizeMax_ = msgSizeMax;

	msgBufferRecv_ = (void*) CREATE_MUL(s, msgSizeMax, char);
	curMsgSizeRecv_ = 0;

	msgBufferSend_ = (void*) CREATE_MUL(s, msgSizeMax, char);
	curMsgSizeSend_ = 0;

	nbFifos_ = nFifos;
	fifos_ = (unsigned long*)shMem;
	shMem_ = (unsigned char*)((long)shMem + nFifos*sizeof(unsigned long));
}

LinuxLrtCommunicator::~LinuxLrtCommunicator(){
	stack_->free(msgBufferRecv_);
	stack_->free(msgBufferSend_);
}

void* LinuxLrtCommunicator::ctrl_start_send(int size){
	if(curMsgSizeSend_)
		throw "LrtCommunicator: Try to send a msg when previous one is not sent";
	curMsgSizeSend_ = size;
	return msgBufferSend_;
}

void LinuxLrtCommunicator::ctrl_end_send(int size){
	unsigned long s = curMsgSizeSend_;
	write(fOut_, &s, sizeof(unsigned long));
	write(fOut_, msgBufferSend_, curMsgSizeSend_);
	curMsgSizeSend_ = 0;
}

int LinuxLrtCommunicator::ctrl_start_recv(void** data){
	unsigned long size;
	int nb = read(fIn_, &size, sizeof(unsigned long));

	if(nb<0) return 0;

	if(size > msgSizeMax_)
		throw "Msg too big\n";

	curMsgSizeRecv_ = size;

	while(size){
		int recv = read(fIn_, msgBufferRecv_, size);
		if(recv > 0)
			size -= recv;
	}

	*data = msgBufferRecv_;
	return curMsgSizeRecv_;
}

void LinuxLrtCommunicator::ctrl_end_recv(){
	curMsgSizeRecv_ = 0;
}

void* LinuxLrtCommunicator::trace_start_send(int size){
	curMsgSizeSend_ = size;
	return msgBufferSend_;
}

void LinuxLrtCommunicator::trace_end_send(int size){
	unsigned long s = curMsgSizeSend_;

	int err = sem_wait(semTrace_);
	if(err != 0){
		perror("LinuxLrtCommunicator::trace_end_send");
		exit(-1);
	}

	write(fTrace_, &s, sizeof(unsigned long));
	write(fTrace_, msgBufferSend_, curMsgSizeSend_);
	curMsgSizeSend_ = 0;
}

void LinuxLrtCommunicator::data_end_send(Fifo* f){
	volatile unsigned long *mutex = fifos_ + f->id;
	// TODO protect mutex !
	// TODO cache memory
	*mutex += f->ntoken;
}
long LinuxLrtCommunicator::data_recv(Fifo* f){
	volatile unsigned long *mutex = fifos_ + f->id;
	while(*mutex < f->ntoken);
	*mutex -= f->ntoken;
	return (long)(shMem_+f->alloc);
}

long LinuxLrtCommunicator::data_start_send(Fifo* f){
	return (long)(shMem_+f->alloc);
}
