/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
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
#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <spider.h>

typedef enum {
    MSG_START_JOB = 1,
    MSG_PARAM_VALUE = 2,
    MSG_CLEAR_TIME = 3,
    MSG_RESET_LRT = 4,
    MSG_END_ITER = 5,
    MSG_STOP_LRT = 6
} CtrlMsgType;

typedef enum {
    TRACE_JOB = 1,
    TRACE_SPIDER = 2
} TraceMsgType;

typedef enum {
    TRACE_SPIDER_GRAPH = 1,
    TRACE_SPIDER_ALLOC = 2,
    TRACE_SPIDER_SCHED = 3,
    TRACE_SPIDER_OPTIM = 4,
    TRACE_SPIDER_TMP0,
    TRACE_SPIDER_TMP1,
    TRACE_SPIDER_TMP2,
    TRACE_SPIDER_TMP3
} TraceSpiderType;


typedef struct {
    unsigned long alloc:32;
    unsigned long size:32;
    unsigned long blkLrtIx:32;
    unsigned long blkLrtJobIx:32;
} Fifo;


class Message {
public:
    std::uint32_t id_;
};

class ClearTimeMessage : public Message {
public:
    struct timespec timespec_;
};


class JobMessage : public Message {
public:
    bool specialActor_;
    bool traceEnabled_;
    unsigned long srdagID_;
    unsigned long fctID_;
    unsigned long nbInEdge_;
    unsigned long nbOutEdge_;
    unsigned long nbInParam_;
    unsigned long nbOutParam_;
};

class TraceMessage : public Message {
public:
    unsigned long srdagID_;
    unsigned long spiderTask_;
    unsigned long lrtID_;
    Time start_;
    Time end_;
};

class ParamValueMessage : public Message {
public:
    unsigned long srdagID_;
    Param *params_;
};

#endif/*MESSAGE_H*/
