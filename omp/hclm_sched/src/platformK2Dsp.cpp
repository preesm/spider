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

#include "platformK2Dsp.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <errno.h>

extern "C"{
#include <ti/csl/cslr_tmr.h>
#include <ti/csl/csl_types.h>
#include <ti/csl/csl_chipAux.h>
#include <ti/csl/cslr_device.h>
}

CSL_TmrRegsOvly tmr_regs;

#define PLATFORM_FPRINTF_BUFFERSIZE 200
#define SHARED_MEM_KEY		8452

#define MAX_MSG_SIZE 10*1024

//static char buffer[PLATFORM_FPRINTF_BUFFERSIZE];

static CSL_TmrRegsOvly regs;

static inline void initTime();

PlatformK2Dsp::PlatformK2Dsp(){
	if(platform_)
		throw "Try to create 2 platforms";

	platform_ = this;

	/* TMR Regs */
	tmr_regs = (CSL_TmrRegsOvly)CSL_TIMER_0_REGS;

	/* Initialize Shared Timer0 */
	/* TGCR */
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMMODE, 0); 	// 64-bit mode

	/* TCR */
	CSL_FINS(tmr_regs->TCR, TMR_TCR_CLKSRC_LO, 0); 	// Select Internal clock
	CSL_FINS(tmr_regs->TCR, TMR_TCR_TIEN_LO, 0); 		// Not gated clock
	CSL_FINS(tmr_regs->TCR, TMR_TCR_ENAMODE_LO, 2); 	// Timer enable continuously
	CSL_FINS(tmr_regs->TCR, TMR_TCR_ENAMODE_HI, 2); 	// Timer enable continuously

	/* Reset Timer */
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMHIRS, 0); 	// Reset
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMLORS, 0); 	// Reset
	tmr_regs->PRDLO = (Uint32)-1;
	tmr_regs->PRDHI = (Uint32)-1;
	tmr_regs->CNTLO = 0;
	tmr_regs->CNTHI = 0;
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMHIRS, 1); 	// Release reset
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMLORS, 1); 	// Release reset

	initTime();
}

PlatformK2Dsp::~PlatformK2Dsp(){
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

/** Time Handling */
static inline void initTime(){
	/* Init base address */
	regs = (CSL_TmrRegsOvly)CSL_TIMER_0_REGS;
}

static clock_t base;

void PlatformK2Dsp::rstTime(){
	/* Nothing to do, time reset do not send messages */
	CSL_Uint64 val;
	val = regs->CNTHI;
	val = (val<<32) + regs->CNTLO;
	timeBase_ = val;

    /* Initialize timer for clock */
    TSCL= 0,TSCH=0;
    base = _itoll(TSCH, TSCL);
}

Time PlatformK2Dsp::getTime(){
	clock_t t = (_itoll(TSCH, TSCL)-base);
	return (t+timeBase_*6)/1.2; /* 200MHz to 1GHz */
}

