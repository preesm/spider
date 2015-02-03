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
#include <ti/csl/csl_chipAux.h>

#include <tools/Stack.h>

#include <lrt.h>
#include <qmss.h>
#include <K2DspLrtCommunicator.h>

#define PLATFORM_FPRINTF_BUFFERSIZE 200
#define SHARED_MEM_KEY		8452

#define MAX_MSG_SIZE 10*1024

//static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];

static CSL_TmrRegsOvly regs;
static void* shMem;

static inline void initTime();

PlatformK2Dsp::PlatformK2Dsp(int shMemSize, Stack *stack, lrtFct* fcts, int nLrtFcts){
	int data_mem_start;
	int data_mem_size;

	if(platform_)
		throw "Try to create 2 platforms";

	platform_ = this;
	stack_ = stack;

	/** Initialize shared memory & QMSS*/
	lrt_qmss_init(&data_mem_start, &data_mem_size);

	shMem = (void*)data_mem_start;
	if(data_mem_size < shMemSize)
		throw "Request too many shared memory";

	initTime();

	/** Create LRT */
	lrtCom_ = CREATE(stack, K2DspLrtCommunicator)();
	lrt_ = CREATE(stack, LRT)(4+CSL_chipReadDNUM());
	lrt_->setFctTbl(fcts, nLrtFcts);

	/** launch LRT */
	lrt_->runInfinitly();
}

PlatformK2Dsp::~PlatformK2Dsp(){

}

int PlatformK2Dsp::getMinAllocSize(){
	return 128;
}

int PlatformK2Dsp::getCacheLineSize(){
	return 128;
}

/** File Handling */
int PlatformK2Dsp::fopen(const char* name){
//	return open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP| S_IROTH | S_IWOTH);
	return -1;
}

void PlatformK2Dsp::fprintf(int id, const char* fmt, ...){
//	va_list ap;
//	va_start(ap, fmt);
//	int n = vsnprintf(buffer, PLATFORM_FPRINTF_BUFFERSIZE, fmt, ap);
//	if(n >= PLATFORM_FPRINTF_BUFFERSIZE){
//		printf("PLATFORM_FPRINTF_BUFFERSIZE too small\n");
//	}
//	write(id, buffer, n);
}
void PlatformK2Dsp::fclose(int id){
//	close(id);
}

void* PlatformK2Dsp::virt_to_phy(void* address){
	return (void*)((int)shMem + (int)address);
}

/** Time Handling */
static inline void initTime(){
	/* Init base address */
	regs = (CSL_TmrRegsOvly)CSL_TIMER_0_REGS;
}

void PlatformK2Dsp::rstTime(ClearTimeMsg* msg){
	/* Nothing to do, time reset do not send messages */
}

void PlatformK2Dsp::rstTime(){
	/* Should not be done by LRT */
}

Time PlatformK2Dsp::getTime(){
	CSL_Uint64 val;
	val = regs->CNTHI;
	val = (val<<32) + regs->CNTLO;
	return val*5; /* 200MHz to 1GHz */
}

