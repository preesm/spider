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
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern "C"{
#include <ti/drv/qmss/qmss_drv.h>
#include <ti/csl/device/k2h/src/cslr_device.h>
#include <ti/csl/device/k2h/src/csl_qm_queue.h>
#include <ti/drv/qmss/device/k2h/src/qmss_device.c>
}

#include "qmss.h"

#define CACHE_LINESZ 64

/* Useful Macros*/
#define align(x)   ((x + CACHE_LINESZ) & (~CACHE_LINESZ))
#define TranslateAddress(address, offset, type) address = (type)(((uint8_t*)address) + (uint32_t)offset)

/************************ GLOBAL VARIABLES ********************/

/* Virtual addresses */
static void *descriptors; 	/* Base address of descriptors */
static void *dat_mem;		/* Base address of data memory */
static void *qmss_cfg;		/* Base address of QMSS Config registers */
static void *qmss_data;		/* Base address of QMSS Data registers */
static int dat_mem_size;	/* Size of data Memory */

/* Memory regions */
static int memReg_ctrl, memReg_data, memReg_trace;

/* OSAL Fcts */
void* Osal_qmssVirtToPhy (void *ptr){
    if(ptr == NULL)
    	return NULL;
    return (void *)(QMSS_DESC_BASE + ((uint8_t*)ptr - (uint8_t*)descriptors));
}

void* Osal_qmssPhyToVirt (void *ptr){
    if(ptr == NULL)
        return NULL;
    return (void *)((uint8_t*)descriptors + ((int)ptr - QMSS_DESC_BASE));
}

void Osal_invalidateCache (void *blockPtr, uint32_t size){}
void Osal_writeBackCache (void *blockPtr, uint32_t size){}

void*  Osal_qmssAccCsEnter () {return(NULL);}
void Osal_qmssAccCsExit (void *  key){}

void*  Osal_qmssCsEnter () {return(NULL);}
void Osal_qmssCsExit (void *  key){}

void Osal_qmssBeginMemAccess (void *blockPtr, uint32_t size)
	{Osal_invalidateCache(blockPtr,size);}
void  Osal_qmssEndMemAccess (void *blockPtr, uint32_t size)
	{Osal_writeBackCache(blockPtr,size);}

void* Osal_qmssConvertDescVirtToPhy(uint32_t QID, void *descAddr)
	{return Osal_qmssVirtToPhy (descAddr);}
void* Osal_qmssConvertDescPhyToVirt(uint32_t QID, void *descAddr)
	{return Osal_qmssPhyToVirt (descAddr);}

void Osal_cppiCsEnter (uint32_t *key){}
void Osal_cppiCsExit (uint32_t key){}
void Osal_cppiLog ( String fmt, ... ){}

/* QMSS global Init */
static void initQmss(int memFile){
    Qmss_InitCfg                qmss_initCfg;
    Qmss_GlobalConfigParams     qmss_globalCfg;
    int i;

	/* QMSS CFG Regs */
	qmss_cfg = mmap(0, 0x00200000, (PROT_READ|PROT_WRITE), MAP_SHARED, memFile, (off_t)CSL_QMSS_CFG_BASE);
	if (!qmss_cfg){
		printf("ERROR: Failed to map QMSS CFG registers\n");
		abort();
	}

	/* QMSS DATA Regs */
	qmss_data = mmap(0, 0x00100000, (PROT_READ|PROT_WRITE), MAP_SHARED, memFile, (off_t)CSL_QMSS_DATA_BASE);
	if (!qmss_data){
		printf("ERROR: Failed to map QMSS DATA registers\n");
		abort();
	}

    /* Initialize QMSS */
    memset (&qmss_initCfg, 0, sizeof (Qmss_InitCfg));

    /* Set up QMSS configuration */
    /* Use internal linking RAM */
    qmss_initCfg.linkingRAM0Base  = 0;   
    qmss_initCfg.linkingRAM0Size  = 0;
    qmss_initCfg.linkingRAM1Base  = 0;
    qmss_initCfg.maxDescNum       = CTRL_DESC_NB + DATA_DESC_NB + TRACE_DESC_NB;

    /* Bypass hardware initialization as it is done within Kernel */
    qmss_initCfg.qmssHwStatus     =   QMSS_HW_INIT_COMPLETE;

    qmss_globalCfg = qmssGblCfgParams;

    /* Convert address to Virtual address */ 
    for(i=0;i < (int)qmss_globalCfg.maxQueMgrGroups;i++){
        TranslateAddress(qmss_globalCfg.groupRegs[i].qmConfigReg,       qmss_cfg-CSL_QMSS_CFG_BASE,     CSL_Qm_configRegs*);
        TranslateAddress(qmss_globalCfg.groupRegs[i].qmDescReg,         qmss_cfg-CSL_QMSS_CFG_BASE,     CSL_Qm_descriptor_region_configRegs*);
        TranslateAddress(qmss_globalCfg.groupRegs[i].qmQueMgmtReg,      qmss_cfg-CSL_QMSS_CFG_BASE,     CSL_Qm_queue_managementRegs*);
        TranslateAddress(qmss_globalCfg.groupRegs[i].qmQueMgmtProxyReg, qmss_cfg-CSL_QMSS_CFG_BASE,     CSL_Qm_queue_managementRegs*);
        TranslateAddress(qmss_globalCfg.groupRegs[i].qmQueStatReg,      qmss_cfg-CSL_QMSS_CFG_BASE,     CSL_Qm_queue_status_configRegs*);
        TranslateAddress(qmss_globalCfg.groupRegs[i].qmStatusRAM,       qmss_cfg-CSL_QMSS_CFG_BASE,     CSL_Qm_Queue_Status*);
        TranslateAddress(qmss_globalCfg.groupRegs[i].qmQueMgmtDataReg,  qmss_data-CSL_QMSS_DATA_BASE,   CSL_Qm_queue_managementRegs*);

    	/* not supported on k2 hardware, and not used by lld */
    	qmss_globalCfg.groupRegs[i].qmQueMgmtProxyDataReg = NULL;
    }
    
    for(i=0;i < QMSS_MAX_INTD;i++)
    	TranslateAddress(qmss_globalCfg.regs.qmQueIntdReg[i], qmss_cfg-CSL_QMSS_CFG_BASE, CSL_Qm_intdRegs*);


    for(i=0;i < QMSS_MAX_PDSP;i++){
    	TranslateAddress(qmss_globalCfg.regs.qmPdspCmdReg[i],  qmss_cfg-CSL_QMSS_CFG_BASE, volatile uint32_t*);
    	TranslateAddress(qmss_globalCfg.regs.qmPdspCtrlReg[i], qmss_cfg-CSL_QMSS_CFG_BASE, CSL_PdspRegs*);
    	TranslateAddress(qmss_globalCfg.regs.qmPdspIRamReg[i], qmss_cfg-CSL_QMSS_CFG_BASE, volatile uint32_t*);
    }

	TranslateAddress(qmss_globalCfg.regs.qmLinkingRAMReg, 	qmss_cfg-CSL_QMSS_CFG_BASE, volatile uint32_t*);
	TranslateAddress(qmss_globalCfg.regs.qmBaseAddr, 		qmss_cfg-CSL_QMSS_CFG_BASE, void*);

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

static void sysInit (int memFile){
	int i;
    Qmss_MemRegInfo regionCfg;
	void *data_desc;
	void *ctrl_desc;
	void *trace_desc;

	/* Open base address for descriptors */
	descriptors = mmap(0, QMSS_DESC_END-QMSS_DESC_BASE, (PROT_READ|PROT_WRITE), MAP_SHARED, memFile, (off_t)QMSS_DESC_BASE);
	if(descriptors == (void *) -1){
		printf("fw_memMap: Failed to mmap \"dev/mem\" err=%s\n", strerror(errno));
		abort();
	}

	dat_mem = mmap(0, DATA_END-DATA_BASE, (PROT_READ|PROT_WRITE), MAP_SHARED, memFile, (off_t)DATA_BASE);
	if(dat_mem == (void *) -1){
		printf("fw_memMap: Failed to mmap \"dev/mem\" err=%s\n", strerror(errno));
		abort();
	}
	dat_mem_size = (DATA_END-DATA_BASE);

	/* Descriptors */
	ctrl_desc = descriptors;
	data_desc = (void*)(align((int)descriptors + CTRL_DESC_POOL_SIZE));
	trace_desc = (void*)(align((int)data_desc + DATA_DESC_POOL_SIZE));

	if((int)trace_desc - (int)descriptors > QMSS_DESC_END-QMSS_DESC_BASE)
		throw "Alloc to many desc memory";

    /* Setup memory region for Ctrl descriptors */
    memset(&regionCfg, 0, sizeof(regionCfg));
    memset(ctrl_desc, 0, CTRL_DESC_POOL_SIZE);

    regionCfg.descBase 			= (uint32_t*)ctrl_desc;
    regionCfg.descSize 			= CTRL_DESC_SIZE;
    regionCfg.descNum 			= CTRL_DESC_NB;
    regionCfg.manageDescFlag 	= Qmss_ManageDesc_UNMANAGED_DESCRIPTOR;
    regionCfg.memRegion 		= Qmss_MemRegion_MEMORY_REGION_NOT_SPECIFIED;
    regionCfg.startIndex 		= 0;

    memReg_ctrl = Qmss_insertMemoryRegion (&regionCfg);
    if (memReg_ctrl < QMSS_SOK){
        printf ("Error : Inserting Ctrl memory region %d error code : %d\n", regionCfg.memRegion, memReg_ctrl);
        abort();
    }else{
        printf ("Core : Memory region %d inserted\n", memReg_ctrl);
    }

    /* Setup memory region for Data descriptors */
	memset(&regionCfg, 0, sizeof(regionCfg));
	memset (data_desc, 0, DATA_DESC_POOL_SIZE);

	regionCfg.descBase 			= (uint32_t*)data_desc;
	regionCfg.descSize 			= DATA_DESC_SIZE;
	regionCfg.descNum 			= DATA_DESC_NB;
	regionCfg.manageDescFlag 	= Qmss_ManageDesc_UNMANAGED_DESCRIPTOR;
	regionCfg.memRegion 		= Qmss_MemRegion_MEMORY_REGION_NOT_SPECIFIED;
	regionCfg.startIndex 		= 0;

	memReg_data = Qmss_insertMemoryRegion (&regionCfg);
	if (memReg_data < QMSS_SOK){
		printf ("Error : Inserting Data memory region %d error code : %d\n", regionCfg.memRegion, memReg_data);
        abort();
	}else{
		printf ("Core : Memory region %d inserted\n", memReg_data);
	}

    /* Setup memory region for Data descriptors */
	memset(&regionCfg, 0, sizeof(regionCfg));
	memset(trace_desc, 0, TRACE_DESC_POOL_SIZE);

	regionCfg.descBase 			= (uint32_t*)trace_desc;
	regionCfg.descSize 			= TRACE_DESC_SIZE;
	regionCfg.descNum 			= TRACE_DESC_NB;
	regionCfg.manageDescFlag 	= Qmss_ManageDesc_UNMANAGED_DESCRIPTOR;
	regionCfg.memRegion 		= Qmss_MemRegion_MEMORY_REGION_NOT_SPECIFIED;
	regionCfg.startIndex 		= 0;

	memReg_trace = Qmss_insertMemoryRegion (&regionCfg);
	if (memReg_trace < QMSS_SOK){
		printf ("Error : Inserting Data memory region %d error code : %d\n", regionCfg.memRegion, memReg_trace);
        abort();
	}else{
		printf ("Core : Memory region %d inserted\n", memReg_trace);
	}

	/* Emptying Queues */
    for(i=EMPTY_CTRL; i<MAX_QUEUES; i++)
        Qmss_queueEmpty (i);

    /* Setup CTRL descriptors into FREE queue */
    MonoPcktDesc* mono_pkt;
    for (i = 0; i < CTRL_DESC_NB; i++) {
		mono_pkt = (MonoPcktDesc *) ((int)ctrl_desc + (i * CTRL_DESC_SIZE));

		mono_pkt->pkt_return_qmgr = 1;
		mono_pkt->pkt_return_qnum = 1;

		Qmss_queuePushDesc(EMPTY_CTRL,mono_pkt);
	}

    /* Setup DATA descriptors into FREE queue */
    for (i = 0; i < DATA_DESC_NB; i++) {
		mono_pkt = (MonoPcktDesc *) ((int)data_desc + (i * DATA_DESC_SIZE));

		mono_pkt->pkt_return_qmgr = 1;
		mono_pkt->pkt_return_qnum = 1;

		Qmss_queuePushDesc(EMPTY_DATA,mono_pkt);
	}

    /* Setup TRACE descriptors into FREE queue */
    for (i = 0; i < TRACE_DESC_NB; i++) {
		mono_pkt = (MonoPcktDesc *) ((int)trace_desc + (i * TRACE_DESC_SIZE));

		mono_pkt->pkt_return_qmgr = 1;
		mono_pkt->pkt_return_qnum = 1;

		Qmss_queuePushDesc(EMPTY_TRACE,mono_pkt);
	}
}

static void sysExit (void){
	int i;

    /* Emptying all queues */
    for(i=EMPTY_CTRL; i<MAX_QUEUES; i++)
        Qmss_queueEmpty (i);

    /* Remove my regions which were only intstalled from core 0 */
    i = Qmss_removeMemoryRegion (memReg_ctrl, 0);
	if (i){
		printf ("Error : remove memReg_ctrl error code : %d\n", i);
		abort();
	}
	i = Qmss_removeMemoryRegion (memReg_data, 0);
	if (i){
		printf ("Error : remove memReg_data error code : %d\n", i);
		abort();
	}
	i = Qmss_removeMemoryRegion (memReg_trace, 0);
	if (i){
		printf ("Error : remove memReg_data error code : %d\n", i);
		abort();
	}
}

void spider_qmss_init(int memFile, long* data_mem_start, long* data_mem_size){
    initQmss(memFile);
    sysInit(memFile);

    *data_mem_start = (long)dat_mem;
    *data_mem_size = dat_mem_size;
}

void spider_qmss_exit(){
    sysExit();
    exitQmss();
}
