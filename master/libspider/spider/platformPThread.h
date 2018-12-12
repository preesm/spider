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
#ifndef PLATFORM_PTHREAD_H
#define PLATFORM_PTHREAD_H

#include "platform.h"
#include <csignal>

#include "TraceQueue.h"
#include "ControlQueue.h"

#ifdef PAPI_AVAILABLE

#include "../papify/PapifyAction.h"

#endif

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

#include <queue>
#include <pthread.h>
#include <PThreadLrtCommunicator.h>

#include <map>
#include <ControlMessageQueue.h>
#include <NotificationQueue.h>
#include <SpiderException.h>


class PlatformPThread : public Platform {
public:
    /**
     * @brief Constructor
     * @param config Reference to the config
     */
    explicit PlatformPThread(SpiderConfig &config);

    /**
     * @brief Destructor
     */
    ~PlatformPThread() override;

    /** File Handling */
    FILE *fopen(const char *name) override;

    void fprintf(FILE *id, const char *fmt, ...) override;

    void fclose(FILE *id) override;

    /** Shared Memory Handling */
    void *virt_to_phy(void *address) override;

    int getMinAllocSize() override;

    int getCacheLineSize() override;

    /** Time Handling */
    void rstTime() override;

    void rstTime(ClearTimeMessage *msg) override;

    Time getTime() override;

    void rstJobIx() override;

    void rstJobIxRecv() override;

    /** Platform getter/setter */
    /**
     * @brief Get current LRT
     * @return Pointer to current LRT class
     */
    inline LRT *getLrt() override {
        return lrt_[getThreadNumber()];
    }

    /**
     * @brief Get current LRT ID
     * @return ID of current LRT
     */
    inline int getLrtIx() override {
        return getThreadNumber();
    }

    /**
     * @brief Get number of LRT
     * @return Number of LRT
     */
    inline int getNLrt() override {
        return nLrt_;
    }

    /**
     * @brief Get current LRT communicator
     * @return LRT current communicator
     */
    inline LrtCommunicator *getLrtCommunicator() override {
        return lrtCom_[getThreadNumber()];
    }

    /**
     * @brief Get Spider communicator
     * @return spider communicator
     */
    inline SpiderCommunicator *getSpiderCommunicator() override {
        if (spiderCom_) {
            return spiderCom_;
        }
        else {
            throwSpiderException("SpiderCommunicator uninitialized.\n");
        }
    }

    inline void setStack(SpiderStack id, Stack *stack) override;

    inline Stack *getStack(SpiderStack id) override;

    inline void registerLRT(int lrtID, pthread_t &thread) {
        lrtThreadsArray[lrtID] = thread;
    }

#ifdef PAPI_AVAILABLE
    inline std::map<lrtFct, PapifyAction *> &getPapifyInfo() {
        return papifyJobInfo;
    }
#endif

private:
    inline int getThreadNumber() {
        for (unsigned int i = 0; i < nLrt_; i++) {
            if (pthread_equal(lrtThreadsArray[i], pthread_self()) != 0)
                return i;
        }
        throwSpiderException("Thread ID not found: %lu.", pthread_self());
    }

    static Time mappingTime(int nActors, int nPe);
    void initStacks(SpiderConfig &config);

    unsigned int nLrt_;
    pthread_t *lrtThreadsArray;

    /** Stack pointers */
    Stack *stackPisdf;
    Stack *stackSrdag;
    Stack *stackTransfo;
    Stack *stackArchi;
    Stack **stackLrt;

    ControlMessageQueue<JobInfoMessage *> *spider2LrtJobQueue_;
    ControlMessageQueue<ParameterMessage *> *lrt2SpiderParamQueue_;
    ControlMessageQueue<TraceMessage *> *traceQueue_;
    NotificationQueue<NotificationMessage> **lrtNotificationQueues_;
    NotificationQueue<JobNotificationMessage> **lrt2LRTDataNotificationQueue_;

    DataQueues *dataQueues_;

    LRT **lrt_;
    LrtCommunicator **lrtCom_;
    SpiderCommunicator *spiderCom_;

    pthread_t *thread_lrt_;
#ifdef PAPI_AVAILABLE
    // Papify information
    std::map<lrtFct, PapifyAction *> papifyJobInfo;
#endif
};

inline void PlatformPThread::setStack(SpiderStack id, Stack *stack) {
    switch (id) {
        case PISDF_STACK :
            stackPisdf = stack;
            break;
        case SRDAG_STACK :
            stackSrdag = stack;
            break;
        case TRANSFO_STACK :
            stackTransfo = stack;
            break;
        case ARCHI_STACK :
            stackArchi = stack;
            break;
        case LRT_STACK :
            stackLrt[getThreadNumber()] = stack;
            break;
        default :
            throwSpiderException("Invalid stack index: %d.", id);
    }
}

inline Stack *PlatformPThread::getStack(SpiderStack id) {
    switch (id) {
        case PISDF_STACK :
            return stackPisdf;
        case SRDAG_STACK :
            return stackSrdag;
        case TRANSFO_STACK :
            return stackTransfo;
        case ARCHI_STACK :
            return stackArchi;
        case LRT_STACK :
            return stackLrt[getThreadNumber()];
        default :
            throwSpiderException("Invalid stack index: %d.", id);
    }
}


/**
 * @brief Stucture for the initialization of a pthread LRT
 */
typedef struct LRTInfo {
    LRT *lrt;
    lrtFct *fcts;
    int lrtID;
    int nFcts;
    int coreAffinity;
    bool usePapify;
    StackConfig lrtStack;
    PlatformPThread *platform;
    pthread_barrier_t *pthreadBarrier;
}LRTInfo;


#endif/*PLATFORM_PTHREADS_H*/
