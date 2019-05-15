/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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
#include "LinuxSpiderCommunicator.h"
#include <monitor/StackMonitor.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>

LinuxSpiderCommunicator::LinuxSpiderCommunicator(
        int msgSizeMax,
        int nLrt,
        sem_t *semTrace,
        int fTraceWr,
        int fTraceRd) {

    fIn_ = CREATE_MUL(ARCHI_STACK, nLrt, int);
    fOut_ = CREATE_MUL(ARCHI_STACK, nLrt, int);
    fTraceRd_ = fTraceRd;
    fTraceWr_ = fTraceWr;
    semTrace_ = semTrace;

    msgSizeMax_ = msgSizeMax;

    msgBufferRecv_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeRecv_ = 0;
    msgBufferSend_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeSend_ = 0;
}

LinuxSpiderCommunicator::~LinuxSpiderCommunicator() {
    StackMonitor::free(ARCHI_STACK, fIn_);
    StackMonitor::free(ARCHI_STACK, fOut_);
    StackMonitor::free(ARCHI_STACK, msgBufferRecv_);
    StackMonitor::free(ARCHI_STACK, msgBufferSend_);
}

void LinuxSpiderCommunicator::setLrtCom(int lrtIx, int fIn, int fOut) {
    fIn_[lrtIx] = fIn;
    fOut_[lrtIx] = fOut;
}

void *LinuxSpiderCommunicator::ctrl_start_send(int lrtIx, std::uint64_t size) {
    if (curMsgSizeSend_)
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");
    curMsgSizeSend_ = size;
    return msgBufferSend_;
}

void LinuxSpiderCommunicator::ctrl_end_send(int lrtIx, std::uint64_t size) {
    std::uint64_t s = curMsgSizeSend_;
    ssize_t l;
    l = write(fOut_[lrtIx], &s, sizeof(std::uint64_t));
    l = write(fOut_[lrtIx], msgBufferSend_, curMsgSizeSend_);
    curMsgSizeSend_ = 0;
}

std::uint64_t LinuxSpiderCommunicator::ctrl_start_recv(int lrtIx, void **data) {
    std::uint64_t size;
    int nb = read(fIn_[lrtIx], &size, sizeof(std::uint64_t));

    if (nb < 0) return 0;

    if (size > (std::uint64_t) msgSizeMax_)
        throw std::runtime_error("Msg too big\n");

    curMsgSizeRecv_ = size;

    while (size) {
        int recv = read(fIn_[lrtIx], msgBufferRecv_, size);
        if (recv > 0)
            size -= recv;
    }

    *data = msgBufferRecv_;
    return curMsgSizeRecv_;
}

void LinuxSpiderCommunicator::ctrl_end_recv(int lrtIx) {
    curMsgSizeRecv_ = 0;
}

void *LinuxSpiderCommunicator::trace_start_send(int size) {
    if (curMsgSizeSend_)
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");
    curMsgSizeSend_ = size;
    return msgBufferSend_;
}

void LinuxSpiderCommunicator::trace_end_send(int size) {
    unsigned long s = curMsgSizeSend_;
    ssize_t l;

    int err = sem_wait(semTrace_);

    if (err != 0) {
        perror("LinuxLrtCommunicator::trace_end_send");
        exit(-1);
    }

    l = write(fTraceWr_, &s, sizeof(unsigned long));
    l = write(fTraceWr_, msgBufferSend_, curMsgSizeSend_);

    sem_post(semTrace_);

    curMsgSizeSend_ = 0;
}

int LinuxSpiderCommunicator::trace_start_recv(void **data) {
    unsigned long size;
    int nb = read(fTraceRd_, &size, sizeof(unsigned long));

    if (nb < 0) return 0;

    if (size > (unsigned long) msgSizeMax_)
        throw std::runtime_error("Msg too big\n");

    curMsgSizeRecv_ = size;

    while (size) {
        int recv = read(fTraceRd_, msgBufferRecv_, size);
        if (recv > 0)
            size -= recv;
    }

    *data = msgBufferRecv_;
    return curMsgSizeRecv_;
}

void LinuxSpiderCommunicator::trace_end_recv() {
    curMsgSizeRecv_ = 0;
}
