/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
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
#ifndef PTHREAD_SPIDER_COMMUNICATOR_H
#define PTHREAD_SPIDER_COMMUNICATOR_H

#include <Message.h>
#include <SpiderCommunicator.h>
#include <tools/Stack.h>

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

#include <ControlQueue.h>
#include <TraceQueue.h>
#include "ControlMessageQueue.h"
#include "NotificationQueue.h"

class PThreadSpiderCommunicator : public SpiderCommunicator {
public:
    PThreadSpiderCommunicator(
            ControlMessageQueue<JobMessage *> *spider2LrtJobQueue,
            ControlMessageQueue<LRTMessage *> *spider2LrtLRTQueue,
            NotificationQueue **notificationQueue,
            TraceQueue *traceQueue);

    bool pop_notification(int lrtID, NotificationMessage *msg, bool blocking);

    void push_notification(int lrtID, NotificationMessage *msg);

    std::int32_t push_job_message(JobMessage **message);

    std::int32_t push_lrt_message(LRTMessage **message);

    ~PThreadSpiderCommunicator() override  = default ;

    void *ctrl_start_send(int /*lrtIx*/, std::uint64_t /*size*/) override {
        return nullptr;
    }

    void ctrl_end_send(int, std::uint64_t) override {};

    std::uint64_t ctrl_start_recv(int, void **) override { return 0; };

    void ctrl_start_recv_block(int, void **) override {};

    void ctrl_end_recv(int) override {};

    void *trace_start_send(int size) override;

    void trace_end_send(int size) override;

    int trace_start_recv(void **data) override;

    void trace_start_recv_block(void **data) override;

    void trace_end_recv() override;

    void rst_ctrl_queue() override {};

private:
    ControlMessageQueue<JobMessage *> *spider2LrtJobQueue_;
    ControlMessageQueue<LRTMessage *> *spider2LrtLRTQueue_;
    NotificationQueue **notificationQueue_;
    TraceQueue *traceQueue_;
};

#endif/*PTHREADS_SPIDER_COMMUNICATOR_H*/
