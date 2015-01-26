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

#include <platformK2Dsp.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <ti/csl/device/k2h/src/cslr_device.h>
#include <ti/csl/cslr_tmr.h>
#include <ti/csl/csl_types.h>

#include <tools/Stack.h>

#include <lrt.h>
#include <qmss.h>
#include <K2ArmLrtCommunicator.h>
#include <K2ArmSpiderCommunicator.h>

#define PLATFORM_FPRINTF_BUFFERSIZE 200
#define SHARED_MEM_KEY		8452

#define MAX_MSG_SIZE 10*1024

static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];
static struct timespec start;

static CSL_TmrRegsOvly regs;
static int dev_mem_fd;
static void* shMem;

static inline void initTime();

PlatformK2Dsp::PlatformK2Dsp(int nArm, int nDsp, int shMemSize, Stack *stack, lrtFct* fcts, int nLrtFcts){
	/* Open mem device */
	if((dev_mem_fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1){
		printf("Failed to open \"dev/mem\" err=%s\n", strerror(errno));
		abort();
	}

	long data_mem_start;
	long data_mem_size;
	int cpIds[nArm];

	stack_ = stack;
	cpIds[0] = getpid();



	/** Initialize shared memory & QMSS*/
	spider_qmss_init(dev_mem_fd, &data_mem_start, &data_mem_size);

	shMem = (void*)data_mem_start;
	if(data_mem_size < shMemSize)
		throw "Request too many shared memory";

	initTime();


	for(int i=1; i<nArm; i++){
        pid_t cpid = fork();
        if (cpid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid == 0) { /* Child */
        	/** Create LRT */
        	K2ArmLrtCommunicator* lrtCom = CREATE(stack, K2ArmLrtCommunicator)();
        	LRT* lrt = CREATE(stack, LRT)(i, lrtCom);
        	setAffinity(i);
        	lrt->setFctTbl(fcts, nLrtFcts);
        	setLrt(lrt);

        	Platform::set(this);

        	/** launch LRT */
			lrt->runInfinitly();
        } else { /* Parent */
        	cpIds[i] = cpid;
        }
	}

	/** Initialize LRT and Communicators */
    K2ArmSpiderCommunicator* spiderCom = CREATE(stack, K2ArmSpiderCommunicator)();

	K2ArmLrtCommunicator* lrtCom = CREATE(stack, K2ArmLrtCommunicator)();
	LRT* lrt = CREATE(stack, LRT)(0, lrtCom);
	setAffinity(0);
	lrt->setFctTbl(fcts, nLrtFcts);

	setLrt(lrt);
	setSpiderCommunicator(spiderCom);

	/** Create Archi */
	archi_ = CREATE(stack, SharedMemArchi)(
				/* Stack */  	stack,
				/* Nb PE */		nArm+nDsp,
				/* Nb PE Type*/ 2);

	archi_->setPETypeRecvSpeed(0, 1, 10);
	archi_->setPETypeSendSpeed(0, 1, 10);
	archi_->setPEType(0, 0);

	char name[40];
	sprintf(name, "PID %d (Spider)", cpIds[0]);
	archi_->setName(0, name);

	for(int i=1; i<nArm; i++){
		sprintf(name, "PID %d (LRT %d)", cpIds[i], i);
		archi_->setPEType(i, 0);
		archi_->setName(i, name);
	}

	Platform::set(this);
	this->rstTime();
}

PlatformK2Dsp::~PlatformK2Dsp(){
	for(int lrt=1; lrt<archi_->getNPE(); lrt++){
		int size = sizeof(StopLrtMsg);
		StopLrtMsg* msg = (StopLrtMsg*) getSpiderCommunicator()->ctrl_start_send(lrt, size);

		msg->msgIx = MSG_STOP_LRT;

		getSpiderCommunicator()->ctrl_end_send(lrt, size);
	}

	wait(0);

	LRT* lrt = getLrt();
	K2ArmSpiderCommunicator* spiderCom = (K2ArmSpiderCommunicator*)getSpiderCommunicator();
	K2ArmLrtCommunicator* lrtCom = (K2ArmLrtCommunicator*)lrt->getCom();

	lrt->~LRT();
	spiderCom->~K2ArmSpiderCommunicator();
	lrtCom->~K2ArmLrtCommunicator();
	archi_->~SharedMemArchi();

	stack_->free(lrt);
	stack_->free(spiderCom);
	stack_->free(lrtCom);
	stack_->free(archi_);

	close(dev_mem_fd);
}

/** File Handling */
int PlatformK2Dsp::fopen(const char* name){
	return open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP| S_IROTH | S_IWOTH);
}

void PlatformK2Dsp::fprintf(int id, const char* fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);
	if(n >= PLATFORM_FPRINTF_BUFFERSIZE){
		printf("PLATFORM_FPRINTF_BUFFERSIZE too small\n");
	}
	write(id, buffer, n);
}
void PlatformK2Dsp::fclose(int id){
	close(id);
}

void* PlatformK2Dsp::virt_to_phy(void* address){
	return (void*)((long)shMem + (long)address);
}

/** Time Handling */
static inline void initTime(){
	/* Init base address */
	regs = (CSL_TmrRegsOvly)mmap(0, 0x10000, (PROT_READ|PROT_WRITE), MAP_SHARED, dev_mem_fd, (off_t)CSL_TIMER_0_REGS);
	if (!regs){
		printf("ERROR: Failed to map TMR registers\n");
		abort();
	}

	/* Init Timer */
	/* TGCR */
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMMODE, 0); 	// 64-bit mode

	/* TCR */
	CSL_FINS(regs->TCR, TMR_TCR_CLKSRC_LO, 0); 	// Select Internal clock
	CSL_FINS(regs->TCR, TMR_TCR_TIEN_LO, 0); 		// Not gated clock
	CSL_FINS(regs->TCR, TMR_TCR_ENAMODE_LO, 2); 	// Timer enable continuously
	CSL_FINS(regs->TCR, TMR_TCR_ENAMODE_HI, 2); 	// Timer enable continuously

	/* Reset Timer */
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMHIRS, 0); 	// Reset
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMLORS, 0); 	// Reset
	regs->PRDLO = -1;
	regs->PRDHI = -1;
	regs->CNTLO = 0;
	regs->CNTHI = 0;
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMHIRS, 1); 	// Release reset
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMLORS, 1); 	// Release reset
}

void PlatformK2Dsp::rstTime(ClearTimeMsg* msg){
	/* Nothing to do, time reset do not send messages */
}

void PlatformK2Dsp::rstTime(){
	/* Reset Timer */
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMHIRS, 0); 	// Reset
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMLORS, 0); 	// Reset
	regs->PRDLO = -1;
	regs->PRDHI = -1;
	regs->CNTLO = 0;
	regs->CNTHI = 0;
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMHIRS, 1); 	// Release reset
	CSL_FINS(regs->TGCR, TMR_TGCR_TIMLORS, 1); 	// Release reset
}

Time PlatformK2Dsp::getTime(){
	CSL_Uint64 val;
	val = regs->CNTHI;
	val = (val<<32) + regs->CNTLO;
	return val;
}

SharedMemArchi* PlatformK2Dsp::getArchi(){
	return archi_;
}
