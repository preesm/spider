/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2015 - 2016) :
 *
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2015 - 2016)
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
#include "init.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#include <ti/csl/cslr_device.h>
#include <ti/csl/cslr_tmr.h>

#include <ti/csl/csl_psc.h>
#include <ti/csl/csl_pscAux.h>

static int dev_mem_fd;

int qmss_cfg_regs;
int msmc_mem_base;
int ddr_mem_base;
CSL_FftcRegs* fftc_a_cfg_regs;
CSL_FftcRegs* fftc_b_cfg_regs;
void* cppi_regs;
void* qm_regs;
CSL_TmrRegsOvly tmr_regs;

CSL_PscRegs* psc_regs;

void* Osal_qmssVirtToPhy (void *ptr){
	if((uint32_t)ptr >= (uint32_t)qmss_cfg_regs
			&& (uint32_t)ptr < (uint32_t)qmss_cfg_regs + QMSS_CFG_SIZE)
		return (void*)(((int)ptr - qmss_cfg_regs) + CSL_QMSS_CFG_BASE);
	else if((uint32_t)ptr >= (uint32_t)msmc_mem_base
			&& (uint32_t)ptr < (uint32_t)msmc_mem_base + MSMC_SIZE)
		return (void*)(((int)ptr - msmc_mem_base) + CSL_MSMC_SRAM_REGS);
	else
		printf("Bad input ptr in Osal_qmssVirtToPhy\n");
	return 0;
}
void* Osal_qmssPhyToVirt (void *ptr){
	if((uint32_t)ptr >= (uint32_t)CSL_QMSS_CFG_BASE
			&& (uint32_t)ptr < (uint32_t)CSL_QMSS_CFG_BASE + QMSS_CFG_SIZE)
		return (void*)(((int)ptr - CSL_QMSS_CFG_BASE) + qmss_cfg_regs);
	else if((uint32_t)ptr >= (uint32_t)CSL_MSMC_SRAM_REGS
			&& (uint32_t)ptr < (uint32_t)CSL_MSMC_SRAM_REGS + MSMC_SIZE)
		return (void*)(((int)ptr - CSL_MSMC_SRAM_REGS) + msmc_mem_base);
	else
		printf("Bad input ptr in Osal_qmssVirtToPhy\n");
	return 0;
}

void* Osal_qmssConvertDescVirtToPhy(uint32_t QID, void *descAddr){
	if(descAddr == 0)
		return 0;

	if((uint32_t)descAddr > (uint32_t)msmc_mem_base + MSMC_SIZE
			|| (uint32_t)descAddr < (uint32_t)msmc_mem_base)
		printf("Bad input ptr in Osal_qmssConvertDescVirtToPhy\n");

	return (void*)(((int)descAddr - msmc_mem_base) + CSL_MSMC_SRAM_REGS);
}
void* Osal_qmssConvertDescPhyToVirt(uint32_t QID, void *descAddr){
	if(descAddr == 0)
		return 0;

	if((uint32_t)descAddr > (uint32_t)CSL_MSMC_SRAM_REGS + MSMC_SIZE
			|| (uint32_t)descAddr < (uint32_t)CSL_MSMC_SRAM_REGS)
		printf("Bad input ptr in Osal_qmssConvertDescPhyToVirt\n");

	return (void*)(((int)descAddr - CSL_MSMC_SRAM_REGS) + msmc_mem_base);
}

/**
 * Initialize all hardware subsytems:
 * 	- Initialize memory addresses
 * 	- Enable Power Domains
 * 	- Configure Timers
 */
void init_hw(){
	/* Open mem device */
	if((dev_mem_fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1){
		printf("Failed to open \"dev/mem\" err=%s\n", strerror(errno));
		abort();
	}

	/* Translate Memory using mmap */
	/* QMSS CFG Regs */
	qmss_cfg_regs = (int)mmap(
			0, QMSS_CFG_SIZE,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)CSL_QMSS_CFG_BASE);

	if (!qmss_cfg_regs){
		printf("ERROR: Failed to map QMSS CFG registers\n");
		abort();
	}

	/* TMR Regs */
	tmr_regs = mmap(
			0, TMR_REGS_SIZE,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)CSL_TIMER_0_REGS);

	if(!tmr_regs){
		printf("ERROR: Failed to map Timer registers\n");
		abort();
	}

	/* MSMC memory */
	msmc_mem_base = (int)mmap(
			0, MSMC_SIZE,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)CSL_MSMC_SRAM_REGS);

	if(!msmc_mem_base){
		printf("ERROR: Failed to map MSMC memory\n");
		abort();
	}

	/* DDR memory */
	ddr_mem_base = (int)mmap(
			0, DDR_SIZE,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)DDR_BASE);

	if(!ddr_mem_base){
		printf("ERROR: Failed to map MSMC memory\n");
		abort();
	}

	/* FFTC A regs */
	fftc_a_cfg_regs = mmap(
			0, 0x8000,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)CSL_FFTC_0_CFG_REGS);

	if(!fftc_a_cfg_regs){
		printf("ERROR: Failed to map FFTC Regs\n");
		abort();
	}

	/* FFTC B regs */
	fftc_b_cfg_regs = mmap(
			0, 0x8000,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)CSL_FFTC_1_CFG_REGS);

	if(!fftc_b_cfg_regs){
		printf("ERROR: Failed to map FFTC Regs\n");
		abort();
	}

	/* QM regs */
	qm_regs = mmap(
			0, 0x00100000,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)0x23a00000);

	if(!qm_regs){
		printf("ERROR: Failed to map FFTC Regs\n");
		abort();
	}

	/* CPPI Regs */
	cppi_regs = mmap(
			0, CPPI_SIZE_REG,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)CPPI_BASE_REG);

	if(!cppi_regs){
		printf("ERROR: Failed to map CPPI Regs\n");
		abort();
	}

	/* Map PSC Regs */
	psc_regs = mmap(
			0, 0x10000,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			dev_mem_fd,
			(off_t)CSL_PSC_REGS);

	if(!psc_regs){
		printf("ERROR: Failed to map PSC Regs\n");
		abort();
	}

	/* Initialize power domains */
	/* FFTC power domain is turned OFF by default.
	 * It needs to be turned on before doing any
	 * FFTC device register access.
	 */
	/* Set FFTC Power domain to ON */
    CSL_FINST (psc_regs->PDCTL[CSL_PSC_PD_FFTC_01], PSC_PDCTL_NEXT, ON);
    CSL_FINST (psc_regs->PDCTL[CSL_PSC_PD_FFTC_2345], PSC_PDCTL_NEXT, ON);
    msync(psc_regs, sizeof(CSL_PscRegs), MS_SYNC);

	/* Enable the clocks too for FFTC */
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_0], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_1], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_2], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_3], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_4], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_5], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    msync(psc_regs, sizeof(CSL_PscRegs), MS_SYNC);

	/* Start the state transition */
    psc_regs->PTCMD =   (1 << CSL_PSC_PD_FFTC_01);
    psc_regs->PTCMD =   (1 << CSL_PSC_PD_FFTC_2345);
    msync(psc_regs, sizeof(CSL_PscRegs), MS_SYNC);

	/* Wait until the state transition process is completed. */
    do msync(psc_regs, sizeof(CSL_PscRegs), MS_INVALIDATE);
	while (CSL_FEXTR (psc_regs->PTSTAT, CSL_PSC_PD_FFTC_01, CSL_PSC_PD_FFTC_01));

    do msync(psc_regs, sizeof(CSL_PscRegs), MS_INVALIDATE);
	while (CSL_FEXTR (psc_regs->PTSTAT, CSL_PSC_PD_FFTC_2345, CSL_PSC_PD_FFTC_2345));

	/* Check FFTC PSC status */
    msync(psc_regs, sizeof(CSL_PscRegs), MS_INVALIDATE);
	if ((CSL_FEXT(psc_regs->PDSTAT[CSL_PSC_PD_FFTC_01],   PSC_PDSTAT_STATE) != PSC_PDSTATE_ON) ||
		(CSL_FEXT(psc_regs->PDSTAT[CSL_PSC_PD_FFTC_2345], PSC_PDSTAT_STATE) != PSC_PDSTATE_ON) ||
		(CSL_FEXT(psc_regs->MDSTAT[CSL_PSC_LPSC_FFTC_0], PSC_MDSTAT_STATE)  != PSC_MODSTATE_ENABLE) ||
		(CSL_FEXT(psc_regs->MDSTAT[CSL_PSC_LPSC_FFTC_1], PSC_MDSTAT_STATE)  != PSC_MODSTATE_ENABLE) ||
		(CSL_FEXT(psc_regs->MDSTAT[CSL_PSC_LPSC_FFTC_2], PSC_MDSTAT_STATE)  != PSC_MODSTATE_ENABLE) ||
		(CSL_FEXT(psc_regs->MDSTAT[CSL_PSC_LPSC_FFTC_3], PSC_MDSTAT_STATE)  != PSC_MODSTATE_ENABLE) ||
		(CSL_FEXT(psc_regs->MDSTAT[CSL_PSC_LPSC_FFTC_4], PSC_MDSTAT_STATE)  != PSC_MODSTATE_ENABLE) ||
		(CSL_FEXT(psc_regs->MDSTAT[CSL_PSC_LPSC_FFTC_5], PSC_MDSTAT_STATE)  != PSC_MODSTATE_ENABLE)){
		/* FFTC Power on failed */
		abort();
	}

	/* Reset FFTC */
	CSL_FINS (fftc_a_cfg_regs->CONTROL, FFTC_CONTROL_RESTART_BIT, 1);
	CSL_FINS (fftc_b_cfg_regs->CONTROL, FFTC_CONTROL_RESTART_BIT, 1);

	/* Initialize Shared Timer0 */
	/* TGCR */
    msync((void*)tmr_regs, sizeof(CSL_TmrRegs), MS_INVALIDATE);
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMMODE, 0); 	// 64-bit mode

	/* TCR */
	CSL_FINS(tmr_regs->TCR, TMR_TCR_CLKSRC_LO, 0); 	// Select Internal clock
	CSL_FINS(tmr_regs->TCR, TMR_TCR_TIEN_LO, 0); 		// Not gated clock
	CSL_FINS(tmr_regs->TCR, TMR_TCR_ENAMODE_LO, 2); 	// Timer enable continuously
	CSL_FINS(tmr_regs->TCR, TMR_TCR_ENAMODE_HI, 2); 	// Timer enable continuously
    msync((void*)tmr_regs, sizeof(CSL_TmrRegs), MS_SYNC);

	/* Reset Timer */
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMHIRS, 0); 	// Reset
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMLORS, 0); 	// Reset
	tmr_regs->PRDLO = (Uint32)-1;
	tmr_regs->PRDHI = (Uint32)-1;
	tmr_regs->CNTLO = 0;
	tmr_regs->CNTHI = 0;
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMHIRS, 1); 	// Release reset
	CSL_FINS(tmr_regs->TGCR, TMR_TGCR_TIMLORS, 1); 	// Release reset
    msync((void*)tmr_regs, sizeof(CSL_TmrRegs), MS_SYNC);
}

void clean_hw(){
}
