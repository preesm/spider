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
#include "PThreadLrtCommunicator.h"

#ifndef _WIN32

#include <unistd.h>

#endif

#include <cstdlib>
#include <stdio.h>
#include <platform.h>

PThreadLrtCommunicator::PThreadLrtCommunicator(
        int msgSizeMax,
        std::queue<unsigned char> *fIn,
        std::queue<unsigned char> *fOut,
        std::queue<unsigned char> *fTrace,
        sem_t *mutexTrace,
        sem_t *mutexFifoSpidertoLRT,
        sem_t *mutexFifoLRTtoSpider,
        sem_t *semFifoSpidertoLRT,
        void *jobTab,
        void *dataMem
) {
    fIn_ = fIn;
    fOut_ = fOut;
    fTrace_ = fTrace;

    mutexTrace_ = mutexTrace;
    mutexFifoLRTtoSpider_ = mutexFifoLRTtoSpider;
    mutexFifoSpidertoLRT_ = mutexFifoSpidertoLRT;
    semFifoSpidertoLRT_ = semFifoSpidertoLRT;

    msgSizeMax_ = msgSizeMax;

    msgBufferRecv_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeRecv_ = 0;

    msgBufferSend_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeSend_ = 0;

    jobTab_ = (unsigned long *) jobTab;
    shMem_ = (unsigned char *) dataMem;
}

PThreadLrtCommunicator::~PThreadLrtCommunicator() {
    StackMonitor::free(ARCHI_STACK, msgBufferRecv_);
    StackMonitor::free(ARCHI_STACK, msgBufferSend_);
}

void *PThreadLrtCommunicator::ctrl_start_send(int size) {
    if (curMsgSizeSend_)
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");
    curMsgSizeSend_ = size;
    return msgBufferSend_;
}

void PThreadLrtCommunicator::ctrl_end_send(int size) {
    unsigned long s = curMsgSizeSend_;

    /** Take Mutex protecting the Queue */
    sem_wait(mutexFifoLRTtoSpider_);

    //Envoi de la taille du message à venir
    for (unsigned int i = 0; i < sizeof(unsigned long); i++)
        fOut_->push(s >> (sizeof(unsigned long) - 1 - i) * 8 & 0xFF);

    //Envoi du message
    for (int i = 0; i < curMsgSizeSend_; i++) fOut_->push((*(((char *) msgBufferSend_) + i)) & 0xFF);

    /** Relax Mutex protecting the Queue */
    sem_post(mutexFifoLRTtoSpider_);

    curMsgSizeSend_ = 0;
}

int PThreadLrtCommunicator::ctrl_start_recv(void **data) {
    unsigned long size = 0;

    /** Take sem (representing 1 message in the queue */
    if(sem_trywait(semFifoSpidertoLRT_)){
        return 0;
    }

    /** Take Mutex protecting the Queue */
    sem_wait(mutexFifoSpidertoLRT_);

    //Reception/reconstitution de la taille du message à venir
    for (unsigned int nb = 0; nb < sizeof(unsigned long); nb++) {
        size = size << 8;
        size += fIn_->front();
        fIn_->pop();
    }

    if (size > (unsigned long) msgSizeMax_)
        throw std::runtime_error("Msg too big\n");

    curMsgSizeRecv_ = size;

    //Reception du message
    for (unsigned int recv = 0; recv < size; recv++) {
        *(((char *) msgBufferRecv_) + recv) = fIn_->front();
        fIn_->pop();
    }

    /** Relax Mutex protecting the Queue */
    sem_post(mutexFifoSpidertoLRT_);

    *data = msgBufferRecv_;
    return curMsgSizeRecv_;
}

void PThreadLrtCommunicator::ctrl_start_recv_block(void **data){
    unsigned long size = 0;

    /** Take sem (representing 1 message in the queue */
    sem_wait(semFifoSpidertoLRT_);

    /** Take Mutex protecting the Queue */
    sem_wait(mutexFifoSpidertoLRT_);

    //Reception/reconstitution de la taille du message à venir
    for (unsigned int nb = 0; nb < sizeof(unsigned long); nb++) {
        size = size << 8;
        size += fIn_->front();
        fIn_->pop();
    }

    if (size > (unsigned long) msgSizeMax_)
        throw std::runtime_error("Msg too big\n");

    curMsgSizeRecv_ = size;

    //Reception du message
    for (unsigned int recv = 0; recv < size; recv++) {
        *(((char *) msgBufferRecv_) + recv) = fIn_->front();
        fIn_->pop();
    }

    /** Relax Mutex protecting the Queue */
    sem_post(mutexFifoSpidertoLRT_);

    *data = msgBufferRecv_;
}

void PThreadLrtCommunicator::ctrl_end_recv() {
    curMsgSizeRecv_ = 0;
}

void *PThreadLrtCommunicator::trace_start_send(int size) {
    if (curMsgSizeSend_)
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");
    curMsgSizeSend_ = size;
    return msgBufferSend_;
}

void PThreadLrtCommunicator::trace_end_send(int size) {
    unsigned long s = curMsgSizeSend_;

    int err = sem_wait(mutexTrace_);
    if (err != 0) {
        perror("PThreadLrtCommunicator::trace_end_send");
        exit(-1);
    }

    //Envoi de la taille de la trace
    for (unsigned int i = 0; i < sizeof(unsigned long); i++)
        fTrace_->push(s >> (sizeof(unsigned long) - 1 - i) * 8 & 0xFF);

    //Envoi de la trace
    for (int i = 0; i < curMsgSizeSend_; i++) fTrace_->push(*(((char *) msgBufferSend_) + i) & 0xFF);

    sem_post(mutexTrace_);

    curMsgSizeSend_ = 0;
}

void PThreadLrtCommunicator::data_end_send(Fifo *f) {
    // Nothing to do
}

void *PThreadLrtCommunicator::data_recv(Fifo *f) {
    return (void *) Platform::get()->virt_to_phy((void *) (intptr_t) (f->alloc));
}

void *PThreadLrtCommunicator::data_start_send(Fifo *f) {
    return (void *) Platform::get()->virt_to_phy((void *) (intptr_t) (f->alloc));
}

void PThreadLrtCommunicator::setLrtJobIx(int lrtIx, int jobIx) {
    jobTab_[lrtIx] = jobIx;
}

long PThreadLrtCommunicator::getLrtJobIx(int lrtIx) {
    // TODO Protect from invalid read: long are not atomic !!!
    return jobTab_[lrtIx];
}

void PThreadLrtCommunicator::waitForLrtUnlock(int nbDependency, int *blkLrtIx, int *blkLrtJobIx, int jobIx) {
    for (int i = 0; i < nbDependency; i++) {
        while (blkLrtJobIx[i] >= getLrtJobIx(blkLrtIx[i]));
    }
}
