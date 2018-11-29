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
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#else

#include <unistd.h>

#endif // _WIN32

#ifdef _MSC_VER
#define steady_clock system_clock

#if (_MSC_VER < 1900)
#define snprintf _snprintf
#endif
#endif

#include <pthread.h>


#include <cstring>
#include <cstdio>

#include <platformPThread.h>

#include <stdarg.h>

#include <graphs/Archi/SharedMemArchi.h>

#include <lrt.h>
#include <PThreadSpiderCommunicator.h>
#include <spider.h>

#define PLATFORM_FPRINTF_BUFFERSIZE 200

#define MAX_MSG_SIZE 10*1024

static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];
static struct timespec start;

static void *dataMem;

static std::chrono::time_point<std::chrono::steady_clock> start_steady;

static std::chrono::time_point<std::chrono::steady_clock> origin_steady = std::chrono::steady_clock::now();

static SharedMemArchi *archi_;

pthread_barrier_t pthread_barrier_init_and_end_thread;

void printfSpider(void);

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


PlatformPThread::PlatformPThread(SpiderConfig &config) {
    if (platform_) { throw std::runtime_error("Try to create 2 platforms"); }
    platform_ = this;

    //printfSpider();

    nLrt_ = config.platform.nLrt;

    stackPisdf = 0;
    stackSrdag = 0;
    stackTransfo = 0;
    stackArchi = 0;

    //Global stacks initialisation
    StackMonitor::initStack(PISDF_STACK, config.pisdfStack);
    StackMonitor::initStack(SRDAG_STACK, config.srdagStack);
    StackMonitor::initStack(TRANSFO_STACK, config.transfoStack);
    StackMonitor::initStack(ARCHI_STACK, config.archiStack);

    stackLrt = CREATE_MUL(ARCHI_STACK, nLrt_, Stack*);

    lrt_ = CREATE_MUL(ARCHI_STACK, nLrt_, LRT*);
    lrtCom_ = CREATE_MUL(ARCHI_STACK, nLrt_, LrtCommunicator*);
    thread_ID_tab_ = CREATE_MUL(ARCHI_STACK, nLrt_, pthread_t);


    //allocation des fifos
    spider2LrtQueues_ = CREATE_MUL(ARCHI_STACK, nLrt_, ControlQueue*);
    lrt2SpiderQueues_ = CREATE_MUL(ARCHI_STACK, nLrt_, ControlQueue*);

    for (int i = 0; i < nLrt_; i++) {
        spider2LrtQueues_[i] = CREATE(ARCHI_STACK, ControlQueue)(MAX_MSG_SIZE);
        lrt2SpiderQueues_[i] = CREATE(ARCHI_STACK, ControlQueue)(MAX_MSG_SIZE);
    }

    dataQueues_ = CREATE(ARCHI_STACK, DataQueues)(nLrt_);
    traceQueue_ = CREATE(ARCHI_STACK, TraceQueue)(MAX_MSG_SIZE, nLrt_);

    //declaration des threads et structures de passage de paramètre
    thread_lrt_ = CREATE_MUL(ARCHI_STACK, nLrt_ - 1, pthread_t);
    arg_lrt_ = CREATE_MUL(ARCHI_STACK, nLrt_ - 1, Arg_lrt);

    // TODO use "usePapify" only for monitored LRTs / HW PEs
#ifndef PAPI_AVAILABLE
    // If PAPI is not available on the current platform, force disable it
    if (config.usePapify) {
        printf("WARNING: Spider was not compiled on a platform with PAPI, thus the monitoring is disabled.\n");
    }
    config.usePapify = false;
#else
    if (config.usePapify) {
        // Initializing Papify
        PapifyEventLib *papifyEventLib = new PapifyEventLib();

        // Register Papify actor configuration
        if (config.usePapify) {
            std::map<lrtFct, PapifyConfig *>::iterator it;
            for (it = config.papifyJobInfo.begin(); it != config.papifyJobInfo.end(); ++it) {
                PapifyConfig *papifyConfig = it->second;
                PapifyAction *papifyAction = new PapifyAction(
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

    //TODO have shMem a multiple of getMinAllocSize
    dataMem = malloc(config.platform.shMemSize);

    // Filling up parameters for each threads
    int offsetPe = 0;
    for (int pe = 0; pe < config.platform.nPeType; ++pe) {
        for (int i = 0; i < config.platform.pesPerPeType[pe]; i++) {
            
		    lrtCom_[i + offsetPe] = CREATE(ARCHI_STACK, PThreadLrtCommunicator)(
		            spider2LrtQueues_[i + offsetPe],
		            lrt2SpiderQueues_[i + offsetPe],
		            dataQueues_,
		            traceQueue_);
		    lrt_[i + offsetPe] = CREATE(ARCHI_STACK, LRT)(i);

		    arg_lrt_[i + offsetPe].lrtCom = lrtCom_[i + offsetPe];
		    arg_lrt_[i + offsetPe].lrt = lrt_[i + offsetPe];
		    arg_lrt_[i + offsetPe].shMemSize = config.platform.shMemSize;
		    arg_lrt_[i + offsetPe].fcts = config.platform.fcts;
		    arg_lrt_[i + offsetPe].nLrtFcts = config.platform.nLrtFcts;
		    arg_lrt_[i + offsetPe].index = i + offsetPe;
		    arg_lrt_[i + offsetPe].nLrt = nLrt_;
		    arg_lrt_[i + offsetPe].instance = this;
            arg_lrt_[i + offsetPe].coreAffinity = config.platform.coreAffinities[pe][i];
		    arg_lrt_[i + offsetPe].lrtStack.name = config.lrtStack.name;
		    arg_lrt_[i + offsetPe].lrtStack.type = config.lrtStack.type;
		    arg_lrt_[i + offsetPe].lrtStack.start = (void *) ((char *) config.lrtStack.start + (i + offsetPe) * config.lrtStack.size / nLrt_);
		    arg_lrt_[i + offsetPe].lrtStack.size = config.lrtStack.size / nLrt_;
		    arg_lrt_[i + offsetPe].usePapify = config.usePapify;
		}
    }

    thread_ID_tab_[0] = pthread_self();
    pthread_barrier_init(&pthread_barrier_init_and_end_thread, NULL, nLrt_);

    //Lancement des threads
    for (int i = 1; i < nLrt_; i++) {
        pthread_create(&thread_lrt_[i - 1], NULL, &lrtPThread_helper, &arg_lrt_[i]);
    }

    //waiting for every threads to register itself in thread_ID_tab_
    pthread_barrier_wait(&pthread_barrier_init_and_end_thread);

    //Declaration des stacks spécific au thread
    StackConfig lrtStackConfig;
    lrtStackConfig.name = config.lrtStack.name;
    lrtStackConfig.type = config.lrtStack.type;
    lrtStackConfig.start = config.lrtStack.start;
    lrtStackConfig.size = config.lrtStack.size / nLrt_;
    StackMonitor::initStack(LRT_STACK, lrtStackConfig);

    /** Initialize shared memory */
    memset(dataMem, 0, config.platform.shMemSize);

    /** Initialize LRT and Communicators */
    spiderCom_ = CREATE(ARCHI_STACK, PThreadSpiderCommunicator)(
            spider2LrtQueues_,
            lrt2SpiderQueues_,
            traceQueue_);

    // Check papify profiles
#ifdef PAPI_AVAILABLE
    if (config.usePapify) {
        lrt_[0]->setUsePapify();
        std::map<lrtFct, PapifyAction *>::iterator it;
        for (it = papifyJobInfo.begin(); it != papifyJobInfo.end(); ++it) {
            lrt_[0]->addPapifyJobInfo(it->first, it->second);
        }
    }
#endif

    // Wait for all LRTs to be ready to start
    pthread_barrier_wait(&pthread_barrier_init_and_end_thread);


    setAffinity(0);
    lrt_[0]->setFctTbl(config.platform.fcts, config.platform.nLrtFcts);


    /** Create Archi */
    int mainPE = 0;
    int mainPEType = 0;
    archi_ = CREATE(ARCHI_STACK, SharedMemArchi)(
            /* Nb PE */        nLrt_,
            /* Nb PE Type*/ config.platform.nPeType,
            /* Spider Pe */ mainPE,
            /*MappingTime*/ this->mappingTime);

    archi_->setPEType(mainPE, mainPEType);
    archi_->activatePE(mainPE);

    char name[40];
    sprintf(name, "TID %ld (Spider)", thread_ID_tab_[0]);
    archi_->setName(mainPE, name);
    offsetPe = 0;
    for (int pe = 0; pe < config.platform.nPeType; ++pe) {
        archi_->setPETypeRecvSpeed(pe, 1, 10);
        archi_->setPETypeSendSpeed(pe, 1, 10);
        for (int i = 0; i < config.platform.pesPerPeType[pe]; i++) {
            if (pe == mainPEType && (i + offsetPe) == mainPE) {
                continue;
            }
            sprintf(name, "TID %ld (LRT %d)", thread_ID_tab_[i + offsetPe], i + offsetPe);
            archi_->setPEType(i + offsetPe, pe);
            archi_->setName(i + offsetPe, name);
            archi_->activatePE(i + offsetPe);
        }
        offsetPe += config.platform.pesPerPeType[pe];
    }

    Spider::setArchi(archi_);

    this->rstTime();
}

PlatformPThread::~PlatformPThread() {
    for (int lrt = 1; lrt < archi_->getNPE(); lrt++) {
        int size = sizeof(StopLrtMsg);
        StopLrtMsg *msg = (StopLrtMsg *) getSpiderCommunicator()->ctrl_start_send(lrt, size);

        msg->msgIx = MSG_STOP_LRT;

        getSpiderCommunicator()->ctrl_end_send(lrt, size);
    }

    //wait for every LRT to end
    pthread_barrier_wait(&pthread_barrier_init_and_end_thread);


    //wait for each thread to free its lrt and archi stacks and to reach its end
    for (int i = 1; i < nLrt_; i++)
        pthread_join(thread_ID_tab_[i], NULL);

#ifdef PAPI_AVAILABLE
    // Free Papify information
    if (!papifyJobInfo.empty()) {
        std::map<lrtFct, PapifyAction *>::iterator it;
        // Delete the event lib manager
        delete papifyJobInfo.begin()->second->getPapifyEventLib();
    }
#endif

    for (int i = 0; i < nLrt_; i++) {
        lrt_[i]->~LRT();
        ((PThreadLrtCommunicator *) lrtCom_[i])->~PThreadLrtCommunicator();
        StackMonitor::free(ARCHI_STACK, lrt_[i]);
        StackMonitor::free(ARCHI_STACK, lrtCom_[i]);
    }
    ((PThreadSpiderCommunicator *) spiderCom_)->~PThreadSpiderCommunicator();
    StackMonitor::free(ARCHI_STACK, spiderCom_);


    archi_->~SharedMemArchi();

    StackMonitor::free(ARCHI_STACK, archi_);
    StackMonitor::free(ARCHI_STACK, thread_lrt_);
    StackMonitor::free(ARCHI_STACK, arg_lrt_);

    for (int i = 0; i < nLrt_; i++) {
        spider2LrtQueues_[i]->~ControlQueue();
        lrt2SpiderQueues_[i]->~ControlQueue();
        StackMonitor::free(ARCHI_STACK, spider2LrtQueues_[i]);
        StackMonitor::free(ARCHI_STACK, lrt2SpiderQueues_[i]);
    }
    dataQueues_->~DataQueues();
    traceQueue_->~TraceQueue();

    StackMonitor::free(ARCHI_STACK, spider2LrtQueues_);
    StackMonitor::free(ARCHI_STACK, lrt2SpiderQueues_);
    StackMonitor::free(ARCHI_STACK, dataQueues_);
    StackMonitor::free(ARCHI_STACK, traceQueue_);


    //Desallocation des tableaux dynamiques
    StackMonitor::free(ARCHI_STACK, lrt_);
    StackMonitor::free(ARCHI_STACK, lrtCom_);

    StackMonitor::freeAll(LRT_STACK);
    StackMonitor::clean(LRT_STACK);
    StackMonitor::free(ARCHI_STACK, stackLrt);

    StackMonitor::free(ARCHI_STACK, thread_ID_tab_);

    StackMonitor::freeAll(ARCHI_STACK);
    StackMonitor::freeAll(TRANSFO_STACK);
    StackMonitor::freeAll(SRDAG_STACK);
    StackMonitor::freeAll(PISDF_STACK);

    //WARNING : Thread specific stacks have to be cleaned BEFORE exiting threads
    StackMonitor::clean(ARCHI_STACK);
    StackMonitor::clean(TRANSFO_STACK);
    StackMonitor::clean(SRDAG_STACK);
    StackMonitor::clean(PISDF_STACK);

    //Destroying synchronisation barrier
    pthread_barrier_destroy(&pthread_barrier_init_and_end_thread);

    free(dataMem);
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
    if (id != NULL) {
        std::fclose(id);
        id = NULL;
    }
}

void *PlatformPThread::virt_to_phy(void *address) {
    return (void *) ((long) dataMem + (long) address);
}

int PlatformPThread::getCacheLineSize() {
    return 0;
}

int PlatformPThread::getMinAllocSize() {
#ifdef _WIN32
    //workaround because Windows
    return 4096;
#else
    return getpagesize();
#endif
}


void PlatformPThread::rstJobIx() {
    //Sending a msg to all slave LRTs, end of graph iteration
    for (int i = 1; i < nLrt_; i++) {
        EndIterMsg *msg = (EndIterMsg *) getSpiderCommunicator()->ctrl_start_send(i, sizeof(EndIterMsg));
        msg->msgIx = MSG_END_ITER;
        getSpiderCommunicator()->ctrl_end_send(i, sizeof(EndIterMsg));
    }

    //Waiting for slave LRTs to finish their job queue
    for (int i = 1; i < nLrt_; i++) {
        void *msg = NULL;

        do {
            getSpiderCommunicator()->ctrl_start_recv_block(i, &msg);
            if(((UndefinedMsg *) msg)->msgIx == MSG_END_ITER)
                break;
            else
                getSpiderCommunicator()->ctrl_end_recv(i);
        } while (1);
        getSpiderCommunicator()->ctrl_end_recv(i);
    }

    //Sending a msg to all slave LRTs, reset jobIx counter
    for (int i = 1; i < nLrt_; i++) {
        ResetLrtMsg *msg = (ResetLrtMsg *) getSpiderCommunicator()->ctrl_start_send(i, sizeof(ResetLrtMsg));
        msg->msgIx = MSG_RESET_LRT;
        getSpiderCommunicator()->ctrl_end_send(i, sizeof(ResetLrtMsg));
    }

    //reseting master LRT jobIx counter
    Platform::get()->getLrt()->setJobIx(-1);

    //reseting jobTab
    for(int i=0; i<nLrt_; i++){
        lrtCom_[0]->setLrtJobIx(i,-1);
    }

    //Waiting for slave LRTs to reset their jobIx counter
    for (int i = 1; i < nLrt_; i++) {
        void *msg = NULL;
        do {
            getSpiderCommunicator()->ctrl_start_recv_block(i, &msg);
            if(((UndefinedMsg *) msg)->msgIx == MSG_RESET_LRT)
                break;
            else
                getSpiderCommunicator()->ctrl_end_recv(i);
        } while (1);
        getSpiderCommunicator()->ctrl_end_recv(i);
    }
}

/** Time Handling */
void PlatformPThread::rstTime(struct ClearTimeMsg *msg) {
    struct timespec *ts = (struct timespec *) (msg + 1);
    start = *ts;
}

void PlatformPThread::rstTime() {
    start_steady = std::chrono::steady_clock::now();

    start.tv_sec = (start_steady - origin_steady).count() / 1000000000;
    start.tv_nsec = (start_steady - origin_steady).count() - (start_steady - origin_steady).count() / 1000000000;


    for (int lrt = 1; lrt < archi_->getNPE(); lrt++) {
        int size = sizeof(ClearTimeMsg) + sizeof(struct timespec);
        ClearTimeMsg *msg = (ClearTimeMsg *) getSpiderCommunicator()->ctrl_start_send(lrt, size);
        struct timespec *ts = (struct timespec *) (msg + 1);

        msg->msgIx = MSG_CLEAR_TIME;
        *ts = start;

        getSpiderCommunicator()->ctrl_end_send(lrt, size);
    }
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

    return val_steady;
}

Time PlatformPThread::mappingTime(int nActors, int /*nPe*/) {
    return (Time) 1 * nActors;
}


void PlatformPThread::lrtPThread(Arg_lrt *argument_lrt) {

    int index = argument_lrt->index;

    //registering itself in thread_ID_tab_
    thread_ID_tab_[index] = pthread_self();

    //waiting for every threads to register itself in thread_ID_tab_
    pthread_barrier_wait(&pthread_barrier_init_and_end_thread);

    //Declaration des stacks spécific au thread
    StackMonitor::initStack(LRT_STACK, argument_lrt->lrtStack);

    setAffinity(argument_lrt->coreAffinity);

    lrt_[index]->setFctTbl(argument_lrt->fcts, argument_lrt->nLrtFcts);

#ifdef PAPI_AVAILABLE
    // Enable papify if need to
    if (argument_lrt->usePapify) {
        lrt_[index]->setUsePapify();
        std::map<lrtFct, PapifyAction *>::iterator it;
        for (it = papifyJobInfo.begin(); it != papifyJobInfo.end(); ++it) {
            lrt_[index]->addPapifyJobInfo(it->first, new PapifyAction(*it->second, index));
        }
    }
#endif

    // Wait for all LRTs to be created
    pthread_barrier_wait(&pthread_barrier_init_and_end_thread);

    /** launch LRT */
    lrt_[index]->runInfinitly();

    //cleaning thread specific stacks
    StackMonitor::freeAll(LRT_STACK);
    StackMonitor::clean(LRT_STACK);

    //wait for every LRT to end
    pthread_barrier_wait(&pthread_barrier_init_and_end_thread);


    pthread_exit(EXIT_SUCCESS);
}

void *lrtPThread_helper(void *voidArgs) {
    Arg_lrt *args = (Arg_lrt *) voidArgs;
    args->instance->lrtPThread(args);
    return 0;
}


void printfSpider() {

    printf("\n");
    printf("  .;;;;;;;                                              ;;;;;;;.  \n");
    printf(" ;;;;;;;;;                                              ;;;;;;;;; \n");
    printf(";;;;;;;;;;                                              ;;;;;;;;;;\n");
    printf(";;;;;             ;8.                        :@.             ;;;;;\n");
    printf(";;;;;               ,@8                   .@@                ;;;;;\n");
    printf(";;;;;                 L@8                @@:                 ;;;;;\n");
    printf(";;;;;                  .@@;            C@@                   ;;;;;\n");
    printf(";;;;;                    @@0          @@@                    ;;;;;\n");
    printf(";;;;;                    .@@0        @@@                     ;;;;;\n");
    printf(";;;;;                     L@@1      8@@:                     ;;;;;\n");
    printf(";;;;;                      @@@      @@@                      ;;;;;\n");
    printf(";;;;;     :L@@@@@@@@t      f@@      @@,      C@@@@@@@8t,     ;;;;;\n");
    printf(";;;;;            :0@@@@@G   @@     .@@   8@@@@@C.            ;;;;;\n");
    printf(";;;;;                 ;@@@1 @@     :@8 G@@@.                 ;;;;;\n");
    printf(";;;;;                   L@@f8@@@@@@8@t8@@:                   ;;;;;\n");
    printf(";;;;;                     @@i@@@@@@@8G@8                     ;;;;;\n");
    printf(";;;;;      t@@@@@@@@@@@@G. L@@@@@@@@@@; ,8@@@@@@@@@@@@;      ;;;;;\n");
    printf(";;;;;                  C@@@@L@@@@@@@C8@@@@t                  ;;;;;\n");
    printf(";;;;;                       :,@@@@@0:,                       ;;;;;\n");
    printf(";;;;;                      t@@@@@@@@@@:                      ;;;;;\n");
    printf(";;;;;                   .@@@8@@@@@@@@@@@C                    ;;;;;\n");
    printf(";;;;;                  1@@@ @@@0ii0@@f.@@@.                  ;;;;;\n");
    printf(";;;;;                 C@@C  @@,@8G@,@f  @@@;                 ;;;;;\n");
    printf(";;;;;                i@@1   ;@8.  ,8@    0@@                 ;;;;;\n");
    printf(";;;;;                @@:     ;@@@@@@.     C@8                ;;;;;\n");
    printf(";;;;;               i@.         ,,         1@                ;;;;;\n");
    printf(";;;;;               0                        @               ;;;;;\n");
    printf(";;;;;;;;;;                                              ;;;;;;;;;;\n");
    printf(" ;;;;;;;;;                                              ;;;;;;;;; \n");
    printf("  .;;;;;;;                                              ;;;;;;;.  \n");
    printf("\n");
}


