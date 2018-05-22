/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2017) :
 *
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
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

#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sched.h>
#include <errno.h>

#include <tools/Stack.h>
#include <graphs/Archi/SharedMemArchi.h>
#include <monitor/StackMonitor.h>

#include <lrt.h>
#include <spider.h>
#include <PThreadLrtCommunicator.h>
#include <PThreadSpiderCommunicator.h>

#define PLATFORM_FPRINTF_BUFFERSIZE 200

#define MAX_MSG_SIZE 10*1024

static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];
static struct timespec start;

static void* jobTab;
static void* dataMem;

static std::chrono::time_point<std::chrono::steady_clock> start_steady;

static std::chrono::time_point<std::chrono::steady_clock> origin_steady = std::chrono::steady_clock::now();



static SharedMemArchi* archi_;

pthread_barrier_t pthread_barrier_init_and_end_thread;

void printfSpider(void);

static void setAffinity(int cpuId){

	cpu_set_t mask;
	int status;

	CPU_ZERO(&mask);
	CPU_SET(cpuId, &mask);
	status = pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
	if (status != 0)
	{
		perror("sched_setaffinity");
	}
}

PlatformPThread::PlatformPThread(int nLrt, int shMemSize, lrtFct* fcts, int nLrtFcts, StackConfig archiStack, StackConfig lrtStack,
	StackConfig pisdfStack, StackConfig srdagStack, StackConfig transfoStack, bool usePapify, std::map<lrtFct, PapifyConfig*> &jobPapifyActions) {

	if (platform_) throw std::runtime_error("Try to create 2 platforms");
	platform_ = this;

	//printfSpider();

	nLrt_ = nLrt;

	stackPisdf = 0;
	stackSrdag = 0;
	stackTransfo = 0;
	stackArchi = (Stack**)malloc(nLrt_*sizeof(Stack*));
	stackLrt = (Stack**)malloc(nLrt_*sizeof(Stack*));

	lrt_ = (LRT**)malloc(nLrt_*sizeof(LRT*));
	lrtCom_ = (LrtCommunicator**)malloc(nLrt_*sizeof(PThreadLrtCommunicator*));

	thread_ID_tab_ = (pthread_t*)malloc(nLrt_*sizeof(pthread_t));

	//Global stacks initialisation
	StackMonitor::initStack(PISDF_STACK, pisdfStack);
	StackMonitor::initStack(SRDAG_STACK, srdagStack);
	StackMonitor::initStack(TRANSFO_STACK, transfoStack);


	//allocation des fifos
	fifoSpidertoLRT = (std::queue<unsigned char>**) malloc(nLrt_ * sizeof(std::queue<unsigned char>*));
	fifoLRTtoSpider = (std::queue<unsigned char>**) malloc(nLrt_ * sizeof(std::queue<unsigned char>*));

	for (int i = 0; i < nLrt_; i++){
		fifoSpidertoLRT[i] = new std::queue<unsigned char>;
		fifoLRTtoSpider[i] = new std::queue<unsigned char>;
	}

	//declaration des threads et structures de passage de paramètre
	pthread_t* thread_lrt = (pthread_t*)malloc((nLrt_ - 1)*sizeof(pthread_t));
	Arg_lrt* arg_lrt = (Arg_lrt*)malloc((nLrt_ - 1)*sizeof(Arg_lrt));

	//Declaration tableau de semaphore
	semFifoSpidertoLRT = (sem_t*)malloc(nLrt_ * sizeof(sem_t));
	semFifoLRTtoSpider = (sem_t*)malloc(nLrt_ * sizeof(sem_t));

	//Initialisation des semaphores
	for (int i = 0; i<nLrt_; i++){
		sem_init(&semFifoSpidertoLRT[i], 0, 1);
		sem_init(&semFifoLRTtoSpider[i], 0, 1);
	}
	sem_init(&semTrace, 0, 1);

	if (usePapify) {
        // Initializing papify
        event_init_multiplex();
	}

    // TODO use "usePapify" only for monitored LRTs / HW PEs
#ifndef PAPI_AVAILABLE
    // If PAPI is not available on the current platform, force disable it
    if (usePapify) {
        printf("WARNING: Spider was not compiled on a platform with PAPI, thus the monitoring is disabled.\n");
    }
    usePapify = false;
#endif
    // Filling up parameters for each threads
	for (int i = 1; i<nLrt_; i++){
		arg_lrt[i - 1].fifoSpidertoLRT = fifoSpidertoLRT[i];
		arg_lrt[i - 1].fifoLRTtoSpider = fifoLRTtoSpider[i];
		arg_lrt[i - 1].fifoTrace = &fifoTrace;
		arg_lrt[i - 1].semTrace = &semTrace;
		arg_lrt[i - 1].semFifoSpidertoLRT = &semFifoSpidertoLRT[i];
		arg_lrt[i - 1].semFifoLRTtoSpider = &semFifoLRTtoSpider[i];
		arg_lrt[i - 1].shMemSize = shMemSize;
		arg_lrt[i - 1].fcts = fcts;
		arg_lrt[i - 1].nLrtFcts = nLrtFcts;
		arg_lrt[i - 1].index = i;
		arg_lrt[i - 1].nLrt = nLrt_;
		arg_lrt[i - 1].instance = this;
		arg_lrt[i - 1].archiStack = archiStack;
		arg_lrt[i - 1].lrtStack = lrtStack;
		arg_lrt[i - 1].usePapify = usePapify;
		arg_lrt[i - 1].jobPapifyActions = jobPapifyActions;
	}


	//TODO have shMem a multiple of getMinAllocSize
	jobTab = malloc(shMemSize);

	dataMem = (void*) ((long)jobTab + sizeof(unsigned int)*nLrt_);

	thread_ID_tab_[0] = pthread_self();
	pthread_barrier_init(&pthread_barrier_init_and_end_thread, NULL, nLrt_);

	// Starting the threads
	for (int i = 1; i < nLrt_; ++i) {
	    pthread_create(&thread_lrt[i - 1], NULL, &lrtPThread_helper, &arg_lrt[i - 1]);
	}

	// Waiting for every threads to register itself in thread_ID_tab_
	pthread_barrier_wait(&pthread_barrier_init_and_end_thread);

	// Declaring thread specific stacks
	archiStack.size /= nLrt_;
	StackMonitor::initStack(ARCHI_STACK, archiStack);

	lrtStack.size /= nLrt_;
	StackMonitor::initStack(LRT_STACK, lrtStack);

	// Initialize shared memory
	memset(jobTab, 0, shMemSize);

	// Initialize LRT and Communicators
	spiderCom_ = CREATE(ARCHI_STACK, PThreadSpiderCommunicator)(
		MAX_MSG_SIZE,
		nLrt_,
		&semTrace,
		semFifoSpidertoLRT,
		semFifoLRTtoSpider,
		&fifoTrace,
		&fifoTrace);


	for (int i = 0;i < nLrt_;i++) {
        PThreadSpiderCommunicator* spiderCom = (PThreadSpiderCommunicator*) spiderCom_;
	    spiderCom->setLrtCom(i, fifoLRTtoSpider[i], fifoSpidertoLRT[i]);
	}


	lrtCom_[0] = CREATE(ARCHI_STACK, PThreadLrtCommunicator)(
		MAX_MSG_SIZE,
		fifoSpidertoLRT[0],
		fifoLRTtoSpider[0],
		&fifoTrace,
		&semTrace,
		&semFifoSpidertoLRT[0],
		&semFifoLRTtoSpider[0],
		jobTab,
		dataMem);

	lrt_[0] = CREATE(ARCHI_STACK, LRT)(0);

	// Check papify profiles
    if (usePapify) {
        lrt_[0]->setUsePapify();
        std::map<lrtFct , PapifyConfig*>::iterator it;
        for (it = jobPapifyActions.begin(); it != jobPapifyActions.end(); ++it) {
            // TODO check that LRT_STACK is large enough
            papify_action_s* papifyAction =CREATE(LRT_STACK, papify_action_s);
            PapifyConfig* papifyConfig = it->second;
            configure_papify(papifyAction,
                    /* componentName */   papifyConfig->peType_,
                    /* PEName */          papifyConfig->peID_,
                    /* actorName */       papifyConfig->actorName_,
                    /* num_events */      papifyConfig->eventSize_,
                    /* all_events_name */ papifyConfig->monitoredEvents_,
                    /* eventSet_Id */     papifyConfig->eventSetID_);
            papifyAction->isTiming = papifyConfig->isTiming_;
            lrt_[0]->addPapifyJobInfo(it->first, papifyAction);
        }
    }

	// Wait for all LRTs to be ready to start
	pthread_barrier_wait(&pthread_barrier_init_and_end_thread);


	setAffinity(0);
	lrt_[0]->setFctTbl(fcts, nLrtFcts);


	/** Create Archi */
	archi_ = CREATE(ARCHI_STACK, SharedMemArchi)(
		/* Nb PE */		nLrt_,
		/* Nb PE Type*/ 1,
		/* Spider Pe */ 0,
		/*MappingTime*/ this->mappingTime);

	archi_->setPETypeRecvSpeed(0, 1, 10);
	archi_->setPETypeSendSpeed(0, 1, 10);
	archi_->setPEType(0, 0);
	archi_->activatePE(0);

	char name[40];
	sprintf(name, "TID %ld (Spider)", thread_ID_tab_[0]);
	archi_->setName(0, name);
	for (int i = 1; i<nLrt_; i++){
		sprintf(name, "TID %ld (LRT %d)", thread_ID_tab_[i], i);
		archi_->setPEType(i, 0);
		archi_->setName(i, name);
		archi_->activatePE(i);
	}
	Spider::setArchi(archi_);

	free(thread_lrt);
	free(arg_lrt);

	this->rstTime();
}

PlatformPThread::~PlatformPThread(){
	for (int lrt = 1; lrt<archi_->getNPE(); lrt++){
		int size = sizeof(StopLrtMsg);
		StopLrtMsg* msg = (StopLrtMsg*)getSpiderCommunicator()->ctrl_start_send(lrt, size);

		msg->msgIx = MSG_STOP_LRT;

		getSpiderCommunicator()->ctrl_end_send(lrt, size);
	}

	// Wait for every LRT to end
	pthread_barrier_wait(&pthread_barrier_init_and_end_thread);


	// Wait for each thread to free its lrt and archi stacks and to reach its end
	for (int i = 1; i < nLrt_; i++) {
	    pthread_join(thread_ID_tab_[i], NULL);
	}


	lrt_[0]->~LRT();
	((PThreadLrtCommunicator*)lrtCom_[0])->~PThreadLrtCommunicator();

	// for (int i = 0; i < nLrt_; i++) lrt_[i]->~LRT();
	((PThreadSpiderCommunicator*)spiderCom_)->~PThreadSpiderCommunicator();
	// for (int i = 0; i < nLrt_; i++) ((PThreadLrtCommunicator*)lrtCom_[i])->~PThreadLrtCommunicator();

	archi_->~SharedMemArchi();


	StackMonitor::free(ARCHI_STACK, lrt_[0]);
	StackMonitor::free(ARCHI_STACK, lrtCom_[0]);
	StackMonitor::free(ARCHI_STACK, spiderCom_);
	StackMonitor::free(ARCHI_STACK, archi_);


	StackMonitor::freeAll(ARCHI_STACK);
	StackMonitor::freeAll(LRT_STACK);
	StackMonitor::freeAll(TRANSFO_STACK);
	StackMonitor::freeAll(SRDAG_STACK);
	StackMonitor::freeAll(PISDF_STACK);


	//WARNING : Thread specific stacks have to be cleaned BEFORE exiting threads
	StackMonitor::cleanAllStack();

	//Destroying synchronisation barrier
	pthread_barrier_destroy(&pthread_barrier_init_and_end_thread);

	//Destroying semaphores
	sem_destroy(&semTrace);

	for (int i = 0; i<nLrt_; i++){
		sem_destroy(&semFifoSpidertoLRT[i]);
		sem_destroy(&semFifoLRTtoSpider[i]);
	}

	//Desallocation des tableaux dynamiques
	free(thread_ID_tab_);

	free(semFifoSpidertoLRT);
	free(semFifoLRTtoSpider);

	free(lrt_);
	free(lrtCom_);

	free(stackArchi);
	free(stackLrt);

	for (int i = 0; i < nLrt_; i++){
		delete fifoSpidertoLRT[i];
		delete fifoLRTtoSpider[i];
	}

	free(fifoLRTtoSpider);
	free(fifoSpidertoLRT);

	free(jobTab);
}

/** File Handling */
FILE* PlatformPThread::fopen(const char* name){

	return std::fopen(name, "w+");
}

void PlatformPThread::fprintf(FILE* id, const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);

#ifdef _WIN32
	int n = _vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);
#else
	int n = vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);
#endif

	if (n >= PLATFORM_FPRINTF_BUFFERSIZE){
		printf("PLATFORM_FPRINTF_BUFFERSIZE too small\n");
	}

	for (int i = 0; i < n; i++) fputc(buffer[i], id);
}
void PlatformPThread::fclose(FILE* id){
	if (id != NULL){
		std::fclose(id);
		id = NULL;
	}
}

void* PlatformPThread::virt_to_phy(void* address){
	return (void*)((long)dataMem + (long)address);
}

int PlatformPThread::getCacheLineSize(){
	return 0;
}

int PlatformPThread::getMinAllocSize(){
#ifdef _WIN32
	//workaround because Windows
	return 4096;
#else
	return getpagesize();
#endif
}


void PlatformPThread::rstJobIx(){
	//Sending a msg to all slave LRTs, end of graph iteration
	for (int i = 1;i < nLrt_;i++){
		EndIterMsg* msg = (EndIterMsg*) getSpiderCommunicator()->ctrl_start_send(i, sizeof(EndIterMsg));
		msg->msgIx = MSG_END_ITER;
		getSpiderCommunicator()->ctrl_end_send(i, sizeof(EndIterMsg));
	}

	//Waiting for slave LRTs to finish their job queue
	for (int i = 1;i < nLrt_;i++){
		void* msg = NULL;

		do{
			while(getSpiderCommunicator()->ctrl_start_recv(i,&msg) == 0);
		}
		while (((UndefinedMsg*) msg)->msgIx != MSG_END_ITER);
		getSpiderCommunicator()->ctrl_end_recv(i);
	}

	//Sending a msg to all slave LRTs, reset jobIx counter
	for (int i = 1;i < nLrt_;i++){
		ResetLrtMsg* msg = (ResetLrtMsg*) getSpiderCommunicator()->ctrl_start_send(i, sizeof(ResetLrtMsg));
		msg->msgIx = MSG_RESET_LRT;
		getSpiderCommunicator()->ctrl_end_send(i, sizeof(ResetLrtMsg));
	}

	//reseting master LRT jobIx counter
	Platform::get()->getLrt()->setJobIx(0);

	//reseting jobTab
	memset(jobTab, 0, sizeof(unsigned int)*nLrt_);

	//Waiting for slave LRTs to reset their jobIx counter
	for (int i = 1;i < nLrt_;i++){
		void* msg = NULL;
		do{
			while(getSpiderCommunicator()->ctrl_start_recv(i,&msg) == 0);
		}
		while (((UndefinedMsg*) msg)->msgIx != MSG_RESET_LRT);
		getSpiderCommunicator()->ctrl_end_recv(i);
	}
}

/** Time Handling */
void PlatformPThread::rstTime(struct ClearTimeMsg* msg){
	struct timespec* ts = (struct timespec*)(msg + 1);
	start = *ts;
}

void PlatformPThread::rstTime(){
	start_steady = std::chrono::steady_clock::now();

	start.tv_sec = (start_steady - origin_steady).count() / 1000000000;
	start.tv_nsec = (start_steady - origin_steady).count() - (start_steady - origin_steady).count() / 1000000000;


	for (int lrt = 1; lrt < archi_->getNPE(); lrt++){
		int size = sizeof(ClearTimeMsg)+sizeof(struct timespec);
		ClearTimeMsg* msg = (ClearTimeMsg*)getSpiderCommunicator()->ctrl_start_send(lrt, size);
		struct timespec* ts = (struct timespec*)(msg + 1);

		msg->msgIx = MSG_CLEAR_TIME;
		*ts = start;

		getSpiderCommunicator()->ctrl_end_send(lrt, size);
	}
}

Time PlatformPThread::getTime(){
    std::chrono::time_point<std::chrono::steady_clock> ts_steady = std::chrono::steady_clock::now();
    long long val_steady = (ts_steady - start_steady).count();
    #ifdef _WIN32
        // Spider will think something went bad if returned time is 0, so in such case we're setting it to 1 because time in Windows is bad
            if (val_steady == 0) {
                val_steady++;
            }
    #endif // _WIN32




	return val_steady;
}

void PlatformPThread::idleLrt(int lrt){
	lrt_[lrt]->setIdle(true);
}

void PlatformPThread::wakeLrt(int lrt){
	lrt_[lrt]->setIdle(false);
}

void PlatformPThread::idle(){
	while (lrt_[0]->isIdle()){
#ifdef _WIN32
		Sleep((unsigned)-1);
#else
		sleep((unsigned)-1);
#endif
	}
}

Time PlatformPThread::mappingTime(int nActors, int nPe){
	return (unsigned long long) 1000 * nActors;
}


void PlatformPThread::lrtPThread(Arg_lrt *argument_lrt){

	int index = argument_lrt->index;

	//registering itself in thread_ID_tab_
	thread_ID_tab_[index] = pthread_self();

	//waiting for every threads to register itself in thread_ID_tab_
	pthread_barrier_wait(&pthread_barrier_init_and_end_thread);

	//Declaration des stacks spécific au thread
	argument_lrt->archiStack.size /= argument_lrt->nLrt;
	argument_lrt->archiStack.start = (char*)argument_lrt->archiStack.start + argument_lrt->archiStack.size * index * sizeof(char);
	StackMonitor::initStack(ARCHI_STACK, argument_lrt->archiStack);

	argument_lrt->lrtStack.size /= argument_lrt->nLrt;
	argument_lrt->lrtStack.start = (char*) argument_lrt->lrtStack.start + argument_lrt->lrtStack.size * index * sizeof(char);
	StackMonitor::initStack(LRT_STACK, argument_lrt->lrtStack);


	/** Create LRT */
	lrtCom_[index] = CREATE(ARCHI_STACK, PThreadLrtCommunicator)(
		MAX_MSG_SIZE,
		argument_lrt->fifoSpidertoLRT,
		argument_lrt->fifoLRTtoSpider,
		argument_lrt->fifoTrace,
		argument_lrt->semTrace,
		argument_lrt->semFifoSpidertoLRT,
		argument_lrt->semFifoLRTtoSpider,
		jobTab,
		dataMem);
	lrt_[index] = CREATE(ARCHI_STACK, LRT)(index);
	setAffinity(index);
	lrt_[index]->setFctTbl(argument_lrt->fcts, argument_lrt->nLrtFcts);

	// Enable papify if need to
    if (argument_lrt->usePapify) {
        lrt_[index]->setUsePapify();
        printf("monitoring !!!\n");
        std::map<lrtFct , PapifyConfig*>::iterator it;
        for (it = argument_lrt->jobPapifyActions.begin(); it != argument_lrt->jobPapifyActions.end(); ++it) {
            // TODO check that LRT_STACK is large enough
            papify_action_s* papifyAction =CREATE(LRT_STACK, papify_action_s);
            PapifyConfig* papifyConfig = it->second;
            configure_papify(papifyAction,
                    /* componentName */   papifyConfig->peType_,
                    /* PEName */          papifyConfig->peID_,
                    /* actorName */       papifyConfig->actorName_,
                    /* num_events */      papifyConfig->eventSize_,
                    /* all_events_name */ papifyConfig->monitoredEvents_,
                    /* eventSet_Id */     papifyConfig->eventSetID_);
            papifyAction->isTiming = papifyConfig->isTiming_;
            lrt_[index]->addPapifyJobInfo(it->first, papifyAction);
        }
    }

	// Wait for all LRTs to be created
	pthread_barrier_wait(&pthread_barrier_init_and_end_thread);

	// Start LRT */
	lrt_[index]->runInfinitly();


	//wait for every LRT to end
	pthread_barrier_wait(&pthread_barrier_init_and_end_thread);


	lrt_[index]->~LRT();
	((PThreadLrtCommunicator*)lrtCom_[index])->~PThreadLrtCommunicator();

	//freeing thread specific stacks
	StackMonitor::free(ARCHI_STACK, lrt_[index]);
	StackMonitor::free(ARCHI_STACK, lrtCom_[index]);

	//cleaning thread specific stacks
	StackMonitor::clean(ARCHI_STACK);
	StackMonitor::clean(LRT_STACK);

	pthread_exit(EXIT_SUCCESS);
}

void* lrtPThread_helper(void *voidArgs) {
	Arg_lrt *args = (Arg_lrt*)voidArgs;
	args->instance->lrtPThread(args);
	return 0;
}



void printfSpider(){

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


