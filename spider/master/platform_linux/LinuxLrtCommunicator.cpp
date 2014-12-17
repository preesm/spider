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

LinuxLrtCommunicator::LinuxLrtCommunicator(int msgSizeMax, int fIn, int fOut, void* shMem, int nFifo, Stack* s){
	fIn_ = fIn;
	fOut_ = fOut;
	msgSizeMax_ = msgSizeMax;
	msgBuffer_ = s->alloc(msgSizeMax);
	curMsgSize_ = 0;
	nbFifos_ = nFifo;
	fifos_ = (unsigned long*)shMem;
	shMem_ = (unsigned char*)((long)shMem + nFifo*sizeof(unsigned long));
}
LinuxLrtCommunicator::~LinuxLrtCommunicator(){

}

void* LinuxLrtCommunicator::alloc(int size){
	curMsgSize_ = size;
	return msgBuffer_;
}
void LinuxLrtCommunicator::send(int lrtIx){
	unsigned long size = curMsgSize_;
	write(fOut_, &size, sizeof(unsigned long));
	write(fOut_, msgBuffer_, curMsgSize_);
	curMsgSize_ = 0;
}

int LinuxLrtCommunicator::recv(int lrtIx, void** data){
	unsigned long size;
	int nb = read(fIn_, &size, sizeof(unsigned long));
	if(nb<0) return 0;
	if(size > msgSizeMax_)
		throw "Msg too big\n";
	read(fIn_, msgBuffer_, size);
	curMsgSize_ = size;
	*data = msgBuffer_;
	return size;
}

void LinuxLrtCommunicator::end_recv(){
	curMsgSize_ = 0;
}

void LinuxLrtCommunicator::sendData(Fifo* f){
	volatile unsigned long *mutex = fifos_ + f->id;
	// TODO protect mutex !
	// TODO cache memory
	*mutex += f->ntoken;
}
long LinuxLrtCommunicator::recvData(Fifo* f){
	volatile unsigned long *mutex = fifos_ + f->id;
	while(*mutex < f->ntoken);
	*mutex -= f->ntoken;
	return (long)(shMem_+f->alloc);
}

long LinuxLrtCommunicator::pre_sendData(Fifo* f){
	return (long)(shMem_+f->alloc);
}
