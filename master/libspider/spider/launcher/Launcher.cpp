/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2018)
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
#include "Launcher.h"
#include <SpiderCommunicator.h>

#include <graphs/SRDAG/SRDAGGraph.h>

#include <algorithm>

#include <lrt.h>
#include <PThreadSpiderCommunicator.h>

Launcher Launcher::instance_;

Launcher::Launcher() {
    curNParam_ = 0;
    nLaunched_ = 0;
}

void Launcher::launchVertex(SRDAGVertex *vertex) {
    if (vertex->getState() == SRDAG_EXEC) {
        int slave = vertex->getSlave();
        send_StartJobMsg(slave, vertex);
        nLaunched_++;
        vertex->setState(SRDAG_RUN);
    }
}

Launcher *Launcher::get() {
    return &instance_;
}

void Launcher::send_ResetLrtMsg(int) {
//    auto msg = (Message *) Platform::get()->getSpiderCommunicator()->ctrl_start_send(lrtIx, sizeof(Message));
//    msg->id_ = MSG_RESET_LRT;
//    Platform::get()->getSpiderCommunicator()->ctrl_end_send(lrtIx, sizeof(Message));
}

void Launcher::send_EndIterMsg(int) {
//    auto msg = (Message *) Platform::get()->getSpiderCommunicator()->ctrl_start_send(lrtIx, sizeof(Message));
//    msg->id_ = MSG_END_ITER;
//    Platform::get()->getSpiderCommunicator()->ctrl_end_send(lrtIx, sizeof(Message));
}

void Launcher::send_ClearTimeMsg(int) {
//    auto msg = (ClearTimeMessage *) Platform::get()->getSpiderCommunicator()->ctrl_start_send(lrtIx,
//                                                                                              sizeof(ClearTimeMessage));
//    msg->id_ = MSG_CLEAR_TIME;
//    Platform::get()->getSpiderCommunicator()->ctrl_end_send(lrtIx, sizeof(ClearTimeMessage));
}

void Launcher::send_StartJobMsg(int lrtIx, SRDAGVertex *vertex) {
    /** retreive Infos for msg */
    int nParams = 0;
    switch (vertex->getType()) {
        case SRDAG_NORMAL:
            nParams = vertex->getNInParam();
            break;
        case SRDAG_FORK:
        case SRDAG_JOIN:
            nParams = 2 + vertex->getNConnectedInEdge() + vertex->getNConnectedOutEdge();
            break;
        case SRDAG_ROUNDBUFFER:
        case SRDAG_BROADCAST:
            nParams = 2;
            break;
        case SRDAG_INIT:
        case SRDAG_END:
            nParams = 3;
            break;
    }

//    auto sizeFifo = sizeof(Fifo);
    auto inFifos = CREATE_MUL(ARCHI_STACK, vertex->getNConnectedInEdge(), Fifo);
    auto outFifos = CREATE_MUL(ARCHI_STACK, vertex->getNConnectedOutEdge(), Fifo);
    auto inParams = CREATE_MUL(ARCHI_STACK, nParams, Param);

    auto msg = CREATE(ARCHI_STACK, JobMessage);
    msg->srdagID_ = vertex->getId();
    msg->specialActor_ = vertex->getType() != SRDAG_NORMAL;
    msg->fctID_ = vertex->getFctId();
    msg->traceEnabled_ = Spider::getTraceEnabled();

    msg->nEdgeIN_ = vertex->getNConnectedInEdge();
    msg->nEdgeOUT_ = vertex->getNConnectedOutEdge();
    msg->nParamIN_ = nParams;
    msg->nParamOUT_ = vertex->getNOutParam();
    msg->inFifos_ = inFifos;
    msg->outFifos_ = outFifos;
    msg->inParams_ = inParams;

    for (int i = 0; i < vertex->getNConnectedInEdge(); i++) {
        SRDAGEdge *edge = vertex->getInEdge(i);
        inFifos[i].alloc = edge->getAlloc();
        inFifos[i].size = edge->getRate();
        inFifos[i].blkLrtIx = edge->getSrc()->getSlave();
        inFifos[i].blkLrtJobIx = edge->getSrc()->getSlaveJobIx();
    }

    for (int i = 0; i < vertex->getNConnectedOutEdge(); i++) {
        SRDAGEdge *edge = vertex->getOutEdge(i);
        outFifos[i].alloc = edge->getAlloc();
        outFifos[i].size = edge->getRate();
    }

    switch (vertex->getType()) {
        case SRDAG_NORMAL:
            for (int i = 0; i < nParams; i++) {
                inParams[i] = vertex->getInParam(i);
            }
            break;
        case SRDAG_FORK:
            inParams[0] = vertex->getNConnectedInEdge();
            inParams[1] = vertex->getNConnectedOutEdge();
            inParams[2] = vertex->getInEdge(0)->getRate();
            for (int i = 0; i < vertex->getNConnectedOutEdge(); i++) {
                inParams[3 + i] = vertex->getOutEdge(i)->getRate();
            }
            break;
        case SRDAG_JOIN:
            inParams[0] = vertex->getNConnectedInEdge();
            inParams[1] = vertex->getNConnectedOutEdge();
            inParams[2] = vertex->getOutEdge(0)->getRate();
            for (int i = 0; i < vertex->getNConnectedInEdge(); i++) {
                inParams[3 + i] = vertex->getInEdge(i)->getRate();
            }
            break;
        case SRDAG_ROUNDBUFFER:
            inParams[0] = vertex->getInEdge(0)->getRate();
            inParams[1] = vertex->getOutEdge(0)->getRate();
            break;
        case SRDAG_BROADCAST:
            inParams[0] = vertex->getInEdge(0)->getRate();
            inParams[1] = vertex->getNConnectedOutEdge();
            break;
        case SRDAG_INIT:
            inParams[0] = vertex->getOutEdge(0)->getRate();
            // Set persistence property
            inParams[1] = vertex->getInParam(0);
            // Set memory address
            inParams[2] = vertex->getInParam(1);

            break;
        case SRDAG_END:
            inParams[0] = vertex->getInEdge(0)->getRate();
            // Set persistence property
            inParams[1] = vertex->getInParam(0);
            // Set memory address
            inParams[2] = vertex->getInParam(1);
            break;
    }

    curNParam_ += vertex->getNOutParam();

    auto spiderCommunicator = (PThreadSpiderCommunicator *) Platform::get()->getSpiderCommunicator();
    /** Push the job message **/
    auto jobID = spiderCommunicator->push_job_message(&msg);
    NotificationMessage notificationMessage(JOB_NOTIFICATION, JOB_ADD, jobID);
    /** Send notification **/
    spiderCommunicator->push_notification(lrtIx, &notificationMessage);
//    fprintf(stderr, "INFO: LRT: %d -- job pushed.\n", lrtIx);
}

void Launcher::resolveParams(Archi */*archi*/, SRDAGGraph *topDag) {
//    int slave = 0;
//    while (curNParam_ != 0) {
//        ParamValueMessage *msg;
//        if (Platform::get()->getSpiderCommunicator()->ctrl_start_recv(slave, (void **) (&msg))) {
//            if (msg->id_ != MSG_PARAM_VALUE)
//                throw std::runtime_error("Unexpected Msg received\n");
//            SRDAGVertex *cfgVertex = topDag->getVertexFromIx(msg->srdagID_);
//            for (int j = 0; j < cfgVertex->getNOutParam(); j++) {
//                int *param = cfgVertex->getOutParam(j);
//                *param = msg->params_[j];
//                if (Spider::getVerbose())
//                    printf("Recv param %s = %d\n", cfgVertex->getReference()->getOutParam(j)->getName(), *param);
//            }
//            curNParam_ -= cfgVertex->getNOutParam();
//            Platform::get()->getSpiderCommunicator()->ctrl_end_recv(slave);
//        }
//        slave = (slave + 1) % archi->getNPE();
//    }
    while (curNParam_) {
        NotificationMessage message;
        if (Platform::get()->getSpiderCommunicator()->pop_notification(Platform::get()->getNLrt(), &message, true)) {
            if (message.getType() == JOB_NOTIFICATION && message.getSubType() == JOB_SENT_PARAM) {
                ParameterMessage *parameterMessage;
                Platform::get()->getSpiderCommunicator()->pop_parameter_message(&parameterMessage, message.getIndex());
                SRDAGVertex *vertex = topDag->getVertexFromIx(parameterMessage->getVertexID());
                if (vertex->getNOutParam() != parameterMessage->getNParam()) {
                    throw std::runtime_error(
                            "ERROR: number of parameters received not consistent with expected value.");
                }
                auto *receivedParams = parameterMessage->getParams();
                for (int i = 0; i < vertex->getNOutParam(); ++i) {
                    auto *param = vertex->getOutParam(i);
                    (*param) = receivedParams[i];
                    if (Spider::getVerbose()) {
                        auto *parameterName = vertex->getReference()->getOutParam(i)->getName();
                        fprintf(stderr, "INFO: Parameter: %s -- Value: %ld\n", parameterName, receivedParams[i]);
                    }
                }
                curNParam_ -= vertex->getNOutParam();
                StackMonitor::free(ARCHI_STACK, parameterMessage);
            } else {
                /** Push back the message in the queue, it will be treated later **/
                Platform::get()->getSpiderCommunicator()->push_notification(Platform::get()->getNLrt(), &message);
            }
        }
    }
}

void Launcher::sendTraceSpider(TraceSpiderType type, Time start, Time end) {
    auto msgTrace = (TraceMessage *) Platform::get()->getSpiderCommunicator()->trace_start_send(sizeof(TraceMessage));

    msgTrace->id_ = TRACE_SPIDER;
    msgTrace->spiderTask_ = type;
    msgTrace->srdagID_ = -1;
    msgTrace->start_ = start;
    msgTrace->end_ = end;
    msgTrace->lrtID_ = Platform::get()->getLrt()->getIx();

    Platform::get()->getSpiderCommunicator()->trace_end_send(sizeof(TraceMsgType));
    nLaunched_++;
}

int Launcher::getNLaunched() {
    return nLaunched_;
}

void Launcher::rstNLaunched() {
    nLaunched_ = 0;
}
