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

#include "LinuxSpiderCommunicator.h"

#include <fcntl.h>
#include <unistd.h>

LinuxSpiderCommunicator::LinuxSpiderCommunicator(int msgSizeMax, int nLrt, Stack* s){
	stack_ = s;
	fIn_ = CREATE_MUL(s, nLrt, int);
	fOut_ = CREATE_MUL(s, nLrt, int);
	msgSizeMax_ = msgSizeMax;
	msgBuffer_ = (void*) CREATE_MUL(s, msgSizeMax, char);
	curMsgSize_ = 0;
}
LinuxSpiderCommunicator::~LinuxSpiderCommunicator(){
	stack_->free(fIn_);
	stack_->free(fOut_);
	stack_->free(msgBuffer_);
}

void LinuxSpiderCommunicator::setLrtCom(int lrtIx, int fIn, int fOut){
	fIn_[lrtIx] = fIn;
	fOut_[lrtIx] = fOut;
}

void* LinuxSpiderCommunicator::alloc(int size){
	curMsgSize_ = size;
	return msgBuffer_;
}
void LinuxSpiderCommunicator::send(int lrtIx){
	unsigned long size = curMsgSize_;
	write(fOut_[lrtIx], &size, sizeof(unsigned long));
	write(fOut_[lrtIx], msgBuffer_, curMsgSize_);
	curMsgSize_ = 0;
}

int LinuxSpiderCommunicator::recv(int lrtIx, void** data){
	unsigned long size;
	read(fIn_[lrtIx], &size, sizeof(unsigned long));
	read(fIn_[lrtIx], msgBuffer_, size);
	curMsgSize_ = size;
	*data = msgBuffer_;
	return size;
}

void LinuxSpiderCommunicator::end_recv(){
	curMsgSize_ = 0;
}

void LinuxSpiderCommunicator::sendData(Fifo* f){
	throw "Spider Communicator should not use Data Fifos\n";
}
long LinuxSpiderCommunicator::recvData(Fifo* f){
	throw "Spider Communicator should not use Data Fifos\n";
	return 0;
}

long LinuxSpiderCommunicator::pre_sendData(Fifo* f){
	throw "Spider Communicator should not use Data Fifos\n";
	return 0;
}

