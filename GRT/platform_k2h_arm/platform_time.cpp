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

#include <platform_time.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#include <ti/csl/device/k2h/src/cslr_device.h>
#include <ti/csl/cslr_tmr.h>
#include <ti/csl/csl_types.h>

static CSL_TmrRegsOvly regs;
static int dev_mem_fd;

void platform_time_init(){
	/* Open mem device */
	if((dev_mem_fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1){
		printf("Failed to open \"dev/mem\" err=%s\n", strerror(errno));
		abort();
	}

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

void platform_time_reset(){
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

UINT32 platform_time_getValue(){
	CSL_Uint64 val;
	val = regs->CNTHI;
	val = (val<<32) + regs->CNTLO;
	return val;
}
