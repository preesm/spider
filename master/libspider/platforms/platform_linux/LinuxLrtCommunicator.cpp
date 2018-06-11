/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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
#include "LinuxLrtCommunicator.h"

#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdlib>
#include <cstdio>
#include <platform.h>

#include <monitor/StackMonitor.h>

LinuxLrtCommunicator::LinuxLrtCommunicator(
        int msgSizeMax,
        int fIn,
        int fOut,
        int fTrace,
        sem_t *semFifo,
        sem_t *semTrace,
        void *fifos,
        void *dataMem,
        int nFifos
) {
    fIn_ = fIn;
    fOut_ = fOut;
    fTrace_ = fTrace;
    semFifo_ = semFifo;
    semTrace_ = semTrace;
    msgSizeMax_ = msgSizeMax;

    msgBufferRecv_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeRecv_ = 0;

    msgBufferSend_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeSend_ = 0;

    nbFifos_ = nFifos;
    fifos_ = (unsigned long *) fifos;
    shMem_ = (unsigned char *) dataMem;
}

LinuxLrtCommunicator::~LinuxLrtCommunicator() {
    StackMonitor::free(ARCHI_STACK, msgBufferRecv_);
    StackMonitor::free(ARCHI_STACK, msgBufferSend_);
}

void *LinuxLrtCommunicator::ctrl_start_send(int size) {
    if (curMsgSizeSend_)
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");
    curMsgSizeSend_ = size;
    return msgBufferSend_;
}

void LinuxLrtCommunicator::ctrl_end_send(int size) {
    unsigned long s = curMsgSizeSend_;
    ssize_t l;
    l = write(fOut_, &s, sizeof(unsigned long));
    l = write(fOut_, msgBufferSend_, curMsgSizeSend_);
    curMsgSizeSend_ = 0;
}

int LinuxLrtCommunicator::ctrl_start_recv(void **data) {
    unsigned long size;
    int nb = read(fIn_, &size, sizeof(unsigned long));

    if (nb < 0) return 0;

    if (size > (unsigned long) msgSizeMax_)
        throw std::runtime_error("Msg too big\n");

    curMsgSizeRecv_ = size;

    while (size) {
        int recv = read(fIn_, msgBufferRecv_, size);
        if (recv > 0)
            size -= recv;
    }

    *data = msgBufferRecv_;
    return curMsgSizeRecv_;
}

void LinuxLrtCommunicator::ctrl_end_recv() {
    curMsgSizeRecv_ = 0;
}

void *LinuxLrtCommunicator::trace_start_send(int size) {
    if (curMsgSizeSend_)
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");
    curMsgSizeSend_ = size;
    return msgBufferSend_;
}

void LinuxLrtCommunicator::trace_end_send(int size) {
    unsigned long s = curMsgSizeSend_;
    ssize_t l;

    int err = sem_wait(semTrace_);

    if (err != 0) {
        perror("LinuxLrtCommunicator::trace_end_send");
        exit(-1);
    }

    l = write(fTrace_, &s, sizeof(unsigned long));
    l = write(fTrace_, msgBufferSend_, curMsgSizeSend_);

    sem_post(semTrace_);

    curMsgSizeSend_ = 0;
}

void LinuxLrtCommunicator::data_end_send(Fifo *f) {
    if (f->ntoken) {
        int err = sem_wait(semFifo_);
        if (err != 0) {
            perror("LinuxLrtCommunicator::data_end_send");
            exit(-1);
        }

        volatile unsigned long *mutex = fifos_ + f->id;

        *mutex += f->ntoken;

        sem_post(semFifo_);
    }
}

long LinuxLrtCommunicator::data_recv(Fifo *f) {
    if (f->ntoken) {
        volatile unsigned long *mutex = fifos_ + f->id;


        do {
            int err = sem_wait(semFifo_);
            if (err != 0) {
                perror("LinuxLrtCommunicator::data_recv");
                exit(-1);
            }

            if (*mutex >= f->ntoken) {
                *mutex -= f->ntoken;
                sem_post(semFifo_);
                break;
            }
            sem_post(semFifo_);
        } while (1);//*mutex < f->ntoken);
    }
    return (long) Platform::get()->virt_to_phy((void *) (intptr_t) (f->alloc));
}

long LinuxLrtCommunicator::data_start_send(Fifo *f) {
    return (long) Platform::get()->virt_to_phy((void *) (intptr_t) (f->alloc));
}

void LinuxLrtCommunicator::setLrtJobIx(int jobIx, int lrtIx) {
    // Not implemented
}

unsigned long LinuxLrtCommunicator::getLrtJobIx(int lrt) {
    // Not implemented
    return 0;
}
