/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
 * Karol Desnos <karol.desnos@insa-rennes.fr> (2017)
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
#ifndef PTHREAD_LRT_COMMUNICATOR_H
#define PTHREAD_LRT_COMMUNICATOR_H

#include <LrtCommunicator.h>
#include <sys/types.h>

// semaphore.h includes _ptw32.h that redefines types int64_t and uint64_t on Visual Studio,
// making compilation error with the IDE's own declaration of said types
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

#include "ControlQueue.h"
#include "TraceQueue.h"
#include "DataQueues.h"

class PThreadLrtCommunicator : public LrtCommunicator {
public:
    PThreadLrtCommunicator(
            ControlQueue *spider2LrtQueue,
            ControlQueue *lrt2SpiderQueue,
            DataQueues *dataQueues,
            TraceQueue *traceQueue
    );

    ~PThreadLrtCommunicator();

    void *ctrl_start_send(int size);

    void ctrl_end_send(int size);

    int ctrl_start_recv(void **data);

    void ctrl_start_recv_block(void **data);

    void ctrl_end_recv();

    void *trace_start_send(int size);

    void trace_end_send(int size);

    void *data_start_send(Fifo *f);

    void data_end_send(Fifo *f);

    void *data_recv(Fifo *f);

    void setLrtJobIx(int lrtIx, int jobIx);

    void waitForLrtUnlock(int nbDependency, int *blkLrtIx, int *blkLrtJobIx, int jobIx);


private:

    ControlQueue *spider2LrtQueue_;
    ControlQueue *lrt2SpiderQueue_;
    DataQueues *dataQueues_;
    TraceQueue *traceQueue_;
};

#endif/*PTHREAD_LRT_COMMUNICATOR_H*/
