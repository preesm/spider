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

#include "init.h"
#include "cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ti/csl/cslr_device.h>
#include <ti/csl/cslr_tmr.h>

#include <ti/csl/csl_psc.h>
#include <ti/csl/csl_pscAux.h>

int qmss_cfg_regs;
int msmc_mem_base;
int ddr_mem_base;
CSL_FftcRegs* fftc_cfg_regs;
void* cppi_regs;
void* qm_regs;
CSL_TmrRegsOvly tmr_regs;

CSL_PscRegs* psc_regs;

#ifdef DEVICE_K2H
#define MSMC_SIZE 		0x00600000
#define QMSS_CFG_SIZE 	0x00200000
#define QMSS_DATA_SIZE 	0x00100000
#define TMR_REGS_SIZE 	0x00010000

#define CPPI_BASE_REG	0x01F00000
#define CPPI_SIZE_REG	0x01100000

#else
#error Please define sizes in platformK2Arm/init_hw.c
#endif

void* Osal_qmssVirtToPhy (void *ptr){
	return ptr;
}
void* Osal_qmssPhyToVirt (void *ptr){
	return ptr;
}

void* Osal_qmssConvertDescVirtToPhy(uint32_t QID, void *descAddr){
	return descAddr;
}
void* Osal_qmssConvertDescPhyToVirt(uint32_t QID, void *descAddr){
	return descAddr;
}

/**
 * Initialize all hardware subsytems:
 * 	- Initialize memory addresses
 * 	- Enable Power Domains
 * 	- Configure Timers
 */
void init_hw(){
	/* Init Cache */
	CACHE_setL1DSize(CACHE_L1_32KCACHE);
	CACHE_setL1PSize(CACHE_L1_32KCACHE);
	CACHE_setL2Size(CACHE_0KCACHE);

	/* Translate Memory using mmap */
	/* QMSS CFG Regs */
	qmss_cfg_regs = CSL_QMSS_CFG_BASE;
	/* TMR Regs */
	tmr_regs = (void*)CSL_TIMER_0_REGS;
	/* MSMC memory */
	msmc_mem_base = CSL_MSMC_SRAM_REGS;
	/* DDR memory */
	ddr_mem_base = CSL_DDR3_0_DATA;
	/* FFTC regs */
	fftc_cfg_regs = (void*)CSL_FFTC_0_CFG_REGS;
	/* QM regs */
	qm_regs = (void*)0x23a00000;
	/* CPPI Regs */
	cppi_regs = (void*)CPPI_BASE_REG;
	/* Map PSC Regs */
	psc_regs = (void*)CSL_PSC_REGS;

	/* Initialize power domains */
	/* FFTC power domain is turned OFF by default.
	 * It needs to be turned on before doing any
	 * FFTC device register access.
	 */
	/* Set FFTC Power domain to ON */
    CSL_FINST (psc_regs->PDCTL[CSL_PSC_PD_FFTC_01], PSC_PDCTL_NEXT, ON);
    CSL_FINST (psc_regs->PDCTL[CSL_PSC_PD_FFTC_2345], PSC_PDCTL_NEXT, ON);

	/* Enable the clocks too for FFTC */
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_0], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_1], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_2], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_3], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_4], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);
    CSL_FINS (psc_regs->MDCTL[CSL_PSC_LPSC_FFTC_5], PSC_MDCTL_NEXT, PSC_MODSTATE_ENABLE);

	/* Start the state transition */
    psc_regs->PTCMD =   (1 << CSL_PSC_PD_FFTC_01);
    psc_regs->PTCMD =   (1 << CSL_PSC_PD_FFTC_2345);

	/* Wait until the state transition process is completed. */
	while (CSL_FEXTR (psc_regs->PTSTAT, CSL_PSC_PD_FFTC_01, CSL_PSC_PD_FFTC_01));
	while (CSL_FEXTR (psc_regs->PTSTAT, CSL_PSC_PD_FFTC_2345, CSL_PSC_PD_FFTC_2345));

	/* Check FFTC PSC status */
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
}

void clean_hw(){
}
