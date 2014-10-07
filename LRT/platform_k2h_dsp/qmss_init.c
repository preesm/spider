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

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <stdint.h>
#include <ti/csl/device/k2h/src/cslr_device.h>
#include <ti/csl/device/k2h/src/csl_qm_queue.h>
#include <ti/drv/qmss/qmss_drv.h>
#include <ti/drv/qmss/device/k2h/src/qmss_device.c>

#include "qmss.h"

#define CACHE_LINESZ 64

/* Useful Macros*/
#define align(x)   ((x + CACHE_LINESZ) & (~CACHE_LINESZ))
#define TranslateAddress(address, offset, type) address = (type)(((uint8_t*)address) + (uint32_t)offset)

/************************ GLOBAL VARIABLES ********************/

/* Virtual addresses */
static void *descriptors; 	/* Base address of descriptors */
static void *data_mem;				/* Base address of data memory */
static int data_mem_size;			/* Size of data Memory */

/* OSAL Fcts */
void Osal_invalidateCache (void *blockPtr, uint32_t size){}
void Osal_writeBackCache (void *blockPtr, uint32_t size){}

void*  Osal_qmssAccCsEnter ()
	{return(NULL);}
void Osal_qmssAccCsExit (void *  key){}

void*  Osal_qmssCsEnter ()
	{return(NULL);}
void Osal_qmssCsExit (void *  key){}

void Osal_qmssBeginMemAccess (void *blockPtr, uint32_t size)
	{Osal_invalidateCache(blockPtr,size);}
void  Osal_qmssEndMemAccess (void *blockPtr, uint32_t size)
	{Osal_writeBackCache(blockPtr,size);}

/* QMSS global Init */
void initQmss(){
    Qmss_InitCfg                qmss_initCfg;
    Qmss_GlobalConfigParams     qmss_globalCfg;
    int i;

    /* Initialize QMSS */
    memset (&qmss_initCfg, 0, sizeof (Qmss_InitCfg));

    /* Set up QMSS configuration */
    /* Use internal linking RAM */
    qmss_initCfg.linkingRAM0Base  = 0;   
    qmss_initCfg.linkingRAM0Size  = 0;
    qmss_initCfg.linkingRAM1Base  = 0;
    qmss_initCfg.maxDescNum       = CTRL_DESC_NB + DATA_DESC_NB;

    /* Bypass hardware initialization as it is done within Kernel */
    qmss_initCfg.qmssHwStatus     =   QMSS_HW_INIT_COMPLETE;

    qmss_globalCfg = qmssGblCfgParams;

    /* Initialize the Queue Manager */
    i = Qmss_init (&qmss_initCfg, &qmss_globalCfg);
    if (i != QMSS_SOK){
        printf ("initQmss: Error initializing Queue Manager SubSystem, Error code : %d\n", i);
        abort();
    }

    /* Start Queue manager on this core */
    i = Qmss_start ();
    if (i != QMSS_SOK){
        printf ("initQmss: Error starting Queue Manager SubSystem, Error code : %d\n", i);
        abort();
    }
}

static void exitQmss(){
	/* Exit Qmss */
	int i = Qmss_exit ();
    if (i){
        printf ("Error : Qmss exit error code : %d\n", i);
		abort();
    }
}

void* platform_getDataMemAdd(){
	return data_mem;
}

int platform_getDataMemSize(){
	return data_mem_size;
}

static void sysInit (){
//	int i;

	/* Data memory */
	data_mem  = (void*)DATA_BASE;
	data_mem_size = DATA_END - DATA_BASE;
}

static void sysExit (void){
}

void platform_queue_Init(){
    initQmss();
    sysInit();
}

void platform_queue_Exit(){
    sysExit();
    exitQmss();
}
