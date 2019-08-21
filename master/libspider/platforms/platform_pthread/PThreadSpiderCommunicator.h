/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
 * Karol Desnos <karol.desnos@insa-rennes.fr> (2017)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013)
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

#include "Message.h"
#include "SpiderCommunicator.h"
#include "tools/Stack.h"

#include "SpiderSemaphore.h"

#include "ControlMessageQueue.h"
#include "NotificationQueue.h"

class PThreadSpiderCommunicator : public SpiderCommunicator {
public:
    PThreadSpiderCommunicator(
            ControlMessageQueue<JobInfoMessage *> *spider2LrtJobQueue,
            ControlMessageQueue<ParameterMessage *> *lrt2SpiderParamQueue,
            NotificationQueue<NotificationMessage> **notificationQueue,
            ControlMessageQueue<TraceMessage *> *traceQueue,
            ControlMessageQueue<PapifyMessage *> *papifyQueue);

    ~PThreadSpiderCommunicator() override = default;

    void push_notification(int lrtID, NotificationMessage *msg) override;

    bool pop_notification(int lrtID, NotificationMessage *msg, bool blocking) override;

    std::int32_t push_job_message(JobInfoMessage **message) override;

    void pop_job_message(JobInfoMessage **msg, std::int32_t id) override;

    std::int32_t push_parameter_message(ParameterMessage **message) override;

    void pop_parameter_message(ParameterMessage **msg, std::int32_t id) override;

    std::int32_t push_trace_message(TraceMessage **message) override;

    void pop_trace_message(TraceMessage **message, std::int32_t id) override;

    std::int32_t push_papify_message(PapifyMessage **message) override;

    void pop_papify_message(PapifyMessage **message, std::int32_t id) override;

private:
    ControlMessageQueue<JobInfoMessage *> *spider2LrtJobQueue_;
    ControlMessageQueue<ParameterMessage *> *lrt2SpiderParamQueue_;
    ControlMessageQueue<TraceMessage *> *traceQueue_;
    ControlMessageQueue<PapifyMessage *> *papifyQueue_;
    NotificationQueue<NotificationMessage> **notificationQueue_;
};

#endif/*PTHREADS_SPIDER_COMMUNICATOR_H*/
