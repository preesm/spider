/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
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
#include "PThreadSpiderCommunicator.h"
#include <monitor/StackMonitor.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <platform.h>

PThreadSpiderCommunicator::PThreadSpiderCommunicator(
		int msgSizeMax,
		int nLrt,
		sem_t* semTrace,
		sem_t* semFifoSpidertoLRT,
		sem_t* semFifoLRTtoSpider,
		std::queue<unsigned char>* fTraceWr,
		std::queue<unsigned char>* fTraceRd){

	fIn_ = (std::queue<unsigned char>**) malloc(nLrt * sizeof(std::queue<unsigned char>*));
	fOut_ = (std::queue<unsigned char>**) malloc(nLrt * sizeof(std::queue<unsigned char>*));
	fTraceRd_ = fTraceRd;
	fTraceWr_ = fTraceWr;

	semTrace_ = semTrace;
	semFifoSpidertoLRT_ = semFifoSpidertoLRT;
	semFifoLRTtoSpider_ = semFifoLRTtoSpider;

	msgSizeMax_ = msgSizeMax;

	msgBufferRecv_ = (void*) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
	curMsgSizeRecv_ = 0;
	msgBufferSend_ = (void*) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
	curMsgSizeSend_ = 0;
}

PThreadSpiderCommunicator::~PThreadSpiderCommunicator(){
//	StackMonitor::free(ARCHI_STACK, fIn_);
//	StackMonitor::free(ARCHI_STACK, fOut_);
	StackMonitor::free(ARCHI_STACK, msgBufferRecv_);
	StackMonitor::free(ARCHI_STACK, msgBufferSend_);

	free(fIn_);
	free(fOut_);
}

void PThreadSpiderCommunicator::setLrtCom(int lrtIx, std::queue<unsigned char>* fIn, std::queue<unsigned char>* fOut){
	fIn_[lrtIx] = fIn;
	fOut_[lrtIx] = fOut;
}

void* PThreadSpiderCommunicator::ctrl_start_send(int lrtIx, int size){
	if(curMsgSizeSend_)
		throw "LrtCommunicator: Try to send a msg when previous one is not sent";
	curMsgSizeSend_ = size;
	return msgBufferSend_;
}

void PThreadSpiderCommunicator::ctrl_end_send(int lrtIx, int size){
	unsigned long s = curMsgSizeSend_;

	static unsigned long size_fifo[4] = {0};


	//prise du semaphore de fOut_[lrtIx]
	sem_wait(&semFifoSpidertoLRT_[lrtIx]);

	//Envoi de la taille du message à venir
	for (unsigned int i = 0;i < sizeof(unsigned long);i++) fOut_[lrtIx]->push(s >> (sizeof(unsigned long)-1-i)*8 & 0xFF);

	//Envoie du message
	for (int i = 0;i < curMsgSizeSend_;i++) fOut_[lrtIx]->push(*(((char*)msgBufferSend_)+i) & 0xFF);

	if (fOut_[lrtIx]->size() > size_fifo[lrtIx]) size_fifo[lrtIx] = fOut_[lrtIx]->size();

	//Relachement du semaphore de fOut_[lrtIx]
	sem_post(&semFifoSpidertoLRT_[lrtIx]);

	curMsgSizeSend_ = 0;
}

int PThreadSpiderCommunicator::ctrl_start_recv(int lrtIx, void** data){
	unsigned long size = 0;

	//Prise du semaphore de fIn_[lrtIx]
	sem_wait(&semFifoLRTtoSpider_[lrtIx]);
	

	//Rien à faire si fifo vide
	if (fIn_[lrtIx]->empty()){
		
		sem_post(&semFifoLRTtoSpider_[lrtIx]);

		return 0;
	}


	//Reception/reconstitution de la taille du message à venir
	for (unsigned int nb = 0;nb < sizeof(unsigned long);nb++){
		size = size << 8;
		size += fIn_[lrtIx]->front();
		fIn_[lrtIx]->pop();
	}

	if(size > (unsigned long)msgSizeMax_)
		throw "Msg too big\n";

	curMsgSizeRecv_ = size;

	//Reception du message
	for (unsigned int recv = 0;recv < size;recv++){
		*(((char*) msgBufferRecv_) + recv) = fIn_[lrtIx]->front();
		fIn_[lrtIx]->pop();
	}

	//Relachement du semaphore de fIn_[lrtIx]
	sem_post(&semFifoLRTtoSpider_[lrtIx]);

	*data = msgBufferRecv_;
	return curMsgSizeRecv_;
}

void PThreadSpiderCommunicator::ctrl_end_recv(int lrtIx){
	curMsgSizeRecv_ = 0;
}

void* PThreadSpiderCommunicator::trace_start_send(int size){
	if(curMsgSizeSend_)
		throw "LrtCommunicator: Try to send a msg when previous one is not sent";
	curMsgSizeSend_ = size;
	return msgBufferSend_;
}

void PThreadSpiderCommunicator::trace_end_send(int size){
	unsigned long s = curMsgSizeSend_;

	static unsigned int size_trace;

	int err = sem_wait(semTrace_);

	if(err != 0){
		perror("PThreadSpiderCommunicator::trace_end_send");
		exit(-1);
	}

	//Envoi de la taille de la trace à venir
	for (unsigned int i = 0;i < sizeof(unsigned long);i++) fTraceWr_->push(s >> (sizeof(unsigned long)-1-i)*8 & 0xFF);

	//Envoi de la trace
	for (int i = 0;i < curMsgSizeSend_;i++) fTraceWr_->push((*(((char*)msgBufferSend_)+i)) & 0xFF);

	sem_post(semTrace_);

	curMsgSizeSend_ = 0;
}

int PThreadSpiderCommunicator::trace_start_recv(void** data){
	unsigned long size = 0;
	unsigned int nb = 0;

	if (fTraceRd_->empty()) return 0;

	for (nb = 0;nb < sizeof(unsigned long);nb++){
		while (fTraceRd_->empty());
		size = size << 8;
		size += fTraceRd_->front();
		fTraceRd_->pop();
	}

	if(nb<0) return 0;

	if(size > (unsigned long)msgSizeMax_)
		throw "Msg too big\n";

	curMsgSizeRecv_ = size;

	unsigned int recv;
	for (recv = 0;recv < size;recv++){
		while (fTraceRd_->empty());
		*(((char*) msgBufferRecv_) + recv) = fTraceRd_->front();
		fTraceRd_->pop();
	}

	*data = msgBufferRecv_;
	return curMsgSizeRecv_;
}

void PThreadSpiderCommunicator::trace_end_recv(){
	curMsgSizeRecv_ = 0;
}
