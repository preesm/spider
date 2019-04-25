/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2017 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2018)
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

#include <pthread.h>
#include <chrono>
#include <cinttypes>
#include <cstring>
#include <cstdio>

#include <platformPThread.h>
#include <graphs/Archi/Archi.h>
#include <tools/Rational.h>
#include <lrt.h>

#ifdef _WIN32
#include <windows.h>
#else

#include <unistd.h>
#include <Logger.h>

#endif // _WIN32

#ifdef _MSC_VER
#define steady_clock system_clock

#if (_MSC_VER < 1900)
#define snprintf _snprintf
#endif
#endif

#define PLATFORM_FPRINTF_BUFFERSIZE 2000

#define MAX_MSG_SIZE (10*1024)

static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];
static struct timespec start;

static std::chrono::time_point<std::chrono::steady_clock> start_steady;

static auto origin_steady = std::chrono::steady_clock::now();

pthread_barrier_t pthreadLRTBarrier;

void printfSpider();

static void setAffinity(int cpuId) {
#ifdef WIN32
    fprintf(stdout, "CPU affinity is not supported on Windows platforms. Ignoring argument %d.\n", cpuId);
#else
    cpu_set_t mask;
    int status;

    CPU_ZERO(&mask);
    CPU_SET(cpuId, &mask);
    status = pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
    if (status != 0) {
        perror("sched_setaffinity");
    }
#endif
}


void *lrtPthreadRunner(void *args) {
    auto lrtInfo = (LRTInfo *) (args);
    /** Registering LRT */
    pthread_t self = pthread_self();
    lrtInfo->platform->registerLRT(lrtInfo->lrtID, self);

    /** Waiting for every threads to register itself */
    pthread_barrier_wait(lrtInfo->pthreadBarrier);

    /** Initialize the LRT specific stack */
    lrtInfo->lrt->initStack(lrtInfo->lrtStack);

    /** Set core affinity (if supported by the OS) */
    setAffinity(lrtInfo->coreAffinity);

    /** Set the function table */
    lrtInfo->lrt->setFctTbl(lrtInfo->fcts, lrtInfo->nFcts);

    /** Set communicators **/
    lrtInfo->lrt->setCommunicators();

#ifdef PAPI_AVAILABLE
    /** Enable PAPIFY if needed to */
    if (lrtInfo->usePapify) {
        auto papifyJobInfo = lrtInfo->platform->getPapifyInfo();
        lrtInfo->lrt->setUsePapify();
        for (auto &mapEntry : papifyJobInfo) {
            lrtInfo->lrt->addPapifyJobInfo(mapEntry.first, new PapifyAction(*mapEntry.second, lrtInfo->lrtID));
        }
    }
#endif
    /** Wait for all LRTs to be created */
    pthread_barrier_wait(lrtInfo->pthreadBarrier);
    /** Run LRT */
    lrtInfo->lrt->runInfinitly();
    /** Cleaning LRT specific stack */
    lrtInfo->lrt->cleanStack();
    /** Wait for all LRTs to finish */
    pthread_barrier_wait(lrtInfo->pthreadBarrier);
    /** Exit thread */
    pthread_exit(EXIT_SUCCESS);
}

void PlatformPThread::createAndLaunchThreads() {
#ifdef __USE_GNU
    /** Blocking SIGINT signals ot handle it properly **/
    sigset_t childMask;
    if ((sigemptyset(&childMask) == -1) || (sigaddset(&childMask, SIGINT) == -1)) {
        throwSpiderException("Failed to initialize signal mask.");
    }
    if (pthread_sigmask(SIG_BLOCK, &childMask, nullptr) == -1) {
        throwSpiderException("Failed to block SIGINT signal.");
    }
#else
    Logger::print(LOG_GENERAL, LOG_WARNING,
                  "Windows environment detected, console interruption will not be catched properly.\n");
#endif

    /** Starting the threads */
    auto *archi = Spider::getArchi();
    auto nSpawn = 0;
    for (std::uint32_t i = 0; i < nLrt_; ++i) {
        if (i != archi->getSpiderGRTID()) {
            pthread_create(&thread_lrt_[nSpawn++], nullptr, &lrtPthreadRunner, &lrtInfoArray_[i]);
        }
    }
}


PlatformPThread::PlatformPThread(SpiderConfig &config, SpiderStackConfig &stackConfig) {
    if (platform_) {
        throwSpiderException("Cannot create new platform, a platform already exist.");
    }
    platform_ = this;
    auto *archi = Spider::getArchi();

    nLrt_ = archi->getNPE();

    if (nLrt_ == 0) {
        throwSpiderException("Spider require at least 1 LRT.");
    }

    lrt_ = CREATE_MUL(ARCHI_STACK, nLrt_, LRT*);
    lrtCom_ = CREATE_MUL(ARCHI_STACK, nLrt_, LrtCommunicator*);
    lrtThreadsArray = CREATE_MUL(ARCHI_STACK, nLrt_, pthread_t);


    /** Create the different queues */
    spider2LrtJobQueue_ = CREATE(ARCHI_STACK, ControlMessageQueue<JobInfoMessage *>);
    lrt2SpiderParamQueue_ = CREATE(ARCHI_STACK, ControlMessageQueue<ParameterMessage *>);
    lrtNotificationQueues_ = CREATE_MUL(ARCHI_STACK, nLrt_ + 1, NotificationQueue<NotificationMessage>*);

    for (std::uint32_t i = 0; i < nLrt_ + 1; ++i) {
        lrtNotificationQueues_[i] = CREATE(ARCHI_STACK, NotificationQueue<NotificationMessage>);
    }


    /** FIFOs allocation */
    dataQueues_ = CREATE(ARCHI_STACK, DataQueues)(nLrt_);
    /** TraceQueue allocation */
    traceQueue_ = CREATE(ARCHI_STACK, ControlMessageQueue<TraceMessage *>);
    /** Threads structure */
    thread_lrt_ = CREATE_MUL(ARCHI_STACK, nLrt_ - 1, pthread_t);
    lrtInfoArray_ = CREATE_MUL(ARCHI_STACK, nLrt_, LRTInfo);

    /** Initialize SpiderCommunicator */
    spiderCom_ = CREATE(ARCHI_STACK, PThreadSpiderCommunicator)(
            spider2LrtJobQueue_,
            lrt2SpiderParamQueue_,
            lrtNotificationQueues_,
            traceQueue_);

    // TODO use "usePapify" only for monitored LRTs / HW PEs
#ifndef PAPI_AVAILABLE
    // If PAPI is not available on the current platform_, force disable it
    if (config.usePapify) {
        printf("WARNING: Spider was not compiled on a platform_ with PAPI, thus the monitoring is disabled.\n");
    }
    config.usePapify = false;
#else
    if (config.usePapify) {
        // Initializing Papify
        auto *papifyEventLib = new PapifyEventLib();

        // Register Papify actor configuration
        if (config.usePapify) {
            std::map<lrtFct, PapifyConfig *>::iterator it;
            for (it = config.papifyJobInfo.begin(); it != config.papifyJobInfo.end(); ++it) {
                PapifyConfig *papifyConfig = it->second;
                auto *papifyAction = new PapifyAction(
                        /* componentName */   papifyConfig->peType_,
                        /* PEName */          papifyConfig->peID_,
                        /* actorName */       papifyConfig->actorName_,
                        /* num_events */      papifyConfig->eventSize_,
                        /* all_events_name */ papifyConfig->monitoredEvents_,
                        /* eventSet_Id */     papifyConfig->eventSetID_, papifyConfig->isTiming_, papifyEventLib);
                papifyJobInfo.insert(std::make_pair(it->first, papifyAction));
            }
        }
    }
#endif

    /** Filling up parameters for each threads */
    pthread_barrier_init(&pthreadLRTBarrier, nullptr, nLrt_);
    for (std::uint32_t i = 0; i < archi->getNPE(); i++) {
        lrtCom_[i] = CREATE(ARCHI_STACK, PThreadLrtCommunicator)(
                spider2LrtJobQueue_,
                lrtNotificationQueues_[i],
                dataQueues_);

        lrt_[i] = CREATE(ARCHI_STACK, LRT)(i);

        lrtInfoArray_[i].lrt = lrt_[i];
        lrtInfoArray_[i].fcts = config.fcts;
        lrtInfoArray_[i].nFcts = config.nLrtFcts;
        lrtInfoArray_[i].lrtID = i;
        lrtInfoArray_[i].platform = this;
        lrtInfoArray_[i].coreAffinity = archi->getPEFromSpiderID(i)->getHardwareID();
        lrtInfoArray_[i].pthreadBarrier = &pthreadLRTBarrier;
        /** Stack related information */
        lrtInfoArray_[i].lrtStack.name = stackConfig.lrtStack.name;
        lrtInfoArray_[i].lrtStack.type = stackConfig.lrtStack.type;
        lrtInfoArray_[i].lrtStack.start = (void *) ((char *) stackConfig.lrtStack.start +
                                                    (i) * stackConfig.lrtStack.size / nLrt_);
        lrtInfoArray_[i].lrtStack.size = stackConfig.lrtStack.size / nLrt_;
        /** Papify related information */
        lrtInfoArray_[i].usePapify = config.usePapify;
    }
    auto spiderGRTID = archi->getSpiderGRTID();
    lrtThreadsArray[spiderGRTID] = pthread_self();

    /** Starting threads **/
    createAndLaunchThreads();

    //waiting for every threads to register itself in lrtThreadsArray
    pthread_barrier_wait(&pthreadLRTBarrier);

    /* == Init GRT Stack == */
    StackMonitor::initStack(LRT_STACK, lrtInfoArray_[spiderGRTID].lrtStack);

    // Check papify profiles
#ifdef PAPI_AVAILABLE
    if (config.usePapify) {
        lrt_[spiderGRTID]->setUsePapify();
        std::map<lrtFct, PapifyAction *>::iterator it;
        for (it = papifyJobInfo.begin(); it != papifyJobInfo.end(); ++it) {
            lrt_[spiderGRTID]->addPapifyJobInfo(it->first, it->second);
        }
    }
#endif

    // Wait for all LRTs to be ready to start
    pthread_barrier_wait(&pthreadLRTBarrier);


    setAffinity(spiderGRTID);
    lrt_[spiderGRTID]->setFctTbl(config.fcts, config.nLrtFcts);

    /** Set Communicators of master **/
    lrt_[spiderGRTID]->setCommunicators();

    this->rstTime();
}

PlatformPThread::~PlatformPThread() {
    auto spiderCommunicator = getSpiderCommunicator();
    auto *archi = Spider::getArchi();
    for (std::uint32_t i = 0; i < nLrt_; ++i) {
        if (i != archi->getSpiderGRTID()) {
            NotificationMessage message(LRT_NOTIFICATION, LRT_STOP, getLrtIx());
            spiderCommunicator->push_notification(i, &message);
        }
    }

    //wait for every LRT to end
    pthread_barrier_wait(&pthreadLRTBarrier);

    //wait for each thread to free its lrt and archi stacks and to reach its end
    for (std::uint32_t i = 0; i < nLrt_; i++) {
        if (i != archi->getSpiderGRTID()) {
            pthread_join(lrtThreadsArray[i], nullptr);
        }
    }
    /* == Free LRT stack of GRT == */
    StackMonitor::freeAll(LRT_STACK);
    StackMonitor::clean(LRT_STACK);

#ifdef PAPI_AVAILABLE
    /** Free Papify information */
    if (!papifyJobInfo.empty()) {
        std::map<lrtFct, PapifyAction *>::iterator it;
        // Delete the event lib manager
        delete papifyJobInfo.begin()->second->getPapifyEventLib();
    }
#endif

    for (std::uint32_t i = 0; i < nLrt_; i++) {
        lrt_[i]->~LRT();
        StackMonitor::free(ARCHI_STACK, lrt_[i]);
        StackMonitor::free(ARCHI_STACK, lrtCom_[i]);
    }
    StackMonitor::free(ARCHI_STACK, spiderCom_);
    StackMonitor::free(ARCHI_STACK, thread_lrt_);
    StackMonitor::free(ARCHI_STACK, lrtInfoArray_);

    for (std::uint32_t i = 0; i <= nLrt_; i++) {
        lrtNotificationQueues_[i]->~NotificationQueue();
        StackMonitor::free(ARCHI_STACK, lrtNotificationQueues_[i]);
    }

    spider2LrtJobQueue_->~ControlMessageQueue();
    lrt2SpiderParamQueue_->~ControlMessageQueue();
    traceQueue_->~ControlMessageQueue();
    StackMonitor::free(ARCHI_STACK, lrtNotificationQueues_);
    StackMonitor::free(ARCHI_STACK, spider2LrtJobQueue_);
    StackMonitor::free(ARCHI_STACK, lrt2SpiderParamQueue_);
    StackMonitor::free(ARCHI_STACK, traceQueue_);


    dataQueues_->~DataQueues();
    StackMonitor::free(ARCHI_STACK, dataQueues_);


    //Desallocation des tableaux dynamiques
    StackMonitor::free(ARCHI_STACK, lrt_);
    StackMonitor::free(ARCHI_STACK, lrtCom_);

    StackMonitor::free(ARCHI_STACK, lrtThreadsArray);

    //Destroying synchronisation barrier
    pthread_barrier_destroy(&pthreadLRTBarrier);
}

/** File Handling */
FILE *PlatformPThread::fopen(const char *name) {
    return std::fopen(name, "w+");
}

void PlatformPThread::fprintf(FILE *id, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

#ifdef _WIN32
    int n = _vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);
#else
    int n = vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);
#endif

    if (n >= PLATFORM_FPRINTF_BUFFERSIZE) {
        printf("PLATFORM_FPRINTF_BUFFERSIZE too small\n");
    }

    for (int i = 0; i < n; i++) fputc(buffer[i], id);
}

void PlatformPThread::fclose(FILE *id) {
    if (id != nullptr) {
        std::fclose(id);
    }
}

int PlatformPThread::getCacheLineSize() {
    return 0;
}

long PlatformPThread::getMinAllocSize() {
#ifdef _WIN32
    //workaround because Windows
    return 4096;
#else
    return sysconf(_SC_PAGESIZE);
#endif
}

void PlatformPThread::rstJobIxRecv() {
    auto *spiderCommunicator = Platform::get()->getSpiderCommunicator();
    NotificationMessage finishedMessage;
    /** Wait for LRTs to finish their jobs **/
    auto *archi = Spider::getArchi();
    auto nPEToWait = archi->getNActivatedPE() - 1;
    std::uint32_t nFinishedPE = 0;
    while (nFinishedPE < nPEToWait) {
        spiderCommunicator->pop_notification(Platform::get()->getNLrt(), &finishedMessage, true);
        if (finishedMessage.getType() == LRT_NOTIFICATION &&
            finishedMessage.getSubType() == LRT_FINISHED_ITERATION) {
            Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- received end signal from LRT: %d.\n", getLrtIx(),
                          finishedMessage.getLRTID());
            nFinishedPE++;
        } else {
            /** Save the notification for later **/
            spiderCommunicator->push_notification(Platform::get()->getNLrt(), &finishedMessage);
        }
    }
}

void PlatformPThread::rstJobIx() {
}

/** Time Handling */
void PlatformPThread::rstTime(ClearTimeMessage *msg) {
    start = msg->timespec_;
}

void PlatformPThread::rstTime() {
    start_steady = std::chrono::steady_clock::now();

    start.tv_sec = (start_steady - origin_steady).count() / 1000000000;
    start.tv_nsec = (start_steady - origin_steady).count() - (start_steady - origin_steady).count() / 1000000000;
}

Time PlatformPThread::getTime() {
    std::chrono::time_point<std::chrono::steady_clock> ts_steady = std::chrono::steady_clock::now();
    long long val_steady = (ts_steady - start_steady).count();

#ifdef _WIN32
    // Spider will think something went bad if returned time is 0, so in such case we're setting it to 1 because time in Windows is bad
    if (val_steady == 0){
        val_steady++;
    }
#endif // _WIN32

    return static_cast<Time>(val_steady);
}