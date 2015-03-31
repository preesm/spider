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
#include "osal.h"
#include "cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ti/drv/qmss/qmss_drv.h>
#include <ti/drv/qmss/qmss_firmware.h>
#include <ti/drv/cppi/cppi_desc.h>

#include <ti/csl/cslr_device.h>
#include <ti/drv/cppi/cppi_drv.h>
#include <ti/drv/qmss/qmss_drv.h>

int data_mem_base;

/* Default QMSS Driver global config */
extern Qmss_GlobalConfigParams qmssGblCfgParams;
/* CPPI device specific configuration */
extern Cppi_GlobalConfigParams cppiGblCfgParams;

void printQueueState(){
	int Queue;

#define PRINT_QUEUE(id) if(Qmss_getQueueEntryCount(id) > 0) \
	printf("%4d: %d\n", id, Qmss_getQueueEntryCount(id))

	printf("\nQueue State:\n");
	/* Special Queues */
	PRINT_QUEUE(QUEUE_TX_FFTC_A);
	PRINT_QUEUE(QUEUE_TX_FFTC_B);

	/* General Purpose Queues */
	for(Queue = QUEUE_FIRST; Queue < QUEUE_LAST; Queue++){
		PRINT_QUEUE(Queue);
	}
	printf("\n");
}

/**
 * Init Queue Manager SUbSystem (QMSS)
 *  - Configure QMSS Driver
 * 	- Define Memory regions
 * 	-
 */
void init_qmss(){
	int result;
    Qmss_InitCfg 			qmss_initCfg;

    /* Descriptor base addresses */
    void* data_desc_base  = (void*)align((int)msmc_mem_base);
    void* ctrl_desc_base  = (void*)align((int)data_desc_base + DATA_DESC_NUM*DATA_DESC_SIZE);
    void* trace_desc_base = (void*)align((int)ctrl_desc_base + CTRL_DESC_NUM*CTRL_DESC_SIZE);
    void* fftc_desc_base  = (void*)align((int)trace_desc_base + TRACE_DESC_NUM*TRACE_DESC_SIZE);
    data_mem_base = align((int)fftc_desc_base + FFTC_DESC_NUM*FFTC_DESC_SIZE);

    /* Initialize QMSS Driver */
    memset (&qmss_initCfg, 0, sizeof (Qmss_InitCfg));

    /* Use internal linking RAM */
	qmss_initCfg.linkingRAM0Base  = 0;
	qmss_initCfg.linkingRAM0Size  = 0;
	qmss_initCfg.linkingRAM1Base  = 0;
	qmss_initCfg.maxDescNum       = DATA_DESC_NUM + CTRL_DESC_NUM + TRACE_DESC_NUM + FFTC_DESC_NUM;

	qmss_initCfg.pdspFirmware[0].pdspId   = Qmss_PdspId_PDSP1;
	qmss_initCfg.pdspFirmware[0].firmware = &acc48_le;
	qmss_initCfg.pdspFirmware[0].size     = sizeof (acc48_le);

    /* Bypass hardware initialization as it is done within Kernel */
    qmss_initCfg.qmssHwStatus     =   QMSS_HW_INIT_COMPLETE;

	if ((result = Qmss_init (&qmss_initCfg, &qmssGblCfgParams)) != QMSS_SOK){
		printf ("initQmss: Error initializing Queue Manager SubSystem, Error code : %d\n", result);
		abort();
	}

	if ((result = Qmss_start ()) != QMSS_SOK){
		printf ("initQmss: Error starting Queue Manager SubSystem, Error code : %d\n", result);
		abort();
	}

    if ((result = Cppi_init (&cppiGblCfgParams)) != CPPI_SOK){
        printf ("Error initializing CPPI LLD, Error code : %d\n", result);
        abort();
    }
}

void clean_qmss(){
	Qmss_Result result;

	/* Exit Cppi */
	Cppi_exit ();

	/* Exit Qmss */
	if ((result = Qmss_exit ()) != 0){
		printf ("Error : Qmss exit error %d\n", result);
		abort();
	}
}
