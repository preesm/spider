/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include <platformLinux.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <sched.h>

#include <tools/Stack.h>

#include <lrt.h>
#include <spider.h>
#include <LinuxLrtCommunicator.h>
#include <LinuxSpiderCommunicator.h>

#define PLATFORM_FPRINTF_BUFFERSIZE 200
#define SHARED_MEM_KEY		8452

#define MAX_MSG_SIZE 10*1024
#define NFIFOS 32*1024

static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];
static struct timespec start;
static void* shMem;
static void* dataMem;

static void initShMem(int shMemSize){
	/** Open Shared Memory */
    int shmid;
    key_t key = SHARED_MEM_KEY;

	printf("Creating shared memory...\n");

    /** Get the segment. */
    if ((shmid = shmget(key, shMemSize, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /** Now we attach the segment to our data space. */
    if ((shMem = (void*)shmat(shmid, NULL, 0)) == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    dataMem = (void*)((long)shMem + sizeof(unsigned int)*NFIFOS);

    memset(shMem, 0, shMemSize);
}

static void setAffinity(int cpuId){
    cpu_set_t mask;
    int status;

    CPU_ZERO(&mask);
    CPU_SET(cpuId, &mask);
    status = sched_setaffinity(0, sizeof(mask), &mask);
    if (status != 0)
    {
        perror("sched_setaffinity");
    }
}

PlatformLinux::PlatformLinux(int nLrt, int shMemSize, Stack *stack, lrtFct* fcts, int nLrtFcts){
	int pipeSpidertoLRT[2*nLrt];
	int pipeLRTtoSpider[2*nLrt];
	int pipeTrace[2];
	int cpIds[nLrt];
	sem_t* semFifo;
	sem_t* semTrace;

	if(platform_)
		throw "Try to create 2 platforms";

	platform_ = this;
	stack_ = stack;

	cpIds[0] = getpid();

	semFifo = sem_open("spider_fifo", O_CREAT, ACCESSPERMS, 1);
	semTrace = sem_open("spider_trace", O_CREAT, ACCESSPERMS, 1);

	if (pipe2(pipeTrace, O_NONBLOCK) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	printf("Pipe Trace: %d <= %d\n", pipeTrace[0], pipeTrace[1]);

	for(int i=0; i<nLrt; i++){
		/** Open Pipes */
		if (pipe2(pipeSpidertoLRT+2*i, O_NONBLOCK) == -1
				|| pipe2(pipeLRTtoSpider+2*i, O_NONBLOCK) == -1) {
			perror("pipe");
			exit(EXIT_FAILURE);
		}
		fcntl(pipeSpidertoLRT[2*i  ], F_SETPIPE_SZ, 1024*1024);
		fcntl(pipeSpidertoLRT[2*i+1], F_SETPIPE_SZ, 1024*1024);
		fcntl(pipeLRTtoSpider[2*i  ], F_SETPIPE_SZ, 1024*1024);
		fcntl(pipeLRTtoSpider[2*i+1], F_SETPIPE_SZ, 1024*1024);
		printf("Pipe Spider=>LRT %d: %d <= %d\n", i, pipeSpidertoLRT[2*i], pipeSpidertoLRT[2*i+1]);
		printf("Pipe LRT=>Spider %d: %d <= %d\n", i, pipeLRTtoSpider[2*i], pipeLRTtoSpider[2*i+1]);
	}

	for(int i=1; i<nLrt; i++){
        pid_t cpid = fork();
        if (cpid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid == 0) { /* Child */
        	/** Close unused pipe */

        	/** Initialize shared memory */
        	initShMem(shMemSize);

        	/** Create LRT */
        	lrtCom_ = (LrtCommunicator*) CREATE(stack, LinuxLrtCommunicator)(
        			MAX_MSG_SIZE,
					pipeSpidertoLRT[2*i],
					pipeLRTtoSpider[2*i+1],
					pipeTrace[1],
					semFifo,
					semTrace,
					shMem,
					dataMem,
					NFIFOS,
					stack);
        	lrt_ = CREATE(stack, LRT)(i);
        	setAffinity(i);
        	lrt_->setFctTbl(fcts, nLrtFcts);

        	/** launch LRT */
        	lrt_->runInfinitly();
        } else { /* Parent */
        	cpIds[i] = cpid;
        }
	}

	/** Close unused pipe */

	/** Initialize shared memory */
	initShMem(shMemSize);
    memset(shMem,0,shMemSize);

	/** Initialize LRT and Communicators */
    spiderCom_ = CREATE(stack, LinuxSpiderCommunicator)(
    		MAX_MSG_SIZE,
			nLrt,
			semTrace,
			pipeTrace[1],
			pipeTrace[0],
			stack);

    for(int i=0; i<nLrt; i++)
    	((LinuxSpiderCommunicator*)spiderCom_)->setLrtCom(i, pipeLRTtoSpider[2*i], pipeSpidertoLRT[2*i+1]);

	lrtCom_ = CREATE(stack, LinuxLrtCommunicator)(
			MAX_MSG_SIZE,
			pipeSpidertoLRT[0],
			pipeLRTtoSpider[1],
			pipeTrace[1],
			semFifo,
			semTrace,
			shMem,
			dataMem,
			NFIFOS,
			stack);
	lrt_ = CREATE(stack, LRT)(0);
	setAffinity(0);
	lrt_->setFctTbl(fcts, nLrtFcts);


	/** Create Archi */
	archi_ = CREATE(stack, SharedMemArchi)(
				/* Stack */  	stack,
				/* Nb PE */		nLrt,
				/* Nb PE Type*/ 1);

	archi_->setPETypeRecvSpeed(0, 1, 10);
	archi_->setPETypeSendSpeed(0, 1, 10);
	archi_->setPEType(0, 0);

	char name[40];
	sprintf(name, "PID %d (Spider)", cpIds[0]);
	archi_->setName(0, name);
	for(int i=1; i<nLrt; i++){
		sprintf(name, "PID %d (LRT %d)", cpIds[i], i);
		archi_->setPEType(i, 0);
		archi_->setName(i, name);
	}

	this->rstTime();
}

PlatformLinux::~PlatformLinux(){
	for(int lrt=1; lrt<archi_->getNPE(); lrt++){
		int size = sizeof(StopLrtMsg);
		StopLrtMsg* msg = (StopLrtMsg*) getSpiderCommunicator()->ctrl_start_send(lrt, size);

		msg->msgIx = MSG_STOP_LRT;

		getSpiderCommunicator()->ctrl_end_send(lrt, size);
	}

	wait(0);

	lrt_->~LRT();
	((LinuxSpiderCommunicator*)spiderCom_)->~LinuxSpiderCommunicator();
	((LinuxLrtCommunicator*)lrtCom_)->~LinuxLrtCommunicator();
	archi_->~SharedMemArchi();

	stack_->free(lrt_);
	stack_->free(spiderCom_);
	stack_->free(lrtCom_);
	stack_->free(archi_);
}

/** File Handling */
int PlatformLinux::fopen(const char* name){
	return open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP| S_IROTH | S_IWOTH);
}

void PlatformLinux::fprintf(int id, const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);
	if(n >= PLATFORM_FPRINTF_BUFFERSIZE){
		printf("PLATFORM_FPRINTF_BUFFERSIZE too small\n");
	}
	write(id, buffer, n);
}
void PlatformLinux::fclose(int id){
	close(id);
}

void* PlatformLinux::virt_to_phy(void* address){
	return (void*)((long)dataMem + (long)address);
}

int PlatformLinux::getCacheLineSize(){
	return 0;
}

int PlatformLinux::getMinAllocSize(){
	return getpagesize();
}

/** Time Handling */
void PlatformLinux::rstTime(ClearTimeMsg* msg){
	struct timespec* ts = (struct timespec*)(msg+1);
	start = *ts;
}

void PlatformLinux::rstTime(){
	clock_gettime(CLOCK_MONOTONIC, &start);

	for(int lrt=1; lrt<archi_->getNPE(); lrt++){
		int size = sizeof(ClearTimeMsg)+sizeof(struct timespec);
		ClearTimeMsg* msg = (ClearTimeMsg*) getSpiderCommunicator()->ctrl_start_send(lrt, size);
		struct timespec* ts = (struct timespec*)(msg+1);

		msg->msgIx = MSG_CLEAR_TIME;
		*ts = start;

		getSpiderCommunicator()->ctrl_end_send(lrt, size);
	}
}

Time PlatformLinux::getTime(){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	long long val = ts.tv_sec - start.tv_sec;
	val *= 1000000000;
	val += ts.tv_nsec - start.tv_nsec;
	return val;
}

SharedMemArchi* PlatformLinux::getArchi(){
	return archi_;
}
